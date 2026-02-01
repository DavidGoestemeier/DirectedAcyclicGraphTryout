/**
 * GraphManager.h - Enhanced DAG Manager with Tags, History, and Modifiers
 * 
 * Phase 2 Features:
 * - GameplayTag container for conditional logic
 * - History node support for temporal mechanics
 * - Modifier system integration
 * - Enhanced JSON serialization for visualizer
 */

#pragma once

#include "StatNode.h"
#include "HistoryNode.h"
#include "GameplayTag.h"
#include "Modifier.h"
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <functional>

using json = nlohmann::json;

// Forward declarations
class Item;
class Aura;

class GraphManager {
private:
    std::unordered_map<std::string, StatNode::Ptr> m_nodes;
    std::unordered_map<std::string, std::shared_ptr<HistoryStatNode>> m_historyNodes;
    std::vector<std::string> m_nodeOrder;
    
    // Tag system
    GameplayTagContainer m_activeTags;
    
    // Modifier aggregators per stat
    std::unordered_map<std::string, ModifierAggregator> m_modifiers;
    
    // "Recently" trackers
    RecentlyTracker m_critRecently{4.0};
    RecentlyTracker m_blockRecently{4.0};
    RecentlyTracker m_killRecently{4.0};
    
    bool m_hasChanges = false;
    TimePoint m_lastTick;

public:
    GraphManager() : m_lastTick(Clock::now()) {
        // When tags change, mark all conditional nodes as dirty
        m_activeTags.SetOnChangedCallback([this]() {
            MarkAllConditionalNodesDirty();
            m_hasChanges = true;
        });
    }

    // ─────────────────────────────────────────────────────────────
    // Node Management
    // ─────────────────────────────────────────────────────────────

    StatNode::Ptr CreateBaseStat(const std::string& id, 
                                  const std::string& label, 
                                  double baseValue = 0.0,
                                  const std::string& category = "") {
        auto node = std::make_shared<StatNode>(id, label, baseValue, StatNode::NodeType::Base);
        if (!category.empty()) node->SetCategory(category);
        RegisterNode(node);
        return node;
    }

    StatNode::Ptr CreateDerivedStat(const std::string& id, 
                                     const std::string& label,
                                     std::initializer_list<StatNode::Ptr> parents,
                                     StatNode::CalculationFunc calcFunc = nullptr,
                                     const std::string& category = "") {
        auto node = std::make_shared<StatNode>(id, label, 0.0, StatNode::NodeType::Derived);
        node->AddParents(parents);
        if (calcFunc) {
            node->SetCalculationFunc(std::move(calcFunc));
        }
        if (!category.empty()) node->SetCategory(category);
        RegisterNode(node);
        return node;
    }

    /**
     * Create a history node for temporal mechanics
     */
    std::shared_ptr<HistoryStatNode> CreateHistoryNode(const std::string& id,
                                                         const std::string& label,
                                                         double windowSeconds = 4.0,
                                                         const std::string& category = "") {
        auto node = std::make_shared<HistoryStatNode>(id, label, windowSeconds);
        if (!category.empty()) node->SetCategory(category);
        node->SetIsHistoryNode(true);
        m_historyNodes[id] = node;
        RegisterNode(node);
        return node;
    }

    /**
     * Create a derived stat that depends on modifiers with conditions
     */
    StatNode::Ptr CreateModifiedStat(const std::string& id,
                                      const std::string& label,
                                      double baseValue,
                                      const std::string& category = "") {
        auto node = std::make_shared<StatNode>(id, label, baseValue, StatNode::NodeType::Derived);
        if (!category.empty()) node->SetCategory(category);
        
        // Calculation function uses the modifier aggregator
        node->SetCalculationFunc([this, id, baseValue](const std::vector<double>& parents) {
            ModifierContext ctx;
            ctx.tags = &m_activeTags;
            ctx.graph = this;
            
            double parentSum = 0.0;
            for (double v : parents) parentSum += v;
            
            return m_modifiers[id].Calculate(baseValue + parentSum, ctx);
        });
        
        RegisterNode(node);
        return node;
    }

    StatNode::Ptr GetNode(const std::string& id) const {
        auto it = m_nodes.find(id);
        if (it != m_nodes.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::shared_ptr<HistoryStatNode> GetHistoryNode(const std::string& id) const {
        auto it = m_historyNodes.find(id);
        if (it != m_historyNodes.end()) {
            return it->second;
        }
        return nullptr;
    }

    bool SetNodeValue(const std::string& id, double value) {
        if (auto node = GetNode(id)) {
            node->SetBaseValue(value);
            m_hasChanges = true;
            return true;
        }
        return false;
    }

    const std::vector<std::string>& GetNodeIds() const {
        return m_nodeOrder;
    }

    // ─────────────────────────────────────────────────────────────
    // Tag Management
    // ─────────────────────────────────────────────────────────────

    GameplayTagContainer& GetTags() { return m_activeTags; }
    const GameplayTagContainer& GetTags() const { return m_activeTags; }

    void AddTag(const std::string& tagName) {
        m_activeTags.AddTag(tagName);
    }

    void AddTag(const GameplayTag& tag) {
        m_activeTags.AddTag(tag);
    }

    void RemoveTag(const std::string& tagName) {
        m_activeTags.RemoveTag(tagName);
    }

    void RemoveTag(const GameplayTag& tag) {
        m_activeTags.RemoveTag(tag);
    }

    bool HasTag(const std::string& tagName) const {
        return m_activeTags.HasTag(tagName);
    }

    bool HasTag(const GameplayTag& tag) const {
        return m_activeTags.HasTag(tag);
    }

    // ─────────────────────────────────────────────────────────────
    // Modifier Management
    // ─────────────────────────────────────────────────────────────

    void AddModifier(Modifier::Ptr modifier) {
        const std::string& targetId = modifier->GetTargetStatId();
        m_modifiers[targetId].AddModifier(modifier);
        
        // Mark the target stat as dirty
        if (auto node = GetNode(targetId)) {
            node->MarkDirty();
            m_hasChanges = true;
        }
    }

    void RemoveModifier(const std::string& modifierId) {
        for (auto& [statId, aggregator] : m_modifiers) {
            aggregator.RemoveModifier(modifierId);
        }
        m_hasChanges = true;
    }

    void RemoveModifiersBySource(const std::string& sourceId) {
        for (auto& [statId, aggregator] : m_modifiers) {
            aggregator.RemoveModifiersBySource(sourceId);
            if (auto node = GetNode(statId)) {
                node->MarkDirty();
            }
        }
        m_hasChanges = true;
    }

    ModifierAggregator* GetModifiers(const std::string& statId) {
        auto it = m_modifiers.find(statId);
        return (it != m_modifiers.end()) ? &it->second : nullptr;
    }

    // ─────────────────────────────────────────────────────────────
    // "Recently" Trackers
    // ─────────────────────────────────────────────────────────────

    void TriggerCritRecently() {
        m_critRecently.Trigger();
        m_hasChanges = true;
        MarkAllConditionalNodesDirty();
    }

    void TriggerBlockRecently() {
        m_blockRecently.Trigger();
        m_hasChanges = true;
    }

    void TriggerKillRecently() {
        m_killRecently.Trigger();
        m_hasChanges = true;
    }

    bool CritRecently() const { return m_critRecently.IsRecent(); }
    bool BlockedRecently() const { return m_blockRecently.IsRecent(); }
    bool KilledRecently() const { return m_killRecently.IsRecent(); }

    double GetSecondsSinceCrit() const { return m_critRecently.GetSecondsSinceTrigger(); }
    double GetSecondsSinceBlock() const { return m_blockRecently.GetSecondsSinceTrigger(); }
    double GetSecondsSinceKill() const { return m_killRecently.GetSecondsSinceTrigger(); }

    // ─────────────────────────────────────────────────────────────
    // History Node Event Recording
    // ─────────────────────────────────────────────────────────────

    void RecordDamage(const std::string& historyNodeId, double amount, const std::string& damageType = "") {
        if (auto histNode = GetHistoryNode(historyNodeId)) {
            histNode->RecordEvent(amount, damageType);
            m_hasChanges = true;
        }
    }

    // ─────────────────────────────────────────────────────────────
    // Tick / Update
    // ─────────────────────────────────────────────────────────────

    /**
     * Call every frame to update history nodes and check expirations
     */
    void Tick() {
        bool anyChanged = false;
        
        // Tick all history nodes
        for (auto& [id, histNode] : m_historyNodes) {
            if (histNode->Tick()) {
                anyChanged = true;
            }
        }
        
        // Check if "recently" states changed
        // (This is approximate - could track previous state)
        anyChanged = anyChanged || m_critRecently.IsRecent() || 
                     m_blockRecently.IsRecent() || m_killRecently.IsRecent();
        
        if (anyChanged) {
            m_hasChanges = true;
        }
        
        m_lastTick = Clock::now();
    }

    // ─────────────────────────────────────────────────────────────
    // Change Tracking
    // ─────────────────────────────────────────────────────────────

    bool HasChanges() {
        bool changes = m_hasChanges;
        m_hasChanges = false;
        return changes;
    }

    void MarkChanged() {
        m_hasChanges = true;
    }

    void MarkAllConditionalNodesDirty() {
        for (const auto& id : m_nodeOrder) {
            auto node = m_nodes.at(id);
            if (!node->GetConditionalParents().empty() || 
                m_modifiers.count(id) > 0) {
                node->MarkDirty();
            }
        }
    }

    // ─────────────────────────────────────────────────────────────
    // JSON Serialization for Visualization
    // ─────────────────────────────────────────────────────────────

    json ToJson() const {
        json result;
        json nodes = json::array();
        json edges = json::array();
        json tags = json::array();

        // Serialize all nodes
        for (const auto& id : m_nodeOrder) {
            auto node = m_nodes.at(id);
            
            json nodeJson;
            nodeJson["id"] = node->GetId();
            nodeJson["label"] = node->GetLabel();
            nodeJson["value"] = node->GetCachedValue();
            nodeJson["isDirty"] = node->IsDirty();
            nodeJson["category"] = node->GetCategory();
            nodeJson["isHistory"] = node->IsHistoryNode();
            
            // Node type
            switch (node->GetType()) {
                case StatNode::NodeType::Base:
                    nodeJson["type"] = "base";
                    break;
                case StatNode::NodeType::History:
                    nodeJson["type"] = "history";
                    // Add decay info for history nodes
                    if (auto histNode = GetHistoryNode(id)) {
                        nodeJson["eventCount"] = histNode->GetEventCount();
                        nodeJson["window"] = histNode->GetDefaultWindow();
                    }
                    break;
                default:
                    nodeJson["type"] = "derived";
            }
            
            // Check if this stat has modifiers
            auto modIt = m_modifiers.find(id);
            if (modIt != m_modifiers.end()) {
                json modArray = json::array();
                ModifierContext ctx;
                ctx.tags = &m_activeTags;
                ctx.graph = this;
                
                for (const auto& mod : modIt->second.GetModifiers()) {
                    json modJson;
                    modJson["id"] = mod->GetId();
                    modJson["type"] = ModifierTypeToString(mod->GetType());
                    modJson["value"] = mod->GetStaticValue();
                    modJson["source"] = mod->GetSourceId();
                    modJson["active"] = mod->IsConditionMet();
                    modJson["hasCondition"] = mod->HasCondition();
                    modJson["description"] = mod->GetDescription();
                    modArray.push_back(modJson);
                }
                nodeJson["modifiers"] = modArray;
            }
            
            nodes.push_back(nodeJson);

            // Serialize edges (including conditional state)
            auto edgeStates = node->GetEdgeStates();
            for (const auto& edge : edgeStates) {
                json edgeJson;
                edgeJson["from"] = edge.fromId;
                edgeJson["to"] = edge.toId;
                edgeJson["active"] = edge.isActive;
                edgeJson["conditional"] = edge.isConditional;
                edgeJson["condition"] = edge.condition;
                edges.push_back(edgeJson);
            }
        }

        // Serialize active tags
        for (const auto& tagName : m_activeTags.GetAllTagNames()) {
            tags.push_back(tagName);
        }

        // "Recently" states
        json recentlyStates;
        recentlyStates["crit"] = m_critRecently.IsRecent();
        recentlyStates["critRemaining"] = m_critRecently.GetRemainingTime();
        recentlyStates["block"] = m_blockRecently.IsRecent();
        recentlyStates["blockRemaining"] = m_blockRecently.GetRemainingTime();
        recentlyStates["kill"] = m_killRecently.IsRecent();
        recentlyStates["killRemaining"] = m_killRecently.GetRemainingTime();

        result["nodes"] = nodes;
        result["edges"] = edges;
        result["tags"] = tags;
        result["recently"] = recentlyStates;
        result["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        return result;
    }

    std::string ToJsonString() const {
        return ToJson().dump();
    }

    std::string RecalculateAndSerialize() {
        // Update modifier conditions
        ModifierContext ctx;
        ctx.tags = &m_activeTags;
        ctx.graph = this;
        
        for (auto& [statId, aggregator] : m_modifiers) {
            for (const auto& mod : aggregator.GetModifiers()) {
                mod->EvaluateCondition(ctx);
            }
        }
        
        // Trigger recalculation on all derived nodes
        for (const auto& id : m_nodeOrder) {
            auto node = m_nodes.at(id);
            if (node->GetType() != StatNode::NodeType::Base) {
                node->GetValue();
            }
        }
        return ToJsonString();
    }

private:
    void RegisterNode(StatNode::Ptr node) {
        const auto& id = node->GetId();
        if (m_nodes.count(id) > 0) {
            throw std::runtime_error("Node with ID '" + id + "' already exists!");
        }
        m_nodes[id] = node;
        m_nodeOrder.push_back(id);
        m_hasChanges = true;
    }
};

// ═══════════════════════════════════════════════════════════════
// Producer Implementations (defined here due to GraphManager dependency)
// ═══════════════════════════════════════════════════════════════

#include "Producer.h"

inline void Item::Activate(GraphManager& graph) {
    if (m_isEquipped) return;
    m_isEquipped = true;
    
    // Add all modifiers
    for (auto& mod : m_modifiers) {
        graph.AddModifier(mod);
    }
    
    // Grant tags
    for (const auto& tag : m_grantedTags) {
        graph.AddTag(tag);
    }
}

inline void Item::Deactivate(GraphManager& graph) {
    if (!m_isEquipped) return;
    m_isEquipped = false;
    
    // Remove all modifiers from this item
    graph.RemoveModifiersBySource(m_id);
    
    // Remove granted tags
    for (const auto& tag : m_grantedTags) {
        graph.RemoveTag(tag);
    }
}

inline void Aura::Activate(GraphManager& graph) {
    if (m_isActive) return;
    m_isActive = true;
    m_activatedAt = Clock::now();
    
    // Grant tags
    for (const auto& tag : m_grantedTags) {
        graph.AddTag(tag);
    }
    
    // Add modifiers
    for (auto& mod : m_modifiers) {
        graph.AddModifier(mod);
    }
}

inline void Aura::Deactivate(GraphManager& graph) {
    if (!m_isActive) return;
    m_isActive = false;
    
    // Remove tags
    for (const auto& tag : m_grantedTags) {
        graph.RemoveTag(tag);
    }
    
    // Remove modifiers
    graph.RemoveModifiersBySource(m_id);
}

// CombatEvent implementations
inline void CombatEvent::TakeDamage(GraphManager& graph, double amount, DamageType type) {
    std::string historyId = std::string(DamageTypeToString(type)) + "DamageTaken";
    
    // Try type-specific history first, then general
    if (auto node = graph.GetHistoryNode(historyId)) {
        node->RecordEvent(amount, DamageTypeToString(type));
    }
    if (auto node = graph.GetHistoryNode("damageTaken")) {
        node->RecordEvent(amount, DamageTypeToString(type));
    }
    
    graph.MarkChanged();
}

inline void CombatEvent::DealDamage(GraphManager& graph, double amount, DamageType type) {
    std::string historyId = std::string(DamageTypeToString(type)) + "DamageDealt";
    
    if (auto node = graph.GetHistoryNode(historyId)) {
        node->RecordEvent(amount, DamageTypeToString(type));
    }
    if (auto node = graph.GetHistoryNode("damageDealt")) {
        node->RecordEvent(amount, DamageTypeToString(type));
    }
    
    graph.MarkChanged();
}

inline void CombatEvent::TriggerCrit(GraphManager& graph) {
    graph.TriggerCritRecently();
    
    if (auto node = graph.GetHistoryNode("critHistory")) {
        node->RecordEvent(1.0, "crit");
    }
}

inline void CombatEvent::TriggerBlock(GraphManager& graph) {
    graph.TriggerBlockRecently();
    
    if (auto node = graph.GetHistoryNode("blockHistory")) {
        node->RecordEvent(1.0, "block");
    }
}

inline void CombatEvent::RecordKill(GraphManager& graph) {
    graph.TriggerKillRecently();
    
    if (auto node = graph.GetHistoryNode("killHistory")) {
        node->RecordEvent(1.0, "kill");
    }
}

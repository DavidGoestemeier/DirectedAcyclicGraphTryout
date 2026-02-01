/**
 * GraphManager.h - Manages the DAG of StatNodes
 * 
 * Provides:
 * - Node creation and retrieval
 * - Graph traversal
 * - JSON serialization for visualization
 */

#pragma once

#include "StatNode.h"
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>

using json = nlohmann::json;

class GraphManager {
private:
    std::unordered_map<std::string, StatNode::Ptr> m_nodes;
    std::vector<std::string> m_nodeOrder; // Maintain insertion order for consistent output
    
    bool m_hasChanges = false; // Track if graph needs to broadcast

public:
    GraphManager() = default;

    // ─────────────────────────────────────────────────────────────
    // Node Management
    // ─────────────────────────────────────────────────────────────

    /**
     * Create and register a new Base stat node
     */
    StatNode::Ptr CreateBaseStat(const std::string& id, 
                                  const std::string& label, 
                                  double baseValue = 0.0) {
        auto node = std::make_shared<StatNode>(id, label, baseValue, StatNode::NodeType::Base);
        RegisterNode(node);
        return node;
    }

    /**
     * Create and register a new Derived stat node
     */
    StatNode::Ptr CreateDerivedStat(const std::string& id, 
                                     const std::string& label,
                                     std::initializer_list<StatNode::Ptr> parents,
                                     StatNode::CalculationFunc calcFunc = nullptr) {
        auto node = std::make_shared<StatNode>(id, label, 0.0, StatNode::NodeType::Derived);
        node->AddParents(parents);
        if (calcFunc) {
            node->SetCalculationFunc(std::move(calcFunc));
        }
        RegisterNode(node);
        return node;
    }

    /**
     * Get a node by its ID
     */
    StatNode::Ptr GetNode(const std::string& id) const {
        auto it = m_nodes.find(id);
        if (it != m_nodes.end()) {
            return it->second;
        }
        return nullptr;
    }

    /**
     * Set a node's base value by ID (convenience method)
     */
    bool SetNodeValue(const std::string& id, double value) {
        if (auto node = GetNode(id)) {
            node->SetBaseValue(value);
            m_hasChanges = true;
            return true;
        }
        return false;
    }

    /**
     * Get all node IDs
     */
    const std::vector<std::string>& GetNodeIds() const {
        return m_nodeOrder;
    }

    /**
     * Check and clear the changes flag
     */
    bool HasChanges() {
        bool changes = m_hasChanges;
        m_hasChanges = false;
        return changes;
    }

    /**
     * Force mark as changed (for initial broadcast)
     */
    void MarkChanged() {
        m_hasChanges = true;
    }

    // ─────────────────────────────────────────────────────────────
    // JSON Serialization for Visualization
    // ─────────────────────────────────────────────────────────────

    /**
     * Serialize the entire graph to JSON for the D3.js visualizer
     * 
     * Format:
     * {
     *   "nodes": [
     *     { "id": "str", "label": "STR", "value": 50, "isDirty": false, "type": "base" },
     *     ...
     *   ],
     *   "edges": [
     *     { "from": "str", "to": "totalAttack" },
     *     ...
     *   ]
     * }
     */
    json ToJson() const {
        json result;
        json nodes = json::array();
        json edges = json::array();

        // Serialize all nodes
        for (const auto& id : m_nodeOrder) {
            auto node = m_nodes.at(id);
            
            json nodeJson;
            nodeJson["id"] = node->GetId();
            nodeJson["label"] = node->GetLabel();
            nodeJson["value"] = node->GetCachedValue();
            nodeJson["isDirty"] = node->IsDirty();
            nodeJson["type"] = (node->GetType() == StatNode::NodeType::Base) ? "base" : "derived";
            
            nodes.push_back(nodeJson);

            // Serialize edges (parent -> this node)
            for (const auto& weakParent : node->GetParents()) {
                if (auto parent = weakParent.lock()) {
                    json edge;
                    edge["from"] = parent->GetId();
                    edge["to"] = node->GetId();
                    edges.push_back(edge);
                }
            }
        }

        result["nodes"] = nodes;
        result["edges"] = edges;
        result["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        return result;
    }

    /**
     * Get JSON as string (for WebSocket transmission)
     */
    std::string ToJsonString() const {
        return ToJson().dump();
    }

    /**
     * Recalculate all dirty nodes and return updated JSON
     * This triggers the "pull" pattern on all nodes
     */
    std::string RecalculateAndSerialize() {
        // Trigger recalculation on all derived nodes
        for (const auto& id : m_nodeOrder) {
            auto node = m_nodes.at(id);
            if (node->GetType() == StatNode::NodeType::Derived) {
                node->GetValue(); // This triggers recalculation if dirty
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

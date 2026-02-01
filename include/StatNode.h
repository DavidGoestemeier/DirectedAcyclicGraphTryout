/**
 * StatNode.h - Core DAG Node for RPG Attribute System
 * 
 * Implements the "Pull" pattern with Dirty Flag optimization:
 * - GetValue() lazily recalculates only when dirty
 * - SetBaseValue() pushes invalidation to all dependents
 * 
 * Phase 2: Added support for conditional modifiers and edge tracking
 */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <optional>

// Edge state for visualization
struct EdgeState {
    std::string fromId;
    std::string toId;
    bool isActive;          // Condition met?
    bool isConditional;     // Has a condition at all?
    std::string condition;  // Description of condition
};

class StatNode : public std::enable_shared_from_this<StatNode> {
public:
    using Ptr = std::shared_ptr<StatNode>;
    using WeakPtr = std::weak_ptr<StatNode>;
    using CalculationFunc = std::function<double(const std::vector<double>&)>;

    enum class NodeType {
        Base,       // Source node (no parents) - Gray
        Derived,    // Calculated from parents - Green/Red
        History     // Temporal node with decay - Purple
    };

    // For conditional edges
    struct ConditionalParent {
        WeakPtr node;
        std::function<bool()> condition;
        std::string conditionDesc;
        bool lastActive = true;
    };

private:
    std::string m_id;
    std::string m_label;
    double m_baseValue;
    double m_cachedValue;
    bool m_isDirty;
    NodeType m_type;

    std::vector<WeakPtr> m_parents;                       // Dependencies (nodes we pull from)
    std::vector<ConditionalParent> m_conditionalParents;  // Conditional dependencies
    std::vector<WeakPtr> m_children;                      // Dependents (nodes that depend on us)
    
    CalculationFunc m_calculateFunc;                      // Custom calculation logic
    
    // Metadata for visualization
    std::string m_nodeCategory;                           // e.g., "offense", "defense", "utility"
    bool m_isHistoryNode = false;

public:
    /**
     * Constructor for a StatNode
     */
    StatNode(const std::string& id, 
             const std::string& label, 
             double baseValue = 0.0,
             NodeType type = NodeType::Base)
        : m_id(id)
        , m_label(label)
        , m_baseValue(baseValue)
        , m_cachedValue(baseValue)
        , m_isDirty(type == NodeType::Derived)
        , m_type(type)
        , m_calculateFunc(nullptr)
        , m_nodeCategory("unknown")
    {}

    // ─────────────────────────────────────────────────────────────
    // Metadata Accessors
    // ─────────────────────────────────────────────────────────────

    const std::string& GetCategory() const { return m_nodeCategory; }
    bool IsHistoryNode() const { return m_isHistoryNode; }
    void SetCategory(const std::string& cat) { m_nodeCategory = cat; }
    void SetIsHistoryNode(bool isHistory) { 
        m_isHistoryNode = isHistory; 
        if (isHistory) m_type = NodeType::History; 
    }
    
    const std::vector<ConditionalParent>& GetConditionalParents() const { 
        return m_conditionalParents; 
    }

    // ─────────────────────────────────────────────────────────────
    // Core Getters
    // ─────────────────────────────────────────────────────────────
    
    const std::string& GetId() const { return m_id; }
    const std::string& GetLabel() const { return m_label; }
    double GetBaseValue() const { return m_baseValue; }
    bool IsDirty() const { return m_isDirty; }
    NodeType GetType() const { return m_type; }
    const std::vector<WeakPtr>& GetParents() const { return m_parents; }
    const std::vector<WeakPtr>& GetChildren() const { return m_children; }

    /**
     * THE CORE "PULL" PATTERN
     * 
     * If clean: return cached value immediately (O(1))
     * If dirty: recursively pull from parents, recalculate, cache, mark clean
     */
    double GetValue() {
        if (m_type == NodeType::Base || m_type == NodeType::History) {
            return m_baseValue;
        }

        if (!m_isDirty) {
            return m_cachedValue;
        }

        // Pull values from all parents
        std::vector<double> parentValues;
        parentValues.reserve(m_parents.size() + m_conditionalParents.size());
        
        for (auto& weakParent : m_parents) {
            if (auto parent = weakParent.lock()) {
                parentValues.push_back(parent->GetValue());
            }
        }

        // Also pull from active conditional parents
        for (auto& cp : m_conditionalParents) {
            if (cp.condition && cp.condition()) {
                if (auto parent = cp.node.lock()) {
                    parentValues.push_back(parent->GetValue());
                }
            }
        }

        // Calculate new value using custom function or sum by default
        if (m_calculateFunc) {
            m_cachedValue = m_calculateFunc(parentValues);
        } else {
            m_cachedValue = 0.0;
            for (double v : parentValues) {
                m_cachedValue += v;
            }
        }

        m_isDirty = false;
        return m_cachedValue;
    }

    /**
     * Get cached value without recalculation (for visualization)
     */
    double GetCachedValue() const {
        if (m_type == NodeType::Base || m_type == NodeType::History) {
            return m_baseValue;
        }
        return m_cachedValue;
    }

    // ─────────────────────────────────────────────────────────────
    // Setters & Modifiers
    // ─────────────────────────────────────────────────────────────

    /**
     * Set the base value and PUSH invalidation to all dependents
     */
    void SetBaseValue(double value) {
        if (m_baseValue == value) return;
        
        m_baseValue = value;
        if (m_type == NodeType::Base || m_type == NodeType::History) {
            m_cachedValue = value;
        }
        
        // PUSH: Invalidate all children recursively
        InvalidateChildren();
    }

    /**
     * Set the calculation function for derived nodes
     */
    void SetCalculationFunc(CalculationFunc func) {
        m_calculateFunc = std::move(func);
    }

    /**
     * Manually mark this node as dirty
     */
    void MarkDirty() {
        if (!m_isDirty) {
            m_isDirty = true;
            InvalidateChildren();
        }
    }

    // ─────────────────────────────────────────────────────────────
    // Graph Structure Management
    // ─────────────────────────────────────────────────────────────

    /**
     * Add a parent (dependency) to this node
     */
    void AddParent(Ptr parent) {
        if (!parent) return;
        
        m_parents.push_back(parent);
        parent->m_children.push_back(weak_from_this());
        
        if (m_type == NodeType::Derived) {
            m_isDirty = true;
        }
    }

    /**
     * Add multiple parents at once
     */
    void AddParents(std::initializer_list<Ptr> parents) {
        for (auto& parent : parents) {
            AddParent(parent);
        }
    }

    /**
     * Add a conditional parent - only contributes when condition is true
     */
    void AddConditionalParent(Ptr parent, 
                               std::function<bool()> condition,
                               const std::string& conditionDesc = "") {
        if (!parent) return;
        
        ConditionalParent cp;
        cp.node = parent;
        cp.condition = std::move(condition);
        cp.conditionDesc = conditionDesc;
        cp.lastActive = true;
        m_conditionalParents.push_back(std::move(cp));
        parent->m_children.push_back(weak_from_this());
        m_isDirty = true;
    }

    /**
     * Get edge states for visualization
     */
    std::vector<EdgeState> GetEdgeStates() const {
        std::vector<EdgeState> edges;
        
        // Regular parents (always active)
        for (const auto& weakParent : m_parents) {
            if (auto parent = weakParent.lock()) {
                edges.push_back({parent->GetId(), m_id, true, false, ""});
            }
        }
        
        // Conditional parents
        for (const auto& cp : m_conditionalParents) {
            if (auto parent = cp.node.lock()) {
                bool active = cp.condition ? cp.condition() : true;
                edges.push_back({parent->GetId(), m_id, active, true, cp.conditionDesc});
            }
        }
        
        return edges;
    }

private:
    /**
     * PUSH INVALIDATION: Recursively mark all children as dirty
     */
    void InvalidateChildren() {
        for (auto& weakChild : m_children) {
            if (auto child = weakChild.lock()) {
                if (!child->m_isDirty) {
                    child->m_isDirty = true;
                    child->InvalidateChildren();
                }
            }
        }
    }
};

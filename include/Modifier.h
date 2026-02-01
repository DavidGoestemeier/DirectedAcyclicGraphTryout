/**
 * Modifier.h - Advanced Modifier System with Conditions
 * 
 * Implements Path of Exile-style modifiers with:
 * - Conditional application (tag-based, state-based)
 * - Different modifier types (Flat, Increased, More)
 * - Priority and stacking rules
 */

#pragma once

#include "GameplayTag.h"
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <optional>

// Forward declarations
class GraphManager;
class StatNode;

// ═══════════════════════════════════════════════════════════════
// ModifierType - How the modifier applies to the stat
// ═══════════════════════════════════════════════════════════════

enum class ModifierType {
    Flat,           // Adds a fixed value (applied first)
    Increased,      // Additive percentage (e.g., +50% = 0.5, stacks additively)
    More,           // Multiplicative (e.g., +20% More = 1.2, each applies separately)
    Override        // Completely replaces the value
};

inline const char* ModifierTypeToString(ModifierType type) {
    switch (type) {
        case ModifierType::Flat: return "Flat";
        case ModifierType::Increased: return "Increased";
        case ModifierType::More: return "More";
        case ModifierType::Override: return "Override";
        default: return "Unknown";
    }
}

// ═══════════════════════════════════════════════════════════════
// ModifierCondition - Lambda-based conditional logic
// ═══════════════════════════════════════════════════════════════

/**
 * Context passed to condition checks
 * Provides access to tags, stats, and other game state
 */
struct ModifierContext {
    const GameplayTagContainer* tags = nullptr;
    const GraphManager* graph = nullptr;
    double currentTime = 0.0;
    
    bool HasTag(const std::string& tagName) const {
        return tags && tags->HasTag(tagName);
    }
    
    bool HasTag(const GameplayTag& tag) const {
        return tags && tags->HasTag(tag);
    }
    
    bool HasAnyTag(const std::vector<GameplayTag>& tagList) const {
        return tags && tags->HasAnyTag(tagList);
    }
    
    bool HasAllTags(const std::vector<GameplayTag>& tagList) const {
        return tags && tags->HasAllTags(tagList);
    }
};

using ConditionFunc = std::function<bool(const ModifierContext&)>;
using ValueFunc = std::function<double(const ModifierContext&)>;

// ═══════════════════════════════════════════════════════════════
// Modifier - A single modifier that can affect a stat
// ═══════════════════════════════════════════════════════════════

class Modifier {
public:
    using Ptr = std::shared_ptr<Modifier>;

private:
    std::string m_id;
    std::string m_description;
    std::string m_sourceId;           // Who/what created this modifier
    std::string m_targetStatId;       // Which stat this modifies
    
    ModifierType m_type;
    double m_value;
    int m_priority;                   // Higher = applied later within same type
    
    ConditionFunc m_condition;        // Optional condition
    ValueFunc m_dynamicValue;         // Optional dynamic value calculation
    
    bool m_isActive = true;           // Can be toggled
    bool m_conditionMet = true;       // Cached condition result

public:
    Modifier(const std::string& id,
             const std::string& targetStatId,
             ModifierType type,
             double value,
             const std::string& description = "")
        : m_id(id)
        , m_description(description)
        , m_targetStatId(targetStatId)
        , m_type(type)
        , m_value(value)
        , m_priority(0)
        , m_condition(nullptr)
        , m_dynamicValue(nullptr)
    {}

    // ─────────────────────────────────────────────────────────────
    // Builder Pattern for Fluent API
    // ─────────────────────────────────────────────────────────────

    Modifier& WithSource(const std::string& sourceId) {
        m_sourceId = sourceId;
        return *this;
    }

    Modifier& WithPriority(int priority) {
        m_priority = priority;
        return *this;
    }

    Modifier& WithDescription(const std::string& desc) {
        m_description = desc;
        return *this;
    }

    /**
     * Set a condition that must be true for the modifier to apply
     * Example: .WithCondition([](const ModifierContext& ctx) { 
     *     return ctx.HasTag("State.DualWielding"); 
     * })
     */
    Modifier& WithCondition(ConditionFunc condition) {
        m_condition = std::move(condition);
        return *this;
    }

    /**
     * Convenience: Condition based on having a specific tag
     */
    Modifier& RequiresTag(const std::string& tagName) {
        m_condition = [tagName](const ModifierContext& ctx) {
            return ctx.HasTag(tagName);
        };
        return *this;
    }

    Modifier& RequiresTag(const GameplayTag& tag) {
        m_condition = [tag](const ModifierContext& ctx) {
            return ctx.HasTag(tag);
        };
        return *this;
    }

    /**
     * Convenience: Condition based on having ANY of the tags
     */
    Modifier& RequiresAnyTag(std::vector<GameplayTag> tags) {
        m_condition = [tags = std::move(tags)](const ModifierContext& ctx) {
            return ctx.HasAnyTag(tags);
        };
        return *this;
    }

    /**
     * Set a dynamic value function (calculated each time)
     * The static value becomes the default/fallback
     */
    Modifier& WithDynamicValue(ValueFunc valueFunc) {
        m_dynamicValue = std::move(valueFunc);
        return *this;
    }

    // ─────────────────────────────────────────────────────────────
    // Evaluation
    // ─────────────────────────────────────────────────────────────

    /**
     * Check if the condition is met (and cache result)
     */
    bool EvaluateCondition(const ModifierContext& ctx) {
        if (!m_isActive) {
            m_conditionMet = false;
            return false;
        }
        
        if (m_condition) {
            m_conditionMet = m_condition(ctx);
        } else {
            m_conditionMet = true;  // No condition = always active
        }
        
        return m_conditionMet;
    }

    /**
     * Get the modifier value (static or dynamic)
     */
    double GetValue(const ModifierContext& ctx) const {
        if (!m_conditionMet) return 0.0;
        
        if (m_dynamicValue) {
            return m_dynamicValue(ctx);
        }
        return m_value;
    }

    /**
     * Get the effective value to apply (considering type)
     * For Increased: returns the percentage (e.g., 0.5 for +50%)
     * For More: returns the multiplier (e.g., 1.2 for 20% more)
     */
    double GetEffectiveValue(const ModifierContext& ctx) const {
        double val = GetValue(ctx);
        switch (m_type) {
            case ModifierType::More:
                return 1.0 + val;  // Convert to multiplier
            default:
                return val;
        }
    }

    // ─────────────────────────────────────────────────────────────
    // Getters
    // ─────────────────────────────────────────────────────────────

    const std::string& GetId() const { return m_id; }
    const std::string& GetDescription() const { return m_description; }
    const std::string& GetSourceId() const { return m_sourceId; }
    const std::string& GetTargetStatId() const { return m_targetStatId; }
    ModifierType GetType() const { return m_type; }
    double GetStaticValue() const { return m_value; }
    int GetPriority() const { return m_priority; }
    bool IsActive() const { return m_isActive; }
    bool IsConditionMet() const { return m_conditionMet; }
    bool HasCondition() const { return m_condition != nullptr; }
    bool HasDynamicValue() const { return m_dynamicValue != nullptr; }

    void SetActive(bool active) { m_isActive = active; }
    void SetValue(double value) { m_value = value; }
};

// ═══════════════════════════════════════════════════════════════
// ModifierAggregator - Combines multiple modifiers for a stat
// ═══════════════════════════════════════════════════════════════

class ModifierAggregator {
private:
    std::vector<Modifier::Ptr> m_modifiers;

public:
    void AddModifier(Modifier::Ptr mod) {
        m_modifiers.push_back(std::move(mod));
        // Sort by type then priority
        std::sort(m_modifiers.begin(), m_modifiers.end(),
            [](const Modifier::Ptr& a, const Modifier::Ptr& b) {
                if (a->GetType() != b->GetType()) {
                    return static_cast<int>(a->GetType()) < static_cast<int>(b->GetType());
                }
                return a->GetPriority() < b->GetPriority();
            });
    }

    void RemoveModifier(const std::string& modifierId) {
        m_modifiers.erase(
            std::remove_if(m_modifiers.begin(), m_modifiers.end(),
                [&](const Modifier::Ptr& m) { return m->GetId() == modifierId; }),
            m_modifiers.end()
        );
    }

    void RemoveModifiersBySource(const std::string& sourceId) {
        m_modifiers.erase(
            std::remove_if(m_modifiers.begin(), m_modifiers.end(),
                [&](const Modifier::Ptr& m) { return m->GetSourceId() == sourceId; }),
            m_modifiers.end()
        );
    }

    /**
     * Calculate the final value after applying all modifiers
     * Order: Base + Flat -> * (1 + sum(Increased)) -> * each(More)
     */
    double Calculate(double baseValue, const ModifierContext& ctx) {
        double flat = 0.0;
        double increased = 0.0;  // Sum of all increased percentages
        double more = 1.0;       // Product of all more multipliers
        std::optional<double> override;

        for (auto& mod : m_modifiers) {
            if (!mod->EvaluateCondition(ctx)) continue;
            
            double val = mod->GetValue(ctx);
            
            switch (mod->GetType()) {
                case ModifierType::Flat:
                    flat += val;
                    break;
                case ModifierType::Increased:
                    increased += val;
                    break;
                case ModifierType::More:
                    more *= (1.0 + val);
                    break;
                case ModifierType::Override:
                    override = val;
                    break;
            }
        }

        if (override.has_value()) {
            return *override;
        }

        return (baseValue + flat) * (1.0 + increased) * more;
    }

    /**
     * Get all modifiers (for inspection/debugging)
     */
    const std::vector<Modifier::Ptr>& GetModifiers() const {
        return m_modifiers;
    }

    /**
     * Get active modifier count
     */
    size_t GetActiveCount(const ModifierContext& ctx) {
        size_t count = 0;
        for (auto& mod : m_modifiers) {
            if (mod->EvaluateCondition(ctx)) count++;
        }
        return count;
    }

    void Clear() {
        m_modifiers.clear();
    }
};

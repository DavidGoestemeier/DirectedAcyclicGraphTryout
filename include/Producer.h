/**
 * Producer.h - Game Objects that Produce Modifiers
 *
 * Demonstrates the "Multi-Target Producer Pattern":
 * - Items: Apply modifiers on Equip(), remove on Unequip()
 * - Auras: Add tags and modifiers when active
 * - CombatEvents: Push data into History nodes
 */

#pragma once

#include "Modifier.h"
#include "HistoryNode.h"
#include "GameplayTag.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

// Forward declaration
class GraphManager;

// ═══════════════════════════════════════════════════════════════
// IProducer - Interface for objects that produce graph effects
// ═══════════════════════════════════════════════════════════════

class IProducer {
public:
    virtual ~IProducer() = default;

    virtual const std::string& GetId() const = 0;
    virtual const std::string& GetName() const = 0;
    virtual bool IsActive() const = 0;

    virtual void Activate(GraphManager& graph) = 0;
    virtual void Deactivate(GraphManager& graph) = 0;
};

// ═══════════════════════════════════════════════════════════════
// Item - Equipment that applies modifiers to multiple stats
// ═══════════════════════════════════════════════════════════════

class Item :  public IProducer {
public:
    using Ptr = std::shared_ptr<Item>;

    enum class Slot {
        MainHand,
        OffHand,
        Helmet,
        BodyArmor,
        Gloves,
        Boots,
        Ring1,
        Ring2,
        Amulet,
        Belt
    };

private:
    std::string m_id;
    std::string m_name;
    Slot m_slot;
    bool m_isEquipped = false;

    // Modifiers this item provides
    std::vector<Modifier::Ptr> m_modifiers;

    // Tags this item grants when equipped
    std::vector<GameplayTag> m_grantedTags;

public:
    Item(const std::string& id, const std::string& name, Slot slot)
        : m_id(id), m_name(name), m_slot(slot)
    {}

    // ─────────────────────────────────────────────────────────────
    // Builder Pattern for Item Creation
    // ─────────────────────────────────────────────────────────────

    /**
     * Add a modifier to this item
     * Example: .AddModifier("attackSpeed", ModifierType::Increased, 0.15)
     */
    Item& AddModifier(const std::string& targetStat,
                      ModifierType type,
                      double value,
                      const std::string& description = "") {
        auto mod = std::make_shared<Modifier>(
            m_id + "_" + targetStat + "_mod",
            targetStat,
            type,
            value,
            description.empty() ? (m_name + " " + targetStat) : description
        );
        mod->WithSource(m_id);
        m_modifiers.push_back(mod);
        return *this;
    }

    /**
     * Add a conditional modifier
     */
    Item& AddConditionalModifier(const std::string& targetStat,
                                  ModifierType type,
                                  double value,
                                  ConditionFunc condition,
                                  const std::string& description = "") {
        auto mod = std::make_shared<Modifier>(
            m_id + "_" + targetStat + "_cond_mod",
            targetStat,
            type,
            value,
            description
        );
        mod->WithSource(m_id);
        mod->WithCondition(std::move(condition));
        m_modifiers.push_back(mod);
        return *this;
    }

    /**
     * Add a tag that this item grants when equipped
     */
    Item& GrantsTag(const GameplayTag& tag) {
        m_grantedTags.push_back(tag);
        return *this;
    }

    Item& GrantsTag(const std::string& tagName) {
        m_grantedTags.emplace_back(tagName);
        return *this;
    }

    // ─────────────────────────────────────────────────────────────
    // IProducer Interface
    // ─────────────────────────────────────────────────────────────

    const std::string& GetId() const override { return m_id; }
    const std::string& GetName() const override { return m_name; }
    bool IsActive() const override { return m_isEquipped; }
    Slot GetSlot() const { return m_slot; }

    void Activate(GraphManager& graph) override;   // Defined after GraphManager
    void Deactivate(GraphManager& graph) override;

    void Equip(GraphManager& graph) { Activate(graph); }
    void Unequip(GraphManager& graph) { Deactivate(graph); }

    const std::vector<Modifier::Ptr>& GetModifiers() const { return m_modifiers; }
    const std::vector<GameplayTag>& GetGrantedTags() const { return m_grantedTags; }
};

// ═══════════════════════════════════════════════════════════════
// Aura - Buff/Debuff that adds tags and modifiers while active
// ═══════════════════════════════════════════════════════════════

class Aura : public IProducer {
public:
    using Ptr = std::shared_ptr<Aura>;

private:
    std::string m_id;
    std::string m_name;
    bool m_isActive = false;
    double m_duration = -1.0;  // -1 = permanent
    TimePoint m_activatedAt;

    // Tags this aura grants while active
    std::vector<GameplayTag> m_grantedTags;

    // Modifiers active while aura is up
    std::vector<Modifier::Ptr> m_modifiers;

public:
    Aura(const std::string& id, const std::string& name, double duration = -1.0)
        : m_id(id), m_name(name), m_duration(duration)
    {}

    // ─────────────────────────────────────────────────────────────
    // Builder Pattern
    // ─────────────────────────────────────────────────────────────

    Aura& GrantsTag(const GameplayTag& tag) {
        m_grantedTags.push_back(tag);
        return *this;
    }

    Aura& GrantsTag(const std::string& tagName) {
        m_grantedTags.emplace_back(tagName);
        return *this;
    }

    Aura& AddModifier(const std::string& targetStat,
                      ModifierType type,
                      double value,
                      const std::string& description = "") {
        auto mod = std::make_shared<Modifier>(
            m_id + "_" + targetStat + "_aura",
            targetStat,
            type,
            value,
            description.empty() ? (m_name + " " + targetStat) : description
        );
        mod->WithSource(m_id);
        m_modifiers.push_back(mod);
        return *this;
    }

    // ─────────────────────────────────────────────────────────────
    // IProducer Interface
    // ─────────────────────────────────────────────────────────────

    const std::string& GetId() const override { return m_id; }
    const std::string& GetName() const override { return m_name; }
    bool IsActive() const override { return m_isActive; }

    void Activate(GraphManager& graph) override;
    void Deactivate(GraphManager& graph) override;

    /**
     * Check if aura has expired
     */
    bool HasExpired() const {
        if (m_duration < 0) return false;  // Permanent
        if (!m_isActive) return true;
        return Duration(Clock::now() - m_activatedAt).count() > m_duration;
    }

    double GetRemainingDuration() const {
        if (m_duration < 0) return std::numeric_limits<double>::infinity();
        if (!m_isActive) return 0.0;
        double elapsed = Duration(Clock::now() - m_activatedAt).count();
        return (elapsed < m_duration) ? (m_duration - elapsed) : 0.0;
    }

    const std::vector<GameplayTag>& GetGrantedTags() const { return m_grantedTags; }
    const std::vector<Modifier::Ptr>& GetModifiers() const { return m_modifiers; }
};

// ═══════════════════════════════════════════════════════════════
// DamageType - Enum for damage categorization
// ═══════════════════════════════════════════════════════════════

enum class DamageType {
    Physical,
    Fire,
    Cold,
    Lightning,
    Chaos,

    COUNT
};

inline const char* DamageTypeToString(DamageType type) {
    switch (type) {
        case DamageType::Physical: return "Physical";
        case DamageType::Fire: return "Fire";
        case DamageType::Cold: return "Cold";
        case DamageType::Lightning: return "Lightning";
        case DamageType::Chaos: return "Chaos";
        default: return "Unknown";
    }
}

inline GameplayTag DamageTypeToTag(DamageType type) {
    switch (type) {
        case DamageType::Physical: return Tags::DamagePhysical;
        case DamageType::Fire: return Tags::DamageFire;
        case DamageType::Cold: return Tags::DamageCold;
        case DamageType::Lightning: return Tags::DamageLightning;
        case DamageType::Chaos: return Tags::DamageChaos;
        default: return GameplayTag();
    }
}

// ═══════════════════════════════════════════════════════════════
// CombatEvent - Pushes data into history nodes
// ═══════════════════════════════════════════════════════════════

class CombatEvent {
public:
    /**
     * Record damage taken - pushes to the appropriate history node
     */
    static void TakeDamage(GraphManager& graph, double amount, DamageType type);

    /**
     * Record damage dealt
     */
    static void DealDamage(GraphManager& graph, double amount, DamageType type);

    /**
     * Record a critical hit (for "crit recently" mechanics)
     */
    static void TriggerCrit(GraphManager& graph);

    /**
     * Record a block
     */
    static void TriggerBlock(GraphManager& graph);

    /**
     * Record an enemy kill
     */
    static void RecordKill(GraphManager& graph);
};

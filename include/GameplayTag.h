/**
 * GameplayTag.h - Lightweight Tag System for Conditional Logic
 * 
 * Provides efficient hash-based tags for representing game states:
 * - State.DualWielding
 * - Buff.PurityOfElements
 * - Damage.Fire
 * 
 * Inspired by Unreal Engine's Gameplay Tag system but lightweight.
 */

#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════
// GameplayTag - Interned string with fast hash comparison
// ═══════════════════════════════════════════════════════════════

class GameplayTag {
private:
    std::size_t m_hash;
    std::string m_name;

    // Static intern pool for string deduplication
    static std::unordered_map<std::size_t, std::string>& GetInternPool() {
        static std::unordered_map<std::size_t, std::string> pool;
        return pool;
    }

public:
    GameplayTag() : m_hash(0), m_name("") {}
    
    explicit GameplayTag(const std::string& name) 
        : m_hash(std::hash<std::string>{}(name))
        , m_name(name) 
    {
        // Intern the string
        GetInternPool()[m_hash] = name;
    }

    // Fast hash-based comparison
    bool operator==(const GameplayTag& other) const { return m_hash == other.m_hash; }
    bool operator!=(const GameplayTag& other) const { return m_hash != other.m_hash; }
    bool operator<(const GameplayTag& other) const { return m_hash < other.m_hash; }

    std::size_t GetHash() const { return m_hash; }
    const std::string& GetName() const { return m_name; }
    bool IsValid() const { return m_hash != 0; }

    // Check if this tag matches a parent hierarchy
    // e.g., "Damage.Fire.Ignite" matches "Damage.Fire" and "Damage"
    bool MatchesParent(const GameplayTag& parent) const {
        if (m_name.length() < parent.m_name.length()) return false;
        return m_name.compare(0, parent.m_name.length(), parent.m_name) == 0 &&
               (m_name.length() == parent.m_name.length() || m_name[parent.m_name.length()] == '.');
    }
};

// Hash specialization for use in containers
namespace std {
    template<> struct hash<GameplayTag> {
        std::size_t operator()(const GameplayTag& tag) const {
            return tag.GetHash();
        }
    };
}

// ═══════════════════════════════════════════════════════════════
// GameplayTagContainer - Efficient tag set with query methods
// ═══════════════════════════════════════════════════════════════

class GameplayTagContainer {
private:
    std::unordered_set<GameplayTag> m_tags;
    mutable std::mutex m_mutex;
    
    // Change callback for dirty propagation
    std::function<void()> m_onChangedCallback;

public:
    GameplayTagContainer() = default;

    void SetOnChangedCallback(std::function<void()> callback) {
        m_onChangedCallback = std::move(callback);
    }

    // ─────────────────────────────────────────────────────────────
    // Tag Management
    // ─────────────────────────────────────────────────────────────

    void AddTag(const GameplayTag& tag) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto result = m_tags.insert(tag);
        if (result.second && m_onChangedCallback) {
            m_onChangedCallback();
        }
    }

    void AddTag(const std::string& tagName) {
        AddTag(GameplayTag(tagName));
    }

    void RemoveTag(const GameplayTag& tag) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto count = m_tags.erase(tag);
        if (count > 0 && m_onChangedCallback) {
            m_onChangedCallback();
        }
    }

    void RemoveTag(const std::string& tagName) {
        RemoveTag(GameplayTag(tagName));
    }

    void ClearAllTags() {
        std::lock_guard<std::mutex> lock(m_mutex);
        bool hadTags = !m_tags.empty();
        m_tags.clear();
        if (hadTags && m_onChangedCallback) {
            m_onChangedCallback();
        }
    }

    // ─────────────────────────────────────────────────────────────
    // Query Methods
    // ─────────────────────────────────────────────────────────────

    bool HasTag(const GameplayTag& tag) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tags.count(tag) > 0;
    }

    bool HasTag(const std::string& tagName) const {
        return HasTag(GameplayTag(tagName));
    }

    // Check if ANY of the provided tags are present
    bool HasAnyTag(const std::vector<GameplayTag>& tags) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& tag : tags) {
            if (m_tags.count(tag) > 0) return true;
        }
        return false;
    }

    // Check if ALL of the provided tags are present
    bool HasAllTags(const std::vector<GameplayTag>& tags) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& tag : tags) {
            if (m_tags.count(tag) == 0) return false;
        }
        return true;
    }

    // Check if any tag matches a parent hierarchy
    bool HasTagMatching(const GameplayTag& parent) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& tag : m_tags) {
            if (tag.MatchesParent(parent)) return true;
        }
        return false;
    }

    // Get all active tags
    std::vector<GameplayTag> GetAllTags() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return std::vector<GameplayTag>(m_tags.begin(), m_tags.end());
    }

    std::vector<std::string> GetAllTagNames() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> names;
        names.reserve(m_tags.size());
        for (const auto& tag : m_tags) {
            names.push_back(tag.GetName());
        }
        // Sort alphabetically for consistent display
        std::sort(names.begin(), names.end());
        return names;
    }

    size_t Count() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tags.size();
    }
};

// ═══════════════════════════════════════════════════════════════
// Common Tag Definitions (compile-time constants)
// ═══════════════════════════════════════════════════════════════

namespace Tags {
    // State Tags
    inline const GameplayTag DualWielding("State.DualWielding");
    inline const GameplayTag Blocking("State.Blocking");
    inline const GameplayTag Channeling("State.Channeling");
    inline const GameplayTag Moving("State.Moving");
    inline const GameplayTag LowLife("State.LowLife");
    inline const GameplayTag FullLife("State.FullLife");

    // Buff Tags
    inline const GameplayTag PurityOfElements("Buff.PurityOfElements");
    inline const GameplayTag Fortify("Buff.Fortify");
    inline const GameplayTag Onslaught("Buff.Onslaught");
    inline const GameplayTag Elusive("Buff.Elusive");

    // Damage Type Tags
    inline const GameplayTag DamageFire("Damage.Fire");
    inline const GameplayTag DamageCold("Damage.Cold");
    inline const GameplayTag DamageLightning("Damage.Lightning");
    inline const GameplayTag DamagePhysical("Damage.Physical");
    inline const GameplayTag DamageChaos("Damage.Chaos");

    // Equipment Tags
    inline const GameplayTag WeaponSword("Equipment.Weapon.Sword");
    inline const GameplayTag WeaponAxe("Equipment.Weapon.Axe");
    inline const GameplayTag WeaponWand("Equipment.Weapon.Wand");
    inline const GameplayTag ShieldEquipped("Equipment.Shield");

    // Skill Tags
    inline const GameplayTag SkillMelee("Skill.Melee");
    inline const GameplayTag SkillSpell("Skill.Spell");
    inline const GameplayTag SkillProjectile("Skill.Projectile");
}

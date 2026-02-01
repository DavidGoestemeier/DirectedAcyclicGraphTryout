/**
 * Main.cpp - Phase 2: Advanced DAG RPG Attribute System
 * 
 * Demonstrates Path of Exile-style mechanics:
 * - Temporal "Recently" mechanics with decay
 * - Conditional modifiers based on tags
 * - Multi-target item producers
 * - History nodes with rolling windows
 * 
 * Scenarios implemented:
 * 1. CritChance: +50% if dealt crit recently (4s window)
 * 2. BlockChance: +1% per 200 Fire damage taken recently
 * 3. PhysToLightning: +20% if Purity of Elements, +10% if dual wielding
 * 4. Mana: Base 100 with 10% increased modifier
 */

// Prevent Windows min/max macro conflicts
#define NOMINMAX

#include "StatNode.h"
#include "GraphManagerV2.h"
#include "WebSocketServer.h"
#include "Producer.h"

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <cmath>
#include <iomanip>
#include <algorithm>

// ─────────────────────────────────────────────────────────────
// Global State
// ─────────────────────────────────────────────────────────────

std::atomic<bool> g_running{true};
std::atomic<bool> g_autoMode{false};
std::atomic<double> g_autoSpeed{1.0};

// Equipped items and active auras (for demo)
std::vector<std::shared_ptr<Item>> g_equippedItems;
std::vector<std::shared_ptr<Aura>> g_activeAuras;

// ─────────────────────────────────────────────────────────────
// Item Factory - Creates demo items
// ─────────────────────────────────────────────────────────────

std::shared_ptr<Item> CreateDualDaggers() {
    auto item = std::make_shared<Item>("dualDaggers", "Dual Daggers", Item::Slot::MainHand);
    
    // Multi-target modifier: Attack Speed AND Cast Speed
    item->AddModifier("attackSpeed", ModifierType::Increased, 0.15, "+15% Attack Speed");
    item->AddModifier("castSpeed", ModifierType::Increased, 0.10, "+10% Cast Speed");
    item->AddModifier("critChance", ModifierType::Flat, 3.0, "+3% Base Crit Chance");
    
    // Grants the dual wielding tag
    item->GrantsTag(Tags::DualWielding);
    
    return item;
}

std::shared_ptr<Item> CreateFireShield() {
    auto item = std::make_shared<Item>("fireShield", "Phoenix Shield", Item::Slot::OffHand);
    
    item->AddModifier("fireRes", ModifierType::Flat, 30.0, "+30% Fire Resistance");
    item->AddModifier("blockChance", ModifierType::Flat, 25.0, "+25% Block Chance");
    item->GrantsTag(Tags::ShieldEquipped);
    
    return item;
}

std::shared_ptr<Item> CreateManaRing() {
    auto item = std::make_shared<Item>("manaRing", "Sapphire Ring", Item::Slot::Ring1);
    
    item->AddModifier("maxMana", ModifierType::Increased, 0.10, "+10% Increased Maximum Mana");
    item->AddModifier("maxMana", ModifierType::Flat, 25.0, "+25 Maximum Mana");
    
    return item;
}

std::shared_ptr<Item> CreateCritGloves() {
    auto item = std::make_shared<Item>("critGloves", "Assassin's Gloves", Item::Slot::Gloves);
    
    // Conditional modifier: extra crit if dual wielding
    item->AddConditionalModifier(
        "critChance",
        ModifierType::Increased,
        0.30,  // 30% increased
        [](const ModifierContext& ctx) {
            return ctx.HasTag("State.DualWielding");
        },
        "+30% Increased Crit Chance while Dual Wielding"
    );
    
    // Always-on modifier
    item->AddModifier("attackSpeed", ModifierType::Increased, 0.08, "+8% Attack Speed");
    
    return item;
}

// ─────────────────────────────────────────────────────────────
// Aura Factory - Creates demo auras/buffs
// ─────────────────────────────────────────────────────────────

std::shared_ptr<Aura> CreatePurityOfElements() {
    auto aura = std::make_shared<Aura>("purityOfElements", "Purity of Elements", -1.0);
    
    aura->GrantsTag(Tags::PurityOfElements);
    aura->AddModifier("fireRes", ModifierType::Flat, 15.0, "Purity: +15% Fire Res");
    aura->AddModifier("coldRes", ModifierType::Flat, 15.0, "Purity: +15% Cold Res");
    aura->AddModifier("lightningRes", ModifierType::Flat, 15.0, "Purity: +15% Lightning Res");
    
    return aura;
}

std::shared_ptr<Aura> CreateOnslaught(double duration = 4.0) {
    auto aura = std::make_shared<Aura>("onslaught", "Onslaught", duration);
    
    aura->GrantsTag(Tags::Onslaught);
    aura->AddModifier("attackSpeed", ModifierType::Increased, 0.20, "Onslaught: +20% Attack Speed");
    aura->AddModifier("moveSpeed", ModifierType::Increased, 0.20, "Onslaught: +20% Move Speed");
    
    return aura;
}

// ─────────────────────────────────────────────────────────────
// Graph Construction - Phase 2 Complex Mechanics
// ─────────────────────────────────────────────────────────────

void BuildAdvancedGraph(GraphManager& graph) {
    std::cout << "\n[Graph] Building Phase 2 advanced stat graph...\n";

    // ═══════════════════════════════════════════════════════════
    // HISTORY NODES (Temporal Dependencies)
    // ═══════════════════════════════════════════════════════════
    
    // Fire damage taken recently (4 second window)
    auto fireDamageTaken = graph.CreateHistoryNode(
        "FireDamageTaken", "Fire DMG Taken", 4.0, "defense"
    );
    
    // All damage taken (for general "damage taken recently" checks)
    auto allDamageTaken = graph.CreateHistoryNode(
        "damageTaken", "Total DMG Taken", 4.0, "defense"
    );
    
    // Crit history (for counting crits)
    auto critHistory = graph.CreateHistoryNode(
        "critHistory", "Crits Recently", 4.0, "offense"
    );

    // ═══════════════════════════════════════════════════════════
    // BASE STATS
    // ═══════════════════════════════════════════════════════════
    
    auto baseCritChance = graph.CreateBaseStat("baseCritChance", "Base Crit %", 5.0, "offense");
    auto baseBlockChance = graph.CreateBaseStat("baseBlockChance", "Base Block %", 0.0, "defense");
    auto basePhysToLight = graph.CreateBaseStat("basePhysToLight", "Base P->L Conv", 0.0, "conversion");
    auto baseMana = graph.CreateBaseStat("baseMana", "Base Mana", 100.0, "resource");
    auto baseLife = graph.CreateBaseStat("baseLife", "Base Life", 50.0, "resource");
    auto baseEnergyShield = graph.CreateBaseStat("baseEnergyShield", "Base ES", 0.0, "resource");
    auto baseAccuracy = graph.CreateBaseStat("baseAccuracy", "Base Accuracy", 100.0, "offense");
    auto baseEvasion = graph.CreateBaseStat("baseEvasion", "Base Evasion", 50.0, "defense");
    auto baseMeleePhysDmg = graph.CreateBaseStat("baseMeleePhysDmg", "Base Melee Phys", 100.0, "offense");
    auto baseAttackSpeed = graph.CreateBaseStat("baseAttackSpeed", "Base Atk Spd", 1.0, "offense");
    auto baseCastSpeed = graph.CreateBaseStat("baseCastSpeed", "Base Cast Spd", 1.0, "offense");
    auto baseMoveSpeed = graph.CreateBaseStat("baseMoveSpeed", "Base Move Spd", 100.0, "utility");
    
    // ═══════════════════════════════════════════════════════════
    // CORE ATTRIBUTES (Strength, Dexterity, Intelligence)
    // ═══════════════════════════════════════════════════════════
    
    auto strength = graph.CreateBaseStat("strength", "Strength", 20.0, "attribute");
    auto dexterity = graph.CreateBaseStat("dexterity", "Dexterity", 20.0, "attribute");
    auto intelligence = graph.CreateBaseStat("intelligence", "Intelligence", 20.0, "attribute");

    // ═══════════════════════════════════════════════════════════
    // ATTRIBUTE-DERIVED STATS
    // Every 10 Strength → +5 max life, +2% melee phys damage
    // Every 10 Dexterity → +20 accuracy, +2% evasion
    // Every 10 Intelligence → +5 max mana, +2% energy shield
    // ═══════════════════════════════════════════════════════════
    
    // Maximum Life: Base + (STR / 10) * 5
    auto maxLife = graph.CreateDerivedStat(
        "maxLife", "Maximum Life",
        {baseLife, strength},
        [](const std::vector<double>& parents) {
            double base = parents[0];
            double str = parents[1];
            double strBonus = std::floor(str / 10.0) * 5.0;
            return base + strBonus;
        },
        "resource"
    );
    
    // Melee Physical Damage: Base * (1 + (STR / 10) * 0.02)
    auto meleePhysDmg = graph.CreateDerivedStat(
        "meleePhysDmg", "Melee Phys DMG",
        {baseMeleePhysDmg, strength},
        [](const std::vector<double>& parents) {
            double base = parents[0];
            double str = parents[1];
            double strIncrease = std::floor(str / 10.0) * 0.02;
            return base * (1.0 + strIncrease);
        },
        "offense"
    );
    
    // Accuracy Rating: Base + (DEX / 10) * 20
    auto accuracy = graph.CreateDerivedStat(
        "accuracy", "Accuracy Rating",
        {baseAccuracy, dexterity},
        [](const std::vector<double>& parents) {
            double base = parents[0];
            double dex = parents[1];
            double dexBonus = std::floor(dex / 10.0) * 20.0;
            return base + dexBonus;
        },
        "offense"
    );
    
    // Evasion Rating: Base * (1 + (DEX / 10) * 0.02)
    auto evasion = graph.CreateDerivedStat(
        "evasion", "Evasion Rating",
        {baseEvasion, dexterity},
        [](const std::vector<double>& parents) {
            double base = parents[0];
            double dex = parents[1];
            double dexIncrease = std::floor(dex / 10.0) * 0.02;
            return base * (1.0 + dexIncrease);
        },
        "defense"
    );
    
    // Maximum Mana: Base + (INT / 10) * 5, then apply modifiers
    auto maxMana = graph.CreateModifiedStat("maxMana", "Maximum Mana", 100.0, "resource");
    maxMana->AddParent(baseMana);
    maxMana->AddParent(intelligence);
    maxMana->SetCalculationFunc([&graph](const std::vector<double>& parents) {
        double base = parents[0];
        double intel = parents[1];
        double intBonus = std::floor(intel / 10.0) * 5.0;
        double preModValue = base + intBonus;
        
        // Apply modifiers
        ModifierContext ctx;
        ctx.tags = &graph.GetTags();
        ctx.graph = &graph;
        
        if (auto* agg = graph.GetModifiers("maxMana")) {
            return agg->Calculate(preModValue, ctx);
        }
        return preModValue;
    });
    
    // Energy Shield: Base * (1 + (INT / 10) * 0.02)
    auto energyShield = graph.CreateDerivedStat(
        "energyShield", "Energy Shield",
        {baseEnergyShield, intelligence},
        [](const std::vector<double>& parents) {
            double base = parents[0];
            double intel = parents[1];
            double intIncrease = std::floor(intel / 10.0) * 0.02;
            return base * (1.0 + intIncrease);
        },
        "resource"
    );
    
    // Resistances
    auto baseFireRes = graph.CreateBaseStat("baseFireRes", "Base Fire Res", 0.0, "defense");
    auto baseColdRes = graph.CreateBaseStat("baseColdRes", "Base Cold Res", 0.0, "defense");
    auto baseLightRes = graph.CreateBaseStat("baseLightRes", "Base Light Res", 0.0, "defense");

    // ═══════════════════════════════════════════════════════════
    // SCENARIO 1: CritChance with "Recently" Modifier
    // Base 5% + 50% MORE if dealt crit recently (4s window)
    // ═══════════════════════════════════════════════════════════
    
    auto critChance = graph.CreateModifiedStat("critChance", "Crit Chance %", 5.0, "offense");
    critChance->AddParent(baseCritChance);
    
    // Create the "crit recently" modifier
    auto critRecentlyMod = std::make_shared<Modifier>(
        "critRecently_bonus",
        "critChance",
        ModifierType::More,
        0.50,  // 50% MORE crit chance
        "+50% MORE Crit if Crit Recently"
    );
    critRecentlyMod->WithSource("innate");
    critRecentlyMod->WithCondition([&graph](const ModifierContext&) {
        return graph.CritRecently();
    });
    graph.AddModifier(critRecentlyMod);

    // ═══════════════════════════════════════════════════════════
    // SCENARIO 2: BlockChance from Accumulated Damage
    // +1% Block per 200 Fire Damage taken recently
    // ═══════════════════════════════════════════════════════════
    
    auto blockChance = graph.CreateModifiedStat("blockChance", "Block Chance %", 0.0, "defense");
    blockChance->AddParent(baseBlockChance);
    blockChance->AddParent(fireDamageTaken);
    blockChance->SetCalculationFunc([&graph](const std::vector<double>& parents) {
        double base = parents[0];
        double fireDmg = parents[1];
        // +1% per 200 fire damage taken recently
        double bonus = std::floor(fireDmg / 200.0);
        double preModValue = base + bonus;
        
        // Apply modifiers (e.g., from Shield)
        ModifierContext ctx;
        ctx.tags = &graph.GetTags();
        ctx.graph = &graph;
        
        if (auto* agg = graph.GetModifiers("blockChance")) {
            return agg->Calculate(preModValue, ctx);
        }
        return preModValue;
    });

    // ═══════════════════════════════════════════════════════════
    // SCENARIO 3: Conditional Tag Logic (Phys to Lightning)
    // +20% if Purity of Elements, +10% if Dual Wielding
    // ═══════════════════════════════════════════════════════════
    
    auto physToLightning = graph.CreateModifiedStat(
        "physToLightning", "Phys->Light %", 0.0, "conversion"
    );
    physToLightning->AddParent(basePhysToLight);
    
    // Purity of Elements modifier
    auto purityConvMod = std::make_shared<Modifier>(
        "purity_phys_light",
        "physToLightning",
        ModifierType::Flat,
        20.0,  // +20% conversion
        "+20% Phys to Lightning (Purity of Elements)"
    );
    purityConvMod->WithSource("purityOfElements");
    purityConvMod->RequiresTag(Tags::PurityOfElements);
    graph.AddModifier(purityConvMod);
    
    // Dual Wielding modifier
    auto dualWieldConvMod = std::make_shared<Modifier>(
        "dualwield_phys_light",
        "physToLightning",
        ModifierType::Flat,
        10.0,  // +10% conversion
        "+10% Phys to Lightning (Dual Wielding)"
    );
    dualWieldConvMod->WithSource("innate");
    dualWieldConvMod->RequiresTag(Tags::DualWielding);
    graph.AddModifier(dualWieldConvMod);

    // ═══════════════════════════════════════════════════════════
    // ATTACK SPEED (Modified by items and auras)
    // ═══════════════════════════════════════════════════════════
    
    auto attackSpeed = graph.CreateModifiedStat("attackSpeed", "Attack Speed", 1.0, "offense");
    attackSpeed->AddParent(baseAttackSpeed);
    
    // ═══════════════════════════════════════════════════════════
    // CAST SPEED
    // ═══════════════════════════════════════════════════════════
    
    auto castSpeed = graph.CreateModifiedStat("castSpeed", "Cast Speed", 1.0, "offense");
    castSpeed->AddParent(baseCastSpeed);

    // ═══════════════════════════════════════════════════════════
    // MOVE SPEED
    // ═══════════════════════════════════════════════════════════
    
    auto moveSpeed = graph.CreateModifiedStat("moveSpeed", "Move Speed", 100.0, "utility");
    moveSpeed->AddParent(baseMoveSpeed);

    // ═══════════════════════════════════════════════════════════
    // RESISTANCES (Modified)
    // ═══════════════════════════════════════════════════════════
    
    auto fireRes = graph.CreateModifiedStat("fireRes", "Fire Resistance", 0.0, "defense");
    fireRes->AddParent(baseFireRes);
    
    auto coldRes = graph.CreateModifiedStat("coldRes", "Cold Resistance", 0.0, "defense");
    coldRes->AddParent(baseColdRes);
    
    auto lightningRes = graph.CreateModifiedStat("lightningRes", "Lightning Res", 0.0, "defense");
    lightningRes->AddParent(baseLightRes);

    // ═══════════════════════════════════════════════════════════
    // DERIVED: Effective DPS 
    // Uses meleePhysDmg (already scaled by STR), crit chance, attack speed
    // This mirrors PoE's damage calculation where melee damage is 
    // affected by Strength via the inherent +2% per 10 STR bonus
    // ═══════════════════════════════════════════════════════════
    
    auto effectiveDPS = graph.CreateDerivedStat(
        "effectiveDPS", "Effective DPS",
        {meleePhysDmg, critChance, attackSpeed, accuracy},
        [](const std::vector<double>& parents) {
            double dmg = parents[0];          // Melee phys already has STR scaling
            double crit = parents[1] / 100.0; // Convert to decimal
            double atkSpd = parents[2];
            double acc = parents[3];
            
            // Hit chance (simplified: acc / (acc + 100))
            double hitChance = std::min(0.95, acc / (acc + 500.0));
            
            // DPS = Damage * HitChance * (1 + crit * critMulti) * attackSpeed
            double critMulti = 1.5;  // 50% more damage on crit
            return dmg * hitChance * (1.0 + crit * (critMulti - 1.0)) * atkSpd;
        },
        "offense"
    );

    std::cout << "[Graph] Created " << graph.GetNodeIds().size() << " stat nodes.\n";
    std::cout << "[Graph] Phase 2 features enabled:\n";
    std::cout << "  - History nodes for temporal mechanics\n";
    std::cout << "  - Conditional modifiers with tag checks\n";
    std::cout << "  - Multi-target item producers\n";
}

// ─────────────────────────────────────────────────────────────
// Console Commands
// ─────────────────────────────────────────────────────────────

void PrintHelp() {
    std::cout << "\n";
    std::cout << "================================================================\n";
    std::cout << "       DAG RPG ATTRIBUTE SYSTEM - PHASE 2 COMMANDS\n";
    std::cout << "================================================================\n";
    std::cout << "\n";
    std::cout << "  BASIC COMMANDS\n";
    std::cout << "    set <stat> <value>  - Set a base stat value\n";
    std::cout << "    get <stat>          - Get current value of a stat\n";
    std::cout << "    stats               - Show all current stat values\n";
    std::cout << "    tags                - Show all active tags\n";
    std::cout << "\n";
    std::cout << "  COMBAT EVENTS (Temporal Mechanics)\n";
    std::cout << "    crit                - Trigger a critical hit\n";
    std::cout << "    block               - Trigger a block\n";
    std::cout << "    damage <amount> <type> - Take damage (fire/cold/phys)\n";
    std::cout << "                           e.g., damage 500 fire\n";
    std::cout << "\n";
    std::cout << "  ITEMS & AURAS (Producer Pattern)\n";
    std::cout << "    equip daggers       - Equip Dual Daggers (+DualWielding)\n";
    std::cout << "    equip shield        - Equip Phoenix Shield\n";
    std::cout << "    equip ring          - Equip Mana Ring\n";
    std::cout << "    equip gloves        - Equip Assassin's Gloves\n";
    std::cout << "    unequip <item>      - Unequip an item\n";
    std::cout << "    aura purity         - Toggle Purity of Elements\n";
    std::cout << "    aura onslaught      - Trigger Onslaught (4s)\n";
    std::cout << "\n";
    std::cout << "  SIMULATION\n";
    std::cout << "    auto [on|off]       - Toggle auto-combat simulation\n";
    std::cout << "    speed <value>       - Set simulation speed (0.1-5)\n";
    std::cout << "\n";
    std::cout << "    help / quit\n";
    std::cout << "================================================================\n\n";
}

DamageType ParseDamageType(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "fire") return DamageType::Fire;
    if (lower == "cold") return DamageType::Cold;
    if (lower == "lightning" || lower == "light") return DamageType::Lightning;
    if (lower == "chaos") return DamageType::Chaos;
    return DamageType::Physical;
}

void ProcessCommand(const std::string& input, GraphManager& graph) {
    std::istringstream iss(input);
    std::string command;
    iss >> command;

    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    if (command == "set") {
        std::string statId;
        double value;
        if (iss >> statId >> value) {
            std::transform(statId.begin(), statId.end(), statId.begin(), ::tolower);
            if (graph.SetNodeValue(statId, value)) {
                std::cout << "[OK] Set " << statId << " = " << value << "\n";
            } else {
                std::cout << "[ERROR] Unknown stat: " << statId << "\n";
            }
        }
    }
    else if (command == "get") {
        std::string statId;
        if (iss >> statId) {
            std::transform(statId.begin(), statId.end(), statId.begin(), ::tolower);
            if (auto node = graph.GetNode(statId)) {
                std::cout << "[" << node->GetLabel() << "] = " << std::fixed 
                          << std::setprecision(2) << node->GetValue() << "\n";
            } else {
                std::cout << "[ERROR] Unknown stat: " << statId << "\n";
            }
        }
    }
    else if (command == "crit") {
        CombatEvent::TriggerCrit(graph);
        std::cout << "[COMBAT] Critical hit! (+50% MORE crit for 4s)\n";
        std::cout << "         Watch CritChance spike in the visualizer!\n";
    }
    else if (command == "block") {
        CombatEvent::TriggerBlock(graph);
        std::cout << "[COMBAT] Blocked!\n";
    }
    else if (command == "damage") {
        double amount;
        std::string typeStr;
        if (iss >> amount >> typeStr) {
            DamageType type = ParseDamageType(typeStr);
            CombatEvent::TakeDamage(graph, amount, type);
            std::cout << "[COMBAT] Took " << amount << " " << DamageTypeToString(type) << " damage!\n";
            
            if (type == DamageType::Fire) {
                std::cout << "         Watch BlockChance increase (+" << std::floor(amount/200) << "%)\n";
            }
        } else {
            std::cout << "[ERROR] Usage: damage <amount> <type>\n";
        }
    }
    else if (command == "equip") {
        std::string itemName;
        iss >> itemName;
        std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);
        
        std::shared_ptr<Item> item;
        if (itemName == "daggers") item = CreateDualDaggers();
        else if (itemName == "shield") item = CreateFireShield();
        else if (itemName == "ring") item = CreateManaRing();
        else if (itemName == "gloves") item = CreateCritGloves();
        
        if (item) {
            item->Equip(graph);
            g_equippedItems.push_back(item);
            std::cout << "[EQUIP] " << item->GetName() << " equipped!\n";
            
            // Show granted tags
            for (const auto& tag : item->GetGrantedTags()) {
                std::cout << "        Granted tag: " << tag.GetName() << "\n";
            }
        } else {
            std::cout << "[ERROR] Unknown item: " << itemName << "\n";
            std::cout << "        Available: daggers, shield, ring, gloves\n";
        }
    }
    else if (command == "unequip") {
        std::string itemName;
        iss >> itemName;
        std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);
        
        for (auto it = g_equippedItems.begin(); it != g_equippedItems.end(); ++it) {
            std::string name = (*it)->GetName();
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            if (name.find(itemName) != std::string::npos) {
                (*it)->Unequip(graph);
                std::cout << "[UNEQUIP] " << (*it)->GetName() << " removed!\n";
                g_equippedItems.erase(it);
                break;
            }
        }
    }
    else if (command == "aura") {
        std::string auraName;
        iss >> auraName;
        std::transform(auraName.begin(), auraName.end(), auraName.begin(), ::tolower);
        
        if (auraName == "purity") {
            // Toggle purity
            bool found = false;
            for (auto it = g_activeAuras.begin(); it != g_activeAuras.end(); ++it) {
                if ((*it)->GetId() == "purityOfElements") {
                    (*it)->Deactivate(graph);
                    g_activeAuras.erase(it);
                    std::cout << "[AURA] Purity of Elements deactivated\n";
                    found = true;
                    break;
                }
            }
            if (!found) {
                auto aura = CreatePurityOfElements();
                aura->Activate(graph);
                g_activeAuras.push_back(aura);
                std::cout << "[AURA] Purity of Elements activated!\n";
                std::cout << "       +20% Phys to Lightning conversion now active\n";
            }
        }
        else if (auraName == "onslaught") {
            auto aura = CreateOnslaught(4.0);
            aura->Activate(graph);
            g_activeAuras.push_back(aura);
            std::cout << "[AURA] Onslaught triggered for 4 seconds!\n";
        }
    }
    else if (command == "tags") {
        auto tagNames = graph.GetTags().GetAllTagNames();
        std::cout << "\n[Active Tags] (" << tagNames.size() << ")\n";
        for (const auto& name : tagNames) {
            std::cout << "  - " << name << "\n";
        }
        std::cout << "\n[Recently States]\n";
        std::cout << "  - Crit Recently: " << (graph.CritRecently() ? "YES" : "no") << "\n";
        std::cout << "  - Block Recently: " << (graph.BlockedRecently() ? "YES" : "no") << "\n";
        std::cout << "  - Kill Recently: " << (graph.KilledRecently() ? "YES" : "no") << "\n";
    }
    else if (command == "stats") {
        std::cout << "\n---------------------------------------------\n";
        std::cout << "           CURRENT STAT VALUES\n";
        std::cout << "---------------------------------------------\n";
        for (const auto& id : graph.GetNodeIds()) {
            auto node = graph.GetNode(id);
            std::cout << "  " << std::setw(18) << std::left << node->GetLabel() 
                      << " = " << std::setw(10) << std::right << std::fixed 
                      << std::setprecision(2) << node->GetValue();
            
            if (node->IsHistoryNode()) {
                std::cout << " [HIST]";
            } else if (node->GetType() == StatNode::NodeType::Base) {
                std::cout << " [BASE]";
            } else {
                std::cout << " [CALC]";
            }
            std::cout << "\n";
        }
        std::cout << "---------------------------------------------\n";
    }
    else if (command == "auto") {
        std::string mode;
        if (iss >> mode) {
            std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
            g_autoMode = (mode == "on" || mode == "1" || mode == "true");
        } else {
            g_autoMode = !g_autoMode;
        }
        std::cout << "[Auto-Combat] " << (g_autoMode ? "ON - Simulating combat!" : "OFF") << "\n";
    }
    else if (command == "speed") {
        double speed;
        if (iss >> speed) {
            g_autoSpeed = std::clamp(speed, 0.1, 5.0);
            std::cout << "[Speed] Simulation speed set to " << g_autoSpeed << "x\n";
        }
    }
    else if (command == "help") {
        PrintHelp();
    }
    else if (command == "quit" || command == "exit") {
        g_running = false;
    }
    else if (!command.empty()) {
        std::cout << "[ERROR] Unknown command: " << command << " (type 'help')\n";
    }
}

// ─────────────────────────────────────────────────────────────
// Input Thread
// ─────────────────────────────────────────────────────────────

void InputThread(GraphManager& graph) {
    std::string input;
    while (g_running) {
        std::cout << "> ";
        std::getline(std::cin, input);
        if (!g_running) break;
        ProcessCommand(input, graph);
    }
}

// ─────────────────────────────────────────────────────────────
// Main Entry Point
// ─────────────────────────────────────────────────────────────

int main() {
    std::cout << "\n";
    std::cout << "===============================================================\n";
    std::cout << "                                                               \n";
    std::cout << "       DAG RPG   -   PHASE 2: TEMPORAL MECHANICS               \n";
    std::cout << "                                                               \n";
    std::cout << "   Path of Exile-style 'Recently' modifiers                    \n";
    std::cout << "   Conditional tag-based modifiers                             \n";
    std::cout << "   History nodes with rolling windows                          \n";
    std::cout << "   Multi-target item producers                                 \n";
    std::cout << "                                                               \n";
    std::cout << "===============================================================\n";
    std::cout << "\n";

    // Create the graph manager and build the advanced graph
    GraphManager graph;
    BuildAdvancedGraph(graph);

    // Create and start the WebSocket server
    WebSocketServer server(8090);
    
    // Handle messages from web clients
    server.OnMessage([&graph](const std::string& msg) {
        std::cout << "[WebSocket] Received: " << msg << "\n";
        ProcessCommand(msg, graph);
    });

    if (!server.Start()) {
        std::cerr << "[FATAL] Failed to start WebSocket server!\n";
        return 1;
    }

    PrintHelp();
    std::cout << "\n[INFO] Open 'index.html' in a browser to see the live visualization!\n";
    std::cout << "[INFO] Try these demo commands:\n";
    std::cout << "       1. 'crit' - See CritChance spike with 'Recently' bonus\n";
    std::cout << "       2. 'damage 500 fire' - See BlockChance increase\n";
    std::cout << "       3. 'equip daggers' then 'aura purity' - See Phys->Light stack\n";
    std::cout << "       4. 'auto on' - Watch combat simulation in real-time\n\n";

    // Start input thread
    std::thread inputThread(InputThread, std::ref(graph));

    // Main update loop
    auto lastUpdate = std::chrono::steady_clock::now();
    auto startTime = std::chrono::steady_clock::now();
    auto lastAutoCombat = std::chrono::steady_clock::now();
    
    while (g_running) {
        auto now = std::chrono::steady_clock::now();
        
        // Tick the graph (update history nodes, check expirations)
        graph.Tick();
        
        // Check for expired auras
        for (auto it = g_activeAuras.begin(); it != g_activeAuras.end();) {
            if ((*it)->HasExpired()) {
                std::cout << "[AURA] " << (*it)->GetName() << " expired\n";
                (*it)->Deactivate(graph);
                it = g_activeAuras.erase(it);
            } else {
                ++it;
            }
        }
        
        // Auto-combat simulation
        if (g_autoMode) {
            auto combatElapsed = std::chrono::duration<double>(now - lastAutoCombat).count();
            double interval = 0.8 / g_autoSpeed;  // Attacks per ~0.8 seconds
            
            if (combatElapsed >= interval) {
                // Simulate combat
                double roll = static_cast<double>(rand()) / RAND_MAX;
                
                // 20% chance to crit
                if (roll < 0.2) {
                    CombatEvent::TriggerCrit(graph);
                }
                
                // Take some fire damage occasionally
                if (roll > 0.7) {
                    double dmg = 100 + (rand() % 300);
                    CombatEvent::TakeDamage(graph, dmg, DamageType::Fire);
                }
                
                lastAutoCombat = now;
            }
        }

        // Broadcast updates at ~1Hz (was ~30Hz)
        auto updateElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate);
        if (updateElapsed.count() >= 1000) {
            // Broadcast current state (showing dirty flags)
            std::string json = graph.ToJsonString();
            server.Broadcast(json);
            
            // Small delay then send recalculated state
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            
            std::string cleanJson = graph.RecalculateAndSerialize();
            server.Broadcast(cleanJson);
            
            lastUpdate = now;
        }

        // Reduce busy-wait frequency to save CPU (was 10ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Cleanup
    std::cout << "\n[Shutdown] Stopping server...\n";
    server.Stop();
    
    if (inputThread.joinable()) {
        inputThread.join();
    }

    std::cout << "[Shutdown] Goodbye!\n";
    return 0;
}

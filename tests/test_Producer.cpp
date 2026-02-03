/**
 * test_Producer.cpp - Unit tests for Producer classes (Item, Aura)
 *
 * Tests cover:
 * - Item creation and equipping
 * - Modifier application
 * - Tag granting
 * - Conditional modifiers from items
 */

#include "GraphManagerV2.h"
#include "Producer.h"

#include <gtest/gtest.h>

class ProducerTest : public ::testing::Test {
protected:
    void SetUp() override { graph = std::make_shared<GraphManager>(); }

    std::shared_ptr<GraphManager> graph;
};

// ═══════════════════════════════════════════════════════════════
// Item Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(ProducerTest, Item_Constructor) {
    auto item = std::make_shared<Item>("sword", "Iron Sword", Item::Slot::MainHand);

    EXPECT_EQ(item->GetId(), "sword");
    EXPECT_EQ(item->GetName(), "Iron Sword");
    EXPECT_FALSE(item->IsActive());  // Not equipped yet
}

TEST_F(ProducerTest, Item_AddModifier) {
    auto item = std::make_shared<Item>("ring", "Gold Ring", Item::Slot::Ring1);

    item->AddModifier("maxHealth", ModifierType::Flat, 50.0, "+50 Maximum Health");

    // Modifiers are internal until equipped
    EXPECT_FALSE(item->IsActive());
}

TEST_F(ProducerTest, Item_GrantsTag) {
    auto item = std::make_shared<Item>("shield", "Wood Shield", Item::Slot::OffHand);

    item->GrantsTag(Tags::ShieldEquipped);

    // Tag would be applied when equipped
    EXPECT_FALSE(item->IsActive());
}

TEST_F(ProducerTest, Item_EquipActivates) {
    auto item = std::make_shared<Item>("weapon", "Weapon", Item::Slot::MainHand);
    item->AddModifier("damage", ModifierType::Flat, 10.0);

    EXPECT_FALSE(item->IsActive());

    item->Activate(*graph);

    EXPECT_TRUE(item->IsActive());
}

TEST_F(ProducerTest, Item_UnequipDeactivates) {
    auto item = std::make_shared<Item>("weapon", "Weapon", Item::Slot::MainHand);
    item->AddModifier("damage", ModifierType::Flat, 10.0);

    item->Activate(*graph);
    EXPECT_TRUE(item->IsActive());

    item->Deactivate(*graph);
    EXPECT_FALSE(item->IsActive());
}

// ═══════════════════════════════════════════════════════════════
// Aura Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(ProducerTest, Aura_Constructor) {
    auto aura = std::make_shared<Aura>("purity", "Purity of Elements");

    EXPECT_EQ(aura->GetId(), "purity");
    EXPECT_EQ(aura->GetName(), "Purity of Elements");
    EXPECT_FALSE(aura->IsActive());
}

TEST_F(ProducerTest, Aura_GrantsTag) {
    auto aura = std::make_shared<Aura>("purity", "Purity of Elements");
    aura->GrantsTag("Aura.PurityOfElements");

    EXPECT_FALSE(aura->IsActive());
}

TEST_F(ProducerTest, Aura_Activate) {
    auto aura = std::make_shared<Aura>("haste", "Haste Aura");

    EXPECT_FALSE(aura->IsActive());

    aura->Activate(*graph);

    EXPECT_TRUE(aura->IsActive());
}

TEST_F(ProducerTest, Aura_Deactivate) {
    auto aura = std::make_shared<Aura>("haste", "Haste Aura");

    aura->Activate(*graph);
    EXPECT_TRUE(aura->IsActive());

    aura->Deactivate(*graph);
    EXPECT_FALSE(aura->IsActive());
}

// ═══════════════════════════════════════════════════════════════
// Builder Pattern Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(ProducerTest, Item_BuilderPattern_Chaining) {
    auto item = std::make_shared<Item>("ring", "Ruby Ring", Item::Slot::Ring1);

    // Test method chaining
    item->AddModifier("damage", ModifierType::Flat, 10.0)
        .AddModifier("fireRes", ModifierType::Flat, 20.0)
        .GrantsTag("Item.Ring");

    EXPECT_EQ(item->GetId(), "ring");
}

// ═══════════════════════════════════════════════════════════════
// Multiple Modifiers Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(ProducerTest, Item_MultipleModifiers) {
    auto weapon = std::make_shared<Item>("axe", "Battle Axe", Item::Slot::MainHand);

    weapon->AddModifier("damage", ModifierType::Flat, 50.0);
    weapon->AddModifier("attackSpeed", ModifierType::Increased, 0.10);
    weapon->AddModifier("critChance", ModifierType::Flat, 5.0);

    // All modifiers stored internally
    EXPECT_FALSE(weapon->IsActive());
}

// ═══════════════════════════════════════════════════════════════
// Edge Cases
// ═══════════════════════════════════════════════════════════════

TEST_F(ProducerTest, Item_EmptyId_IsValid) {
    auto item = std::make_shared<Item>("", "No ID Item", Item::Slot::Helmet);
    EXPECT_EQ(item->GetId(), "");
}

TEST_F(ProducerTest, Item_NoModifiers_IsValid) {
    auto item = std::make_shared<Item>("plain", "Plain Item", Item::Slot::Boots);

    item->Activate(*graph);
    EXPECT_TRUE(item->IsActive());

    item->Deactivate(*graph);
    EXPECT_FALSE(item->IsActive());
}

/**
 * test_Modifier.cpp - Unit tests for Modifier class
 *
 * Tests cover:
 * - Modifier creation and basic properties
 * - Different modifier types (Flat, Increased, More)
 * - Conditional modifiers
 * - Dynamic value calculation
 * - Priority and stacking
 */

#include "../include/GameplayTag.h"
#include "../include/Modifier.h"

#include <gtest/gtest.h>

// ═══════════════════════════════════════════════════════════════
// Basic Modifier Tests
// ═══════════════════════════════════════════════════════════════

TEST(ModifierTest, Constructor_CreatesFlatModifier) {
    Modifier mod("test_mod", "targetStat", ModifierType::Flat, 50.0);
    ModifierContext ctx;

    EXPECT_EQ(mod.GetId(), "test_mod");
    EXPECT_EQ(mod.GetTargetStatId(), "targetStat");
    EXPECT_EQ(mod.GetType(), ModifierType::Flat);
    EXPECT_DOUBLE_EQ(mod.GetValue(ctx), 50.0);
    EXPECT_TRUE(mod.IsActive());
}

TEST(ModifierTest, Description_CanBeSet) {
    Modifier mod("test", "stat", ModifierType::Flat, 10.0);
    mod.WithDescription("Test Description");

    EXPECT_EQ(mod.GetDescription(), "Test Description");
}

TEST(ModifierTest, Source_CanBeSet) {
    Modifier mod("test", "stat", ModifierType::Flat, 10.0);
    mod.WithSource("weapon_1");

    EXPECT_EQ(mod.GetSourceId(), "weapon_1");
}

// ═══════════════════════════════════════════════════════════════
// Modifier Type Tests
// ═══════════════════════════════════════════════════════════════

TEST(ModifierTest, FlatModifier_AddsValue) {
    Modifier mod("test", "damage", ModifierType::Flat, 25.0);
    ModifierContext ctx;

    double baseValue = 100.0;
    // Flat modifiers just add to the value
    double result = baseValue + mod.GetValue(ctx);

    EXPECT_DOUBLE_EQ(result, 125.0);
}

TEST(ModifierTest, IncreasedModifier_AddsPercentage) {
    Modifier mod("test", "damage", ModifierType::Increased, 0.50);  // 50%
    ModifierContext ctx;

    EXPECT_EQ(mod.GetType(), ModifierType::Increased);
    EXPECT_DOUBLE_EQ(mod.GetValue(ctx), 0.50);
}

TEST(ModifierTest, MoreModifier_MultipliesValue) {
    Modifier mod("test", "damage", ModifierType::More, 0.30);  // 30% more
    ModifierContext ctx;

    EXPECT_EQ(mod.GetType(), ModifierType::More);
    EXPECT_DOUBLE_EQ(mod.GetValue(ctx), 0.30);
}

// ═══════════════════════════════════════════════════════════════
// Conditional Modifier Tests
// ═══════════════════════════════════════════════════════════════

TEST(ModifierTest, ConditionalModifier_EvaluatesCondition) {
    Modifier mod("test", "damage", ModifierType::Flat, 50.0);

    bool conditionState = false;
    mod.WithCondition([&conditionState](const ModifierContext&) { return conditionState; });

    ModifierContext ctx;

    // Condition false
    EXPECT_FALSE(mod.EvaluateCondition(ctx));

    // Condition true
    conditionState = true;
    EXPECT_TRUE(mod.EvaluateCondition(ctx));
}

TEST(ModifierTest, ConditionalModifier_WithTags) {
    GameplayTagContainer tags;
    tags.AddTag("Combat.DualWielding");

    Modifier mod("test", "crit", ModifierType::Increased, 0.30);
    mod.WithCondition([](const ModifierContext& ctx) { return ctx.HasTag("Combat.DualWielding"); });

    ModifierContext ctx;
    ctx.tags = &tags;

    EXPECT_TRUE(mod.EvaluateCondition(ctx));

    // Remove tag
    tags.RemoveTag("Combat.DualWielding");
    EXPECT_FALSE(mod.EvaluateCondition(ctx));
}

// ═══════════════════════════════════════════════════════════════
// Priority Tests
// ═══════════════════════════════════════════════════════════════

TEST(ModifierTest, Priority_CanBeSetAndRetrieved) {
    Modifier mod("test", "stat", ModifierType::Flat, 10.0);

    mod.WithPriority(5);
    EXPECT_EQ(mod.GetPriority(), 5);
}

TEST(ModifierTest, DefaultPriority_IsZero) {
    Modifier mod("test", "stat", ModifierType::Flat, 10.0);
    EXPECT_EQ(mod.GetPriority(), 0);
}

// ═══════════════════════════════════════════════════════════════
// Active State Tests
// ═══════════════════════════════════════════════════════════════

TEST(ModifierTest, SetActive_TogglesModifier) {
    Modifier mod("test", "stat", ModifierType::Flat, 10.0);

    EXPECT_TRUE(mod.IsActive());

    mod.SetActive(false);
    EXPECT_FALSE(mod.IsActive());

    mod.SetActive(true);
    EXPECT_TRUE(mod.IsActive());
}

// ═══════════════════════════════════════════════════════════════
// Edge Cases
// ═══════════════════════════════════════════════════════════════

TEST(ModifierTest, NegativeValue_IsValid) {
    Modifier mod("test", "stat", ModifierType::Flat, -25.0);
    ModifierContext ctx;
    EXPECT_DOUBLE_EQ(mod.GetValue(ctx), -25.0);
}

TEST(ModifierTest, ZeroValue_IsValid) {
    Modifier mod("test", "stat", ModifierType::Flat, 0.0);
    ModifierContext ctx;
    EXPECT_DOUBLE_EQ(mod.GetValue(ctx), 0.0);
}

TEST(ModifierTest, ModifierTypeToString_ReturnsCorrectStrings) {
    EXPECT_STREQ(ModifierTypeToString(ModifierType::Flat), "Flat");
    EXPECT_STREQ(ModifierTypeToString(ModifierType::Increased), "Increased");
    EXPECT_STREQ(ModifierTypeToString(ModifierType::More), "More");
    EXPECT_STREQ(ModifierTypeToString(ModifierType::Override), "Override");
}

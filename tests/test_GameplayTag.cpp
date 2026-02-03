/**
 * test_GameplayTag.cpp - Unit tests for GameplayTag system
 *
 * Tests cover:
 * - Tag creation and comparison
 * - Tag container operations
 * - Hierarchical tag matching
 */

#include "GameplayTag.h"

#include <gtest/gtest.h>

class GameplayTagTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test fixtures
    }
};

// ═══════════════════════════════════════════════════════════════
// Basic Tag Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(GameplayTagTest, Constructor_CreatesTag) {
    GameplayTag tag("Combat.Melee.Sword");

    EXPECT_EQ(tag.ToString(), "Combat.Melee.Sword");
}

TEST_F(GameplayTagTest, EmptyTag_IsValid) {
    GameplayTag tag("");
    EXPECT_EQ(tag.ToString(), "");
}

TEST_F(GameplayTagTest, Equality_ComparesCorrectly) {
    GameplayTag tag1("State.Burning");
    GameplayTag tag2("State.Burning");
    GameplayTag tag3("State.Frozen");

    EXPECT_TRUE(tag1 == tag2);
    EXPECT_FALSE(tag1 == tag3);
}

TEST_F(GameplayTagTest, Inequality_ComparesCorrectly) {
    GameplayTag tag1("Aura.Haste");
    GameplayTag tag2("Aura.Precision");

    EXPECT_TRUE(tag1 != tag2);
}

// ═══════════════════════════════════════════════════════════════
// Tag Container Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(GameplayTagTest, Container_StartsEmpty) {
    GameplayTagContainer container;

    EXPECT_TRUE(container.IsEmpty());
    EXPECT_EQ(container.GetTagCount(), 0);
}

TEST_F(GameplayTagTest, AddTag_String) {
    GameplayTagContainer container;

    container.AddTag("State.Blocking");

    EXPECT_FALSE(container.IsEmpty());
    EXPECT_EQ(container.GetTagCount(), 1);
    EXPECT_TRUE(container.HasTag("State.Blocking"));
}

TEST_F(GameplayTagTest, AddTag_GameplayTag) {
    GameplayTagContainer container;
    GameplayTag tag("Combat.Active");

    container.AddTag(tag);

    EXPECT_TRUE(container.HasTag(tag));
}

TEST_F(GameplayTagTest, AddTag_Duplicate_OnlyAddsOnce) {
    GameplayTagContainer container;

    container.AddTag("Duplicate");
    container.AddTag("Duplicate");

    EXPECT_EQ(container.GetTagCount(), 1);
}

TEST_F(GameplayTagTest, RemoveTag_RemovesExistingTag) {
    GameplayTagContainer container;
    container.AddTag("ToRemove");

    EXPECT_TRUE(container.HasTag("ToRemove"));

    container.RemoveTag("ToRemove");

    EXPECT_FALSE(container.HasTag("ToRemove"));
}

TEST_F(GameplayTagTest, RemoveTag_NonExistent_DoesNothing) {
    GameplayTagContainer container;
    container.AddTag("Existing");

    container.RemoveTag("NonExistent");

    EXPECT_EQ(container.GetTagCount(), 1);
    EXPECT_TRUE(container.HasTag("Existing"));
}

TEST_F(GameplayTagTest, Clear_RemovesAllTags) {
    GameplayTagContainer container;
    container.AddTag("Tag1");
    container.AddTag("Tag2");
    container.AddTag("Tag3");

    container.Clear();

    EXPECT_TRUE(container.IsEmpty());
    EXPECT_EQ(container.GetTagCount(), 0);
}

// ═══════════════════════════════════════════════════════════════
// Query Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(GameplayTagTest, HasAnyTag_ReturnsTrueWhenMatch) {
    GameplayTagContainer container;
    container.AddTag("Tag1");
    container.AddTag("Tag2");

    std::vector<GameplayTag> query = {GameplayTag("Tag2"), GameplayTag("Tag3")};

    EXPECT_TRUE(container.HasAnyTag(query));
}

TEST_F(GameplayTagTest, HasAnyTag_ReturnsFalseWhenNoMatch) {
    GameplayTagContainer container;
    container.AddTag("Tag1");

    std::vector<GameplayTag> query = {GameplayTag("Tag2"), GameplayTag("Tag3")};

    EXPECT_FALSE(container.HasAnyTag(query));
}

TEST_F(GameplayTagTest, HasAllTags_ReturnsTrueWhenAllMatch) {
    GameplayTagContainer container;
    container.AddTag("Tag1");
    container.AddTag("Tag2");
    container.AddTag("Tag3");

    std::vector<GameplayTag> query = {GameplayTag("Tag1"), GameplayTag("Tag2")};

    EXPECT_TRUE(container.HasAllTags(query));
}

TEST_F(GameplayTagTest, HasAllTags_ReturnsFalseWhenMissing) {
    GameplayTagContainer container;
    container.AddTag("Tag1");

    std::vector<GameplayTag> query = {GameplayTag("Tag1"), GameplayTag("Tag2")};

    EXPECT_FALSE(container.HasAllTags(query));
}

// ═══════════════════════════════════════════════════════════════
// Hierarchical Tests (if implemented)
// ═══════════════════════════════════════════════════════════════

TEST_F(GameplayTagTest, HierarchicalMatch_ExactMatch) {
    GameplayTagContainer container;
    container.AddTag("Combat.Melee.Sword");

    EXPECT_TRUE(container.HasTag("Combat.Melee.Sword"));
}

// Add more tests if hierarchical matching is implemented
// e.g., "Combat.Melee" matching "Combat.Melee.Sword"

/**
 * test_StatNode.cpp - Unit tests for StatNode class
 *
 * Tests cover:
 * - Basic node creation and value retrieval
 * - Dirty flag propagation
 * - Parent-child dependencies
 * - Conditional dependencies
 * - Custom calculation functions
 */

#include "StatNode.h"

#include <gtest/gtest.h>

#include <memory>

class StatNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create fresh nodes for each test
    }

    void TearDown() override {
        // Cleanup if needed
    }
};

// ═══════════════════════════════════════════════════════════════
// Basic Functionality Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(StatNodeTest, Constructor_CreatesBaseNode) {
    auto node = std::make_shared<StatNode>("test", "Test Node", 100.0);

    EXPECT_EQ(node->GetId(), "test");
    EXPECT_EQ(node->GetLabel(), "Test Node");
    EXPECT_DOUBLE_EQ(node->GetValue(), 100.0);
    EXPECT_EQ(node->GetType(), StatNode::NodeType::Base);
    EXPECT_FALSE(node->IsDirty());
}

TEST_F(StatNodeTest, SetBaseValue_UpdatesValue) {
    auto node = std::make_shared<StatNode>("test", "Test", 100.0);

    node->SetBaseValue(200.0);

    EXPECT_DOUBLE_EQ(node->GetValue(), 200.0);
    EXPECT_FALSE(node->IsDirty());
}

TEST_F(StatNodeTest, SetBaseValue_MarksDependentsAsDirty) {
    auto base = std::make_shared<StatNode>("base", "Base", 10.0);
    auto derived =
        std::make_shared<StatNode>("derived", "Derived", 0.0, StatNode::NodeType::Derived);

    derived->AddParent(base);
    derived->SetCalculationFunc([](const std::vector<double>& parents) {
        return parents[0] * 2.0;  // Double the base value
    });

    // Initial calculation
    EXPECT_DOUBLE_EQ(derived->GetValue(), 20.0);
    EXPECT_FALSE(derived->IsDirty());

    // Change base value
    base->SetBaseValue(15.0);

    EXPECT_TRUE(derived->IsDirty());
    EXPECT_DOUBLE_EQ(derived->GetValue(), 30.0);  // Recalculated
    EXPECT_FALSE(derived->IsDirty());
}

// ═══════════════════════════════════════════════════════════════
// Dependency Chain Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(StatNodeTest, ThreeLevelDependency_PropagatesDirtyFlag) {
    // Create: A -> B -> C
    auto nodeA = std::make_shared<StatNode>("A", "A", 10.0);
    auto nodeB = std::make_shared<StatNode>("B", "B", 0.0, StatNode::NodeType::Derived);
    auto nodeC = std::make_shared<StatNode>("C", "C", 0.0, StatNode::NodeType::Derived);

    nodeB->AddParent(nodeA);
    nodeB->SetCalculationFunc([](const std::vector<double>& p) { return p[0] + 5.0; });

    nodeC->AddParent(nodeB);
    nodeC->SetCalculationFunc([](const std::vector<double>& p) { return p[0] * 2.0; });

    // Initial state
    EXPECT_DOUBLE_EQ(nodeB->GetValue(), 15.0);  // 10 + 5
    EXPECT_DOUBLE_EQ(nodeC->GetValue(), 30.0);  // 15 * 2

    // Change A
    nodeA->SetBaseValue(20.0);

    EXPECT_TRUE(nodeB->IsDirty());
    EXPECT_TRUE(nodeC->IsDirty());

    EXPECT_DOUBLE_EQ(nodeB->GetValue(), 25.0);  // 20 + 5
    EXPECT_DOUBLE_EQ(nodeC->GetValue(), 50.0);  // 25 * 2
}

TEST_F(StatNodeTest, MultipleParents_CombinesValues) {
    auto str = std::make_shared<StatNode>("str", "Strength", 10.0);
    auto dex = std::make_shared<StatNode>("dex", "Dexterity", 15.0);
    auto attack = std::make_shared<StatNode>("attack", "Attack", 0.0, StatNode::NodeType::Derived);

    attack->AddParent(str);
    attack->AddParent(dex);
    attack->SetCalculationFunc([](const std::vector<double>& p) {
        return p[0] * 2.0 + p[1] * 1.5;  // STR*2 + DEX*1.5
    });

    EXPECT_DOUBLE_EQ(attack->GetValue(), 42.5);  // 10*2 + 15*1.5 = 20 + 22.5
}

// ═══════════════════════════════════════════════════════════════
// Conditional Dependency Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(StatNodeTest, ConditionalParent_OnlyAppliesWhenConditionMet) {
    auto base = std::make_shared<StatNode>("base", "Base", 100.0, StatNode::NodeType::Base);
    auto bonus = std::make_shared<StatNode>("bonus", "Bonus", 50.0, StatNode::NodeType::Base);
    auto derived =
        std::make_shared<StatNode>("derived", "Derived", 0.0, StatNode::NodeType::Derived);

    // Add base as normal parent
    derived->AddParent(base);

    // Add bonus as conditional parent
    bool conditionMet = false;
    derived->AddConditionalParent(
        bonus, [&conditionMet]() { return conditionMet; }, "Test Condition");

    // Condition not met - only base value
    EXPECT_DOUBLE_EQ(derived->GetValue(), 100.0);

    // Meet condition - base + bonus
    conditionMet = true;
    derived->MarkDirty();
    EXPECT_DOUBLE_EQ(derived->GetValue(), 150.0);  // 100 + 50
}

// ═══════════════════════════════════════════════════════════════
// Edge Cases and Error Handling
// ═══════════════════════════════════════════════════════════════

TEST_F(StatNodeTest, NegativeValues_AreAllowed) {
    auto node = std::make_shared<StatNode>("test", "Test", -50.0);
    EXPECT_DOUBLE_EQ(node->GetValue(), -50.0);
}

TEST_F(StatNodeTest, ZeroBaseValue_IsValid) {
    auto node = std::make_shared<StatNode>("test", "Test", 0.0);
    EXPECT_DOUBLE_EQ(node->GetValue(), 0.0);
}

TEST_F(StatNodeTest, GetValue_WithoutCalculationFunc_ReturnsBaseValue) {
    // Derived node without parents or calc func returns 0.0 (sum of empty parent list)
    auto derived = std::make_shared<StatNode>("test", "Test", 42.0, StatNode::NodeType::Derived);
    EXPECT_DOUBLE_EQ(derived->GetValue(), 0.0);  // No parents = 0.0

    // But if we add a parent, it sums the parent values
    auto parent = std::make_shared<StatNode>("parent", "Parent", 100.0);
    derived->AddParent(parent);
    EXPECT_DOUBLE_EQ(derived->GetValue(), 100.0);
}

// ═══════════════════════════════════════════════════════════════
// Metadata Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(StatNodeTest, Category_CanBeSetAndRetrieved) {
    auto node = std::make_shared<StatNode>("test", "Test", 100.0);

    node->SetCategory("offense");
    EXPECT_EQ(node->GetCategory(), "offense");
}

TEST_F(StatNodeTest, HistoryNode_TypeIsCorrect) {
    auto node = std::make_shared<StatNode>("test", "Test", 0.0);

    EXPECT_FALSE(node->IsHistoryNode());

    node->SetIsHistoryNode(true);
    EXPECT_TRUE(node->IsHistoryNode());
    EXPECT_EQ(node->GetType(), StatNode::NodeType::History);
}

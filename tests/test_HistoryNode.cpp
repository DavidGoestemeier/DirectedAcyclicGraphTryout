/**
 * test_HistoryNode.cpp - Unit tests for HistoryNode functionality
 *
 * Tests cover:
 * - History node creation
 * - Event recording
 * - Time-based decay
 * - Rolling window calculations
 */

#include "HistoryNode.h"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

class HistoryNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test history node
    }
};

// ═══════════════════════════════════════════════════════════════
// Basic History Node Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(HistoryNodeTest, Constructor_CreatesEmptyHistory) {
    HistoryNode history("test", 5.0);  // 5 second window

    EXPECT_EQ(history.GetId(), "test");
    EXPECT_EQ(history.GetEventCount(), 0);
}

TEST_F(HistoryNodeTest, RecordEvent_AddsEvent) {
    HistoryNode history("damage", 10.0);

    history.RecordEvent(100.0, 0.0);

    EXPECT_EQ(history.GetEventCount(), 1);
}

TEST_F(HistoryNodeTest, RecordEvent_MultipleEvents) {
    HistoryNode history("crits", 10.0);

    history.RecordEvent(1.0, 0.0);
    history.RecordEvent(1.0, 1.0);
    history.RecordEvent(1.0, 2.0);

    EXPECT_EQ(history->GetEventCount(), 3);
}

// ═══════════════════════════════════════════════════════════════
// Time Window Tests
// ═══════════════════════════════════════════════════════════════

TEST_F(HistoryNodeTest, GetSum_WithinWindow) {
    HistoryNode history("damage", 5.0);  // 5 second window

    history.RecordEvent(10.0, 0.0);
    history.RecordEvent(20.0, 1.0);
    history.RecordEvent(30.0, 2.0);

    double sum = history.GetSum(2.5);  // Query at 2.5 seconds

    EXPECT_DOUBLE_EQ(sum, 60.0);  // All events within window
}

TEST_F(HistoryNodeTest, GetSum_ExpiredEvents) {
    HistoryNode history("damage", 2.0);  // 2 second window

    history.RecordEvent(10.0, 0.0);
    history.RecordEvent(20.0, 1.0);
    history.RecordEvent(30.0, 3.0);  // This is still valid at time 3

    double sum = history.GetSum(3.5);  // Query at 3.5 seconds

    // Only events from 1.5-3.5 seconds are valid
    EXPECT_DOUBLE_EQ(sum, 50.0);  // 20 + 30
}

TEST_F(HistoryNodeTest, GetCount_CountsEventsInWindow) {
    HistoryNode history("hits", 5.0);

    history.RecordEvent(1.0, 0.0);
    history.RecordEvent(1.0, 1.0);
    history.RecordEvent(1.0, 6.0);  // Outside window when queried at 7

    int count = history.GetCount(7.0);

    EXPECT_EQ(count, 1);  // Only event at 6.0 is within window
}

// ═══════════════════════════════════════════════════════════════
// Edge Cases
// ═══════════════════════════════════════════════════════════════

TEST_F(HistoryNodeTest, EmptyHistory_ReturnsZero) {
    HistoryNode history("empty", 5.0);

    EXPECT_DOUBLE_EQ(history.GetSum(10.0), 0.0);
    EXPECT_EQ(history.GetCount(10.0), 0);
}

TEST_F(HistoryNodeTest, AllEventsExpired_ReturnsZero) {
    HistoryNode history("old", 1.0);  // 1 second window

    history.RecordEvent(100.0, 0.0);
    history.RecordEvent(200.0, 0.5);

    // Query far in the future
    EXPECT_DOUBLE_EQ(history.GetSum(100.0), 0.0);
}

TEST_F(HistoryNodeTest, NegativeValues_AreAllowed) {
    HistoryNode history("test", 5.0);

    history.RecordEvent(-10.0, 0.0);
    history.RecordEvent(20.0, 1.0);

    EXPECT_DOUBLE_EQ(history.GetSum(2.0), 10.0);  // -10 + 20
}

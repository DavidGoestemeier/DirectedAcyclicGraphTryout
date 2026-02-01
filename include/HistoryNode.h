/**
 * HistoryNode.h - Temporal Dependency Node with Ring Buffer
 * 
 * Specialized node for "Recently" mechanics in Path of Exile style:
 * - "Damage taken recently"
 * - "Enemies killed recently"
 * - "Crits dealt recently"
 * 
 * Uses a ring buffer to track timestamped events with automatic decay.
 */

#pragma once

#include "StatNode.h"
#include <vector>
#include <chrono>
#include <algorithm>
#include <deque>
#include <mutex>
#include <cmath>

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = std::chrono::duration<double>;

// ═══════════════════════════════════════════════════════════════
// TimestampedEvent - A single event with value and timestamp
// ═══════════════════════════════════════════════════════════════

struct TimestampedEvent {
    double value;
    TimePoint timestamp;
    std::string eventType;  // Optional categorization (e.g., "Fire", "Cold")

    TimestampedEvent(double val, const std::string& type = "")
        : value(val)
        , timestamp(Clock::now())
        , eventType(type)
    {}

    TimestampedEvent(double val, TimePoint time, const std::string& type = "")
        : value(val)
        , timestamp(time)
        , eventType(type)
    {}

    // Get age in seconds
    double GetAge() const {
        return Duration(Clock::now() - timestamp).count();
    }

    bool IsWithinWindow(double windowSeconds) const {
        return GetAge() <= windowSeconds;
    }
};

// ═══════════════════════════════════════════════════════════════
// HistoryStatNode - Node with temporal event tracking
// ═══════════════════════════════════════════════════════════════

class HistoryStatNode : public StatNode {
public:
    static constexpr double DEFAULT_WINDOW = 4.0;  // 4 seconds = "Recently"
    static constexpr size_t MAX_EVENTS = 1000;     // Prevent unbounded growth

private:
    std::deque<TimestampedEvent> m_events;
    mutable std::mutex m_eventsMutex;
    double m_defaultWindow;
    TimePoint m_lastCleanup;

public:
    HistoryStatNode(const std::string& id, 
                    const std::string& label,
                    double windowSeconds = DEFAULT_WINDOW)
        : StatNode(id, label, 0.0, NodeType::Derived)
        , m_defaultWindow(windowSeconds)
        , m_lastCleanup(Clock::now())
    {
        // History nodes calculate their value from the event buffer
        SetCalculationFunc([this](const std::vector<double>&) {
            return SumRecent(m_defaultWindow);
        });
    }

    // ─────────────────────────────────────────────────────────────
    // Event Recording
    // ─────────────────────────────────────────────────────────────

    /**
     * Record a new event (e.g., damage taken, crit dealt)
     * This automatically marks dependents as dirty
     */
    void RecordEvent(double value, const std::string& eventType = "") {
        {
            std::lock_guard<std::mutex> lock(m_eventsMutex);
            m_events.emplace_back(value, eventType);
            
            // Prevent unbounded growth
            while (m_events.size() > MAX_EVENTS) {
                m_events.pop_front();
            }
        }
        
        // Mark this node and all dependents as dirty
        MarkDirty();
    }

    /**
     * Record event with specific timestamp (for testing/replay)
     */
    void RecordEventAt(double value, TimePoint time, const std::string& eventType = "") {
        {
            std::lock_guard<std::mutex> lock(m_eventsMutex);
            m_events.emplace_back(value, time, eventType);
        }
        MarkDirty();
    }

    // ─────────────────────────────────────────────────────────────
    // Query Methods
    // ─────────────────────────────────────────────────────────────

    /**
     * Sum all events within the time window
     */
    double SumRecent(double windowSeconds) const {
        std::lock_guard<std::mutex> lock(m_eventsMutex);
        double sum = 0.0;
        for (const auto& event : m_events) {
            if (event.IsWithinWindow(windowSeconds)) {
                sum += event.value;
            }
        }
        return sum;
    }

    /**
     * Sum events of a specific type within the window
     */
    double SumRecentByType(double windowSeconds, const std::string& eventType) const {
        std::lock_guard<std::mutex> lock(m_eventsMutex);
        double sum = 0.0;
        for (const auto& event : m_events) {
            if (event.eventType == eventType && event.IsWithinWindow(windowSeconds)) {
                sum += event.value;
            }
        }
        return sum;
    }

    /**
     * Count events within the time window
     */
    size_t CountRecent(double windowSeconds) const {
        std::lock_guard<std::mutex> lock(m_eventsMutex);
        size_t count = 0;
        for (const auto& event : m_events) {
            if (event.IsWithinWindow(windowSeconds)) {
                count++;
            }
        }
        return count;
    }

    /**
     * Check if any event occurred within the window
     */
    bool HasRecentEvent(double windowSeconds) const {
        return CountRecent(windowSeconds) > 0;
    }

    /**
     * Get the most recent event timestamp
     */
    std::optional<TimePoint> GetLastEventTime() const {
        std::lock_guard<std::mutex> lock(m_eventsMutex);
        if (m_events.empty()) return std::nullopt;
        return m_events.back().timestamp;
    }

    /**
     * Get seconds since last event
     */
    double GetSecondsSinceLastEvent() const {
        auto lastTime = GetLastEventTime();
        if (!lastTime) return std::numeric_limits<double>::infinity();
        return Duration(Clock::now() - *lastTime).count();
    }

    /**
     * Get decaying value (exponential decay from last event)
     */
    double GetDecayingValue(double halfLife = 2.0) const {
        std::lock_guard<std::mutex> lock(m_eventsMutex);
        if (m_events.empty()) return 0.0;
        
        const auto& lastEvent = m_events.back();
        double age = lastEvent.GetAge();
        double decay = std::exp(-0.693 * age / halfLife);  // ln(2) ≈ 0.693
        return lastEvent.value * decay;
    }

    // ─────────────────────────────────────────────────────────────
    // Maintenance
    // ─────────────────────────────────────────────────────────────

    /**
     * Remove events older than the window (call periodically)
     */
    void CleanupOldEvents(double maxAge) {
        std::lock_guard<std::mutex> lock(m_eventsMutex);
        auto now = Clock::now();
        
        while (!m_events.empty()) {
            if (Duration(now - m_events.front().timestamp).count() > maxAge) {
                m_events.pop_front();
            } else {
                break;  // Events are chronologically ordered
            }
        }
        m_lastCleanup = now;
    }

    /**
     * Tick function - call each frame to update decay and cleanup
     * Returns true if the value changed (needs broadcast)
     */
    bool Tick(double maxAge = 10.0) {
        double oldValue = GetCachedValue();
        
        // Cleanup old events periodically
        if (Duration(Clock::now() - m_lastCleanup).count() > 1.0) {
            CleanupOldEvents(maxAge);
        }
        
        // Force recalculation
        MarkDirty();
        double newValue = GetValue();
        
        // Return true if value changed significantly
        return std::abs(newValue - oldValue) > 0.01;
    }

    /**
     * Get all events for debugging/visualization
     */
    std::vector<std::pair<double, double>> GetEventHistory() const {
        std::lock_guard<std::mutex> lock(m_eventsMutex);
        std::vector<std::pair<double, double>> history;
        history.reserve(m_events.size());
        for (const auto& event : m_events) {
            history.emplace_back(event.value, event.GetAge());
        }
        return history;
    }

    double GetDefaultWindow() const { return m_defaultWindow; }
    size_t GetEventCount() const { 
        std::lock_guard<std::mutex> lock(m_eventsMutex);
        return m_events.size(); 
    }
};

// ═══════════════════════════════════════════════════════════════
// RecentlyTracker - Simple boolean "did X happen recently?"
// ═══════════════════════════════════════════════════════════════

class RecentlyTracker {
private:
    TimePoint m_lastTrigger;
    double m_window;
    bool m_hasEverTriggered = false;

public:
    explicit RecentlyTracker(double windowSeconds = 4.0) 
        : m_lastTrigger(Clock::now())
        , m_window(windowSeconds)
    {}

    void Trigger() {
        m_lastTrigger = Clock::now();
        m_hasEverTriggered = true;
    }

    void TriggerAt(TimePoint time) {
        m_lastTrigger = time;
        m_hasEverTriggered = true;
    }

    bool IsRecent() const {
        if (!m_hasEverTriggered) return false;
        return Duration(Clock::now() - m_lastTrigger).count() <= m_window;
    }

    double GetSecondsSinceTrigger() const {
        if (!m_hasEverTriggered) return std::numeric_limits<double>::infinity();
        return Duration(Clock::now() - m_lastTrigger).count();
    }

    double GetRemainingTime() const {
        if (!m_hasEverTriggered) return 0.0;
        double elapsed = Duration(Clock::now() - m_lastTrigger).count();
        return (elapsed < m_window) ? (m_window - elapsed) : 0.0;
    }

    void SetWindow(double seconds) { m_window = seconds; }
    double GetWindow() const { return m_window; }
};

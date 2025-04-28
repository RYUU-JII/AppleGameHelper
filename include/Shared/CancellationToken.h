#pragma once
#include <atomic>

class CancellationToken {
public:
    CancellationToken() : m_cancelled(false) {}

    void cancel() {
        m_cancelled.store(true, std::memory_order_relaxed);
    }

    bool isCancelled() const {
        return m_cancelled.load(std::memory_order_relaxed);
    }

    void reset() {
        m_cancelled.store(false, std::memory_order_relaxed);
    }

private:
    std::atomic<bool> m_cancelled;
};

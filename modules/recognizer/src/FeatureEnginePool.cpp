#include "FeatureEngine.hpp"
#include "FeatureEnginePool.hpp"

#include <array>
#include <condition_variable>
#include <mutex>
#include <queue>

#include <iostream>
namespace face {
namespace recognizer {
class FeatureEnginePool::Implement {
  public:
    std::queue<std::unique_ptr<FeatureEngine>> m_engines;
    std::mutex m_mutex;
    std::condition_variable m_condition;

    int m_size;
    int m_available;
    std::string m_model_dir;
    Implement(int size, const std::string &model_dir)
        : m_size(size)
        , m_available(0)
        , m_model_dir(model_dir) {
    }

    ~Implement() {
    }

    std::unique_ptr<FeatureEngine> Acquire() {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_engines.empty()) {
            auto engine = std::move(m_engines.front());
            m_engines.pop();
            return engine;
        }

        if (m_available < m_size) {
            m_available++;
            return std::make_unique<FeatureEngine>(m_model_dir);
        }

        while (m_engines.empty()) {
            m_condition.wait(lock);
        }

        auto engine = std::move(m_engines.front());
        m_engines.pop();
        return engine;
    }

    void Release(std::unique_ptr<FeatureEngine> engine) {
        if (engine == nullptr) {
            return;
        }
        std::unique_lock<std::mutex> lock(m_mutex);
        m_engines.push(std::move(engine));
        m_condition.notify_one();
    }
};

FeatureEnginePool::FeatureEnginePool(int size, const std::string &model_dir)
    : impl(std::make_unique<Implement>(size, model_dir)) {
}

FeatureEnginePool::~FeatureEnginePool() {
}

std::unique_ptr<FeatureEngine> FeatureEnginePool::Acquire() {
    return impl->Acquire();
}

void FeatureEnginePool::Release(std::unique_ptr<FeatureEngine> engine) {
    impl->Release(std::move(engine));
}

FeatureEnginePool::LockGuard::LockGuard(std::shared_ptr<FeatureEnginePool> &p)
    : pool(p) {
    engine = pool->Acquire();
}

FeatureEnginePool::LockGuard::~LockGuard() {
    pool->Release(std::move(engine));
}

FeatureEngine *FeatureEnginePool::LockGuard::operator->() {
    return engine.get();
}

FeatureEnginePool::LockGuard::operator bool() const noexcept { return engine != nullptr; }
}  // namespace recognizer
}  // namespace face

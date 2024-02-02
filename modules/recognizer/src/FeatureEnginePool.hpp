#ifndef __FeatureEnginePool_hpp__
#define __FeatureEnginePool_hpp__

#include <memory>

namespace face {
namespace recognizer {
class FeatureEngine;
class FeatureEnginePool {
  private:
    class Implement;
    std::unique_ptr<Implement> impl;

  public:
    class LockGuard {
      private:
        std::unique_ptr<FeatureEngine> engine;
        std::shared_ptr<FeatureEnginePool> pool;

      public:
        explicit LockGuard(std::shared_ptr<FeatureEnginePool> &p);
        ~LockGuard();
        FeatureEngine *operator->();
        explicit operator bool() const noexcept;
    };

    FeatureEnginePool(int size, const std::string &model_dir);
    ~FeatureEnginePool();

    std::unique_ptr<FeatureEngine> Acquire();
    void Release(std::unique_ptr<FeatureEngine> engine);
};
}  // namespace recognizer
};  // namespace face
#endif

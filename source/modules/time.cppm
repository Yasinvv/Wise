module;
#include <chrono>
export module time;

namespace WisE {

export struct UniformTime {

  inline static const auto startTime =
      std::chrono::high_resolution_clock::now();

  inline static auto past = std::chrono::high_resolution_clock::now();

  float getTime() const {
    auto currentTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float>(currentTime - startTime).count();
  }
  float getDeltaTime() {
    auto Current = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float>(Current - past).count();
    past = Current;
    return dt;
  }
};
} // namespace WisE

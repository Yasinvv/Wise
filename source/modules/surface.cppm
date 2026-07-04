module;

#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

export module surface;
import context;
import window;

namespace WisE {

export class Surface {
private:
public:
  void createSurface(Window& window, VK_CTX& ctx) {
    VkSurfaceKHR _surface;
    if (!SDL_Vulkan_CreateSurface(window.window, *ctx.instance, nullptr,
                                  &_surface)) {
      throw std::runtime_error("failed to create window surface!");
    }
    ctx.surface = vk::raii::SurfaceKHR(ctx.instance, _surface);
  }
};
} // namespace WisE

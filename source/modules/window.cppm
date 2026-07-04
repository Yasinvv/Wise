module;

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>

export module window;
import context;

namespace WisE {

export class Window {

private:
public:
  uint32_t WIDTH{1920U};
  uint32_t HEIGHT{1080U};
  SDL_Window* window = nullptr;

  void initWindow() {
    SDL_SetAppMetadata("preview", "0.0.1", "Ys");

    // SDL_Initialization
    if (!SDL_Init(SDL_INIT_VIDEO)) {

      SDL_Log("Could'nt Initializing SDL: %s", SDL_GetError());
    }

    // Window
    window = SDL_CreateWindow("preview", WIDTH, HEIGHT,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (!window) {
      SDL_Log("Error Creating Window: %s", SDL_GetError());
    }
    SDL_Log("SDL Initialized");
    SDL_SetWindowRelativeMouseMode(window, true);
  }
};
} // namespace WisE

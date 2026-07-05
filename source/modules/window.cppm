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
  struct WindowSettings {
    uint32_t WIDTH{1920U};
    uint32_t HEIGHT{1080U};
    const char* appname{};
    const char* appversion{};
    const char* appID{};
  };
  SDL_Window* window = nullptr;

  void initWindow(WindowSettings& ws) {
    SDL_SetAppMetadata(ws.appname, ws.appversion, ws.appID);

    if (!SDL_Init(SDL_INIT_VIDEO)) {

      SDL_Log("Could'nt Initializing SDL: %s", SDL_GetError());
    }

    window = SDL_CreateWindow(ws.appname, ws.WIDTH, ws.HEIGHT,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (!window) {
      SDL_Log("Error Creating Window: %s", SDL_GetError());
    }
    SDL_Log("SDL Initialized : %p", (void*)window);
    SDL_SetWindowRelativeMouseMode(window, true);
  }
};
} // namespace WisE

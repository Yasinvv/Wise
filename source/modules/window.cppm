module;

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>
#include <utility>

export module window;
import context;

namespace WisE {

export class Window {

private:
public:
  struct WindowSettings {
    std::pair<uint16_t, uint16_t> getWindowConfgis(Configs& config) {
      return {config.WIDTH, config.HEIGHT};
    }
    const char* appname{};
    const char* appversion{};
    const char* appID{};
  };
  SDL_Window* window = nullptr;

  void initWindow(WindowSettings& ws, Configs& config) {
    auto [width, heght] = ws.getWindowConfgis(config);

    SDL_SetAppMetadata(ws.appname, ws.appversion, ws.appID);

    if (!SDL_Init(SDL_INIT_VIDEO)) {

      SDL_Log("Could'nt Initializing SDL: %s", SDL_GetError());
    }

    window = SDL_CreateWindow(ws.appname, width, heght,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (!window) {
      SDL_Log("Error Creating Window: %s", SDL_GetError());
    }
    SDL_Log("SDL Initialized : %p", (void*)window);
    SDL_SetWindowRelativeMouseMode(window, true);
  }
};
} // namespace WisE

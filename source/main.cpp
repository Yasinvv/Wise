
#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_image/SDL_image.h>
#include <cstddef>
#include <cstdlib>
#include <glm/glm.hpp>
#include <inttypes.h>
#include <iostream>

#define SDL_Flags SDL_INIT_VIDEO

static char debug_string[32];

struct AppState {
  bool running = true;
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* idleTex = nullptr;
  SDL_Texture* backTex = nullptr;
  uint16_t Width = 800;
  uint16_t Height = 600;
  float frame = 0;
};

struct Player {
  int wasd = 0;
  float PXP = 0;
  float PYP = 0;
  float MovementSpeed = 0.1;
  bool PRotated = false;
} P;

class App {

public:
  AppState State;

  void run() {
    init();
    while (State.running) {
      AppEvents();
      Uint64 dt_ns;
      Uint64 now;
      Time(now, dt_ns);
      animation(now);
      rendering(dt_ns);
    }
    cleanup(State);
  }

private:
  void init() {

    SDL_SetAppMetadata("Void", "0.0.1", "Wise");

    if (!SDL_Init(SDL_Flags)) {

      SDL_Log("Could'nt Initializing SDL: %s", SDL_GetError());
      cleanup(State);
    }

    // Creating Window
    State.window = SDL_CreateWindow("Demo", State.Width, State.Height,
                                    SDL_WINDOW_RESIZABLE);
    if (!State.window) {
      SDL_Log("Error Creating Window: %s", SDL_GetError());
      cleanup(State);
    }

    // Creating Renderer
    State.renderer = SDL_CreateRenderer(State.window, nullptr);
    if (!State.renderer) {
      SDL_Log("Error Creating Renderer: %s", SDL_GetError());
      cleanup(State);
    }
    // Leading The Texture

    State.backTex = IMG_LoadTexture(State.renderer, "../data/back.png");

    State.idleTex = IMG_LoadTexture(State.renderer, "../data/idle.png");
    // Set Presentation
    SDL_SetRenderLogicalPresentation(State.renderer, 640, 320,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_Log("SDL Initialized");
  }

  void AppEvents() {

    SDL_Event event{0};
    int Current_Window_Width = event.window.data1;
    int Current_Window_Height = event.window.data2;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        State.running = false;
        break;
      case SDL_EVENT_WINDOW_RESIZED:
        SDL_Log(" %d %d ", Current_Window_Width, Current_Window_Height);
        break;

      case SDL_EVENT_KEY_DOWN:
        if (event.key.scancode == SDL_SCANCODE_D) {
          P.wasd |= 1;
          P.PRotated = false;
          SDL_Log("wasd is %d %lf %lf", P.wasd, P.PXP, P.PYP);
        }
        if (event.key.scancode == SDL_SCANCODE_A) {
          P.wasd |= 4;
          P.PRotated = true;
          SDL_Log("wasd is %d %lf %lf", P.wasd, P.PXP, P.PYP);
        }
        if (event.key.scancode == SDL_SCANCODE_W) {
          P.wasd |= 8;
          SDL_Log("wasd is %d %lf %lf", P.wasd, P.PXP, P.PYP);
        }
        if (event.key.scancode == SDL_SCANCODE_S) {
          P.wasd |= 2;
          SDL_Log("wasd is %d %lf %lf", P.wasd, P.PXP, P.PYP);
        }
        break;
      case SDL_EVENT_KEY_UP:
        if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
          State.running = false;
        }
        if (event.key.scancode == SDL_SCANCODE_D) {
          P.wasd &= 30;
          SDL_Log("wasd is %d %lf %lf", P.wasd, P.PXP, P.PYP);
        }
        if (event.key.scancode == SDL_SCANCODE_A) {
          P.wasd &= 27;
          SDL_Log("wasd is %d %lf %lf", P.wasd, P.PXP, P.PYP);
        }
        if (event.key.scancode == SDL_SCANCODE_W) {
          P.wasd &= 23;
          SDL_Log("wasd is %d %lf %lf", P.wasd, P.PXP, P.PYP);
        }
        if (event.key.scancode == SDL_SCANCODE_S) {
          P.wasd &= 29;
          SDL_Log("wasd is %d %lf %lf", P.wasd, P.PXP, P.PYP);
        }
        break;
      }
    }
  }

  void rendering(Uint64 dt_ns) {
    dt_ns = dt_ns / 1000000.0f;

    SDL_FRect src{
        .x = 0 + (32 * State.frame),
        .y = 0,
        .w = 32,
        .h = 32,
    };

    SDL_FRect dst{
        .x = (P.PXP += ((P.wasd & 1 ? (P.MovementSpeed * dt_ns) : 0.0)) -
                       (P.wasd & 4 ? (P.MovementSpeed * dt_ns) : 0.0)),
        .y = (P.PYP += ((P.wasd & 2 ? (P.MovementSpeed * dt_ns) : 0.0)) -
                       (P.wasd & 8 ? (P.MovementSpeed * dt_ns) : 0.0)),

        .w = 32,
        .h = 32,

    };

    SDL_SetRenderDrawColor(State.renderer, 20, 10, 30, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(State.renderer);

    SDL_RenderTexture(State.renderer, State.backTex, NULL, NULL);
    SDL_RenderTextureRotated(
        State.renderer, State.idleTex, &src, &dst, 0, NULL,
        (P.PRotated ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));

    SDL_SetRenderClipRect(State.renderer, 0);
    SDL_SetRenderDrawColor(State.renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(State.renderer, 0, 0, debug_string);

    SDL_SetRenderVSync(State.renderer, 1);

    SDL_RenderPresent(State.renderer);
    // SDL_Log("%" PRIu64, (dt_sc));
  }
  void cleanup(AppState& state) {
    SDL_DestroyTexture(State.backTex);
    SDL_DestroyTexture(State.idleTex);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
    std::cout << "App is Closed" << std::endl;
  }

  void animation(Uint64 now) {
    static Uint64 last = 0;
    if (now - last > 200000000) {
      last = now;
      State.frame += 1;
      if (State.frame >= 8) {
        State.frame = 0;
      }
    }
  }
  void Time(Uint64& now, Uint64& dt_ns) {
    static Uint64 accu = 0;
    static Uint64 last = 0;
    static Uint64 past = 0;

    now = SDL_GetTicksNS();
    dt_ns = now - past;
    if (now - last > 999999999) {
      last = now;
      SDL_snprintf(debug_string, sizeof(debug_string), "%" SDL_PRIu64 " fps",
                   accu);
      accu = 0;
    }
    past = now;
    accu += 1;
    Uint64 elapsed = SDL_GetTicksNS() - now;
    if (elapsed < 999999) {
      SDL_DelayNS(999999 - elapsed);
    }
  }
};

int main(int argc, char* argv[]) {
  App app;

  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

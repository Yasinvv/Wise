
/*
 * Part of WisE
 * Developed by Yasinvv (2026)
 * * This file ports and adapts concepts from Vulkan-Tutorial by Alexander
 * Overvoorde.
 * Licensed under CC BY-SA 4.0 (See /LICENSE for details)
 */
module;

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <chrono>
#include <thread>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

export module engine;

import context;
import window;
import time;
import camera;
import luaConfigs;
import physicalDevice;
import extra;
import instance;
import vk_debug;
import surface;
import logicalDevice;
import swapchain;
import imageViews;
import descriptor;
import pipeline;
import commandPool;
import depthResource;
import commandBuffer;
import texture;
import model;
import systemObject;
import grid;
import imGUI;
import draw;

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

const std::vector<char const*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

export class APP {
public:
  WisE::Configs configs;
  void run() {
    WisE::Window::WindowSettings n1_ws{
        .appname = "WisE", .appversion = "0.0.1", .appID = "1"};
    n1_window.initWindow(n1_ws, configs);
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  bool appState{true};
  SDL_Event event{0};
  WisE::VK_CTX ctx;
  WisE::Path path;
  WisE::UniformTime timer;
  WisE::Camera camera;
  WisE::Window n0_window;
  WisE::Window n1_window;
  WisE::Instance n0_instance;
  WisE::VK_Debug n0_debugMessenger;
  WisE::Surface n0_surface;
  WisE::PhysicalDevice n0_physicalDevice;
  WisE::LogicalDevice n0_logicalDevice;
  WisE::Swapchain n0_swapchain;
  WisE::ImageViews n0_imageViews;
  WisE::Pipeline n0_pipeline;
  WisE::Descriptor n0_descriptor;
  WisE::CommandPool n0_commandPool;
  WisE::DepthResource n0_depthResource;
  WisE::CommandBuffer n0_commandBuffer;
  WisE::Texture n0_texture;
  WisE::Model n0_model;
  WisE::SystemObject n0_systemObject;
  WisE::Model_CTX model_ctx;
  WisE::ImGUI mainGUI;
  WisE::Grid grid;
  WisE::Draw draw;

  WisE::InfiniteGrid m_infiniteGrid;
  WisE::InfiniteGrid m1_infiniteGrid;

  void framebufferResizeCallback([[maybe_unused]] int width,
                                 [[maybe_unused]] int height) {
    ctx.framebufferResized = true;
  }

  void initVulkan() {
    path.MODEL_PATH = "data/models/viking_room.obj";
    path.TEXTURE_PATH = "data/textures/viking_room.png";

    n0_instance.createInstance(ctx);
    SDL_Log("n0_instance OK");

    n0_debugMessenger.setupDebugMessenger(ctx);
    SDL_Log("n0_debugMessenger OK");

    n0_surface.createSurface(n1_window, ctx);
    SDL_Log("n0_surface OK");

    n0_physicalDevice.pickPhysicalDevice(ctx);
    SDL_Log("n0_physicalDevice OK");

    n0_logicalDevice.createLogicalDevice(ctx);
    SDL_Log("n0_logicalDevice OK");

    n0_swapchain.createSwapChain(ctx, n1_window);
    SDL_Log("n0_swapchain OK");

    n0_imageViews.createImageViews(ctx);
    SDL_Log("n0_imageViews OK");

    n0_descriptor.createDescriptorSetLayout(ctx);
    SDL_Log("n0_descriptor OK");

    n0_pipeline.createGraphicsPipeline(ctx);
    SDL_Log("n0_pipeline OK");

    n0_commandPool.createCommandPool(ctx);
    SDL_Log("n0_commandPool OK");

    n0_depthResource.createDepthResources(ctx);
    SDL_Log("n0_depthResource OK");

    n0_texture.createTextureImage(ctx, n0_commandBuffer, path);
    SDL_Log("n0_textureImage OK");

    n0_texture.createTextureImageView(ctx);
    SDL_Log("n0_textureImageView OK");

    n0_texture.createTextureSampler(ctx);
    SDL_Log("n0_textureSampler OK");

    n0_model.loadModel(path, model_ctx);
    SDL_Log("n0_model OK");

    grid.createGridMesh(ctx, m1_infiniteGrid, n0_commandBuffer);
    SDL_Log("n0_gridMesh OK");

    grid.createGridPipeline(ctx, m1_infiniteGrid);
    SDL_Log("n0_gridPipeLine OK");

    n0_commandBuffer.createVertexBuffer(ctx, model_ctx);
    SDL_Log("n0_vertexBuffer OK");

    n0_commandBuffer.createIndexBuffer(ctx, model_ctx);
    SDL_Log("n0_indexBuffer OK");

    n0_commandBuffer.createUniformBuffers(ctx);
    SDL_Log("n0_unifromBuffer OK");

    n0_descriptor.createDescriptorPool(ctx);
    SDL_Log("n0_descriptorPool OK");

    n0_descriptor.createDescriptorSets(ctx);
    SDL_Log("n0_descriptorSet OK");

    n0_commandBuffer.createCommandBuffers(ctx);
    SDL_Log("n0_commandBuffer OK");

    n0_systemObject.createSyncObjects(ctx);
    SDL_Log("n0_systemObject OK");

    mainGUI.initImGui(ctx, n1_window);
    SDL_Log("n0_imGUI OK");
  }

  void mainLoop() {
    while (appState) {
      float deltatime = timer.getDeltaTime();
      AppEvents();
      camera.updatePlayerMovement(deltatime);
      draw.drawFrame(ctx, deltatime, configs, n0_swapchain, n0_commandBuffer,
                     n1_window.window, event, camera, m1_infiniteGrid,
                     model_ctx);

      FPSCalculation();
    }
    ctx.device.waitIdle();
  }

  void FPSCalculation() {
    if (configs.MaxFPS > 0) {

      float targetFrameRate{1.0f / static_cast<float>(configs.MaxFPS)};

      auto now = std::chrono::high_resolution_clock::now();
      float timeSpent = std::chrono::duration<float>(now - timer.past).count();

      if (timeSpent < targetFrameRate) {

        float sleepTimeSc{targetFrameRate - timeSpent};
        auto sleepDuration{std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::duration<float>(sleepTimeSc))};
        std::this_thread::sleep_for(sleepDuration);
      }
    }
  }

  void AppEvents() {

    while (SDL_PollEvent(&event)) {
      if (ImGui::GetCurrentContext() != nullptr) {
        ImGui_ImplSDL3_ProcessEvent(&event);
      }
      switch (event.type) {
      case SDL_EVENT_QUIT:
        appState = false;
        break;
      case SDL_EVENT_WINDOW_RESIZED: {
        [[maybe_unused]] int Current_Window_Width = event.window.data1;
        [[maybe_unused]] int Current_Window_Height = event.window.data2;

        auto* app = static_cast<APP*>(SDL_GetPointerProperty(
            SDL_GetWindowProperties(SDL_GetWindowFromID(event.window.windowID)),
            "user_pointer", nullptr));
        if (app) {
          app->framebufferResizeCallback(event.window.data1,
                                         event.window.data2);
        }
      } break;
      case SDL_EVENT_KEY_DOWN:
        if (event.key.scancode == SDL_SCANCODE_EQUALS) {
          configs.MaxFPS += 1;
          if (configs.MaxFPS == 0) {
            configs.MaxFPS += 1;
          }
        }
        if (event.key.scancode == SDL_SCANCODE_MINUS) {
          configs.MaxFPS -= 1;
          if (configs.MaxFPS == 0) {
            configs.MaxFPS -= 1;
          }
        }
        if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
          appState = false;
        }
        if (event.key.scancode == SDL_SCANCODE_D) {
          camera.settings.wasd |= 1;
        }
        if (event.key.scancode == SDL_SCANCODE_A) {
          camera.settings.wasd |= 4;
        }
        if (event.key.scancode == SDL_SCANCODE_W) {
          camera.settings.wasd |= 8;
        }
        if (event.key.scancode == SDL_SCANCODE_S) {
          camera.settings.wasd |= 2;
        }
        break;
      case SDL_EVENT_KEY_UP:
        if (event.key.scancode == SDL_SCANCODE_D) {
          camera.settings.wasd &= 30;
        }
        if (event.key.scancode == SDL_SCANCODE_A) {
          camera.settings.wasd &= 27;
        }
        if (event.key.scancode == SDL_SCANCODE_W) {
          camera.settings.wasd &= 23;
        }
        if (event.key.scancode == SDL_SCANCODE_S) {
          camera.settings.wasd &= 29;
        }
        break;
      case SDL_EVENT_MOUSE_MOTION:
        camera.settings.addRotation(event.motion.xrel, event.motion.yrel);
        break;
      }
    }
  }

  void cleanupSwapChain() {
    ctx.swapChainImageViews.clear();
    ctx.swapChain = nullptr;
  }

  void cleanup() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(n1_window.window);
    SDL_Quit();
  }
};

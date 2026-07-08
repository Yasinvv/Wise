/*
 * Part of WisE
 * Copyright 2026 Yasinvv
 * Licensed under the Apache License, Version 2.0 (the "License");
 * See /LICENSE for details
 */

module;

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <chrono>
#include <thread>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

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
  WisE::Pipeline::PipelineConfigs n0_pipelineConfigs;
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

  WisE::Material viking_room_material;
  WisE::Object_CTX viking_room;
  WisE::Material m1_infiniteGrid_material;
  WisE::Object_CTX m1_infiniteGrid;

  void framebufferResizeCallback([[maybe_unused]] int width,
                                 [[maybe_unused]] int height) {
    ctx.framebufferResized = true;
  }

  void init_Configs() {
    path.MODEL_PATH = "data/models/viking_room.obj";
    path.TEXTURE_PATH = "data/textures/viking_room.png";
    n0_pipelineConfigs.shaderPath = "data/shaders/slang.spv";
  }

  void initVulkan() {
    init_Configs();

    viking_room.materialRef = &viking_room_material;
    m1_infiniteGrid.materialRef = &m1_infiniteGrid_material;

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

    n0_descriptor.createDescriptorSetLayout(ctx, viking_room);
    SDL_Log("n0_descriptor OK");

    n0_descriptor.createDescriptorSetLayout(ctx, m1_infiniteGrid);
    SDL_Log("m1_infiniteGrid descriptor layout OK");

    n0_pipeline.createGraphicsPipeline(
        ctx, viking_room.materialRef->descriptorSetLayout, n0_pipelineConfigs);
    SDL_Log("n0_pipeline OK");

    grid.createGridPipeline(ctx, m1_infiniteGrid);
    SDL_Log("n0_gridPipeLine OK");

    n0_commandPool.createCommandPool(ctx);
    SDL_Log("n0_commandPool OK");

    n0_depthResource.createDepthResources(ctx);
    SDL_Log("n0_depthResource OK");

    n0_texture.createTextureImage(ctx, viking_room, n0_commandBuffer, path);
    SDL_Log("n0_textureImage OK");

    n0_texture.createTextureImageView(ctx, viking_room);
    SDL_Log("n0_textureImageView OK");

    n0_texture.createTextureSampler(ctx, viking_room);
    SDL_Log("n0_textureSampler OK");

    n0_model.loadModel(path, model_ctx);
    SDL_Log("n0_model OK");

    grid.createGridMesh(ctx, m1_infiniteGrid, n0_commandBuffer);
    SDL_Log("n0_gridMesh OK");

    n0_commandBuffer.createUniformBuffers(ctx, m1_infiniteGrid);
    SDL_Log("m1_infiniteGrid uniformBuffers OK");

    n0_descriptor.createDescriptorPool(ctx, m1_infiniteGrid);
    n0_descriptor.createDescriptorSets(ctx, m1_infiniteGrid);
    SDL_Log("m1_infiniteGrid Descriptors OK");

    n0_commandBuffer.createVertexBuffer(ctx, viking_room, model_ctx);
    SDL_Log("n0_vertexBuffer OK");

    n0_commandBuffer.createIndexBuffer(ctx, viking_room, model_ctx);
    SDL_Log("n0_indexBuffer OK");

    n0_commandBuffer.createUniformBuffers(ctx, viking_room);
    SDL_Log("n0_unifromBuffer OK");

    n0_descriptor.createDescriptorPool(ctx, viking_room);
    SDL_Log("n0_descriptorPool OK");

    n0_descriptor.createDescriptorSets(ctx, viking_room);
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
      drawFrame(deltatime);
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

  void recordCommandBuffer(auto imageIndex, float& deltaTime) {
    auto& commandBuffer = ctx.commandBuffers[ctx.frameIndex];
    commandBuffer.begin({});

    n0_commandBuffer.transition_image_layout(
        ctx, ctx.swapChainImages[imageIndex], vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal, {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::ImageAspectFlagBits::eColor);

    n0_commandBuffer.transition_image_layout(
        ctx, *ctx.depthImage, vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthAttachmentOptimal,
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        vk::PipelineStageFlagBits2::eEarlyFragmentTests |
            vk::PipelineStageFlagBits2::eLateFragmentTests,
        vk::PipelineStageFlagBits2::eEarlyFragmentTests |
            vk::PipelineStageFlagBits2::eLateFragmentTests,
        vk::ImageAspectFlagBits::eDepth);

    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

    vk::RenderingAttachmentInfo colorAttachmentInfo = {
        .imageView = ctx.swapChainImageViews[imageIndex],
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearColor};

    vk::RenderingAttachmentInfo depthAttachmentInfo = {
        .imageView = ctx.depthImageView,
        .imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearDepth};

    vk::RenderingInfo renderingInfo = {
        .renderArea = {.offset = {0, 0}, .extent = ctx.swapChainExtent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = &depthAttachmentInfo};

    commandBuffer.beginRendering(renderingInfo);

    commandBuffer.setViewport(
        0, vk::Viewport(
               0.0f, 0.0f, static_cast<float>(ctx.swapChainExtent.width),
               static_cast<float>(ctx.swapChainExtent.height), 0.0f, 1.0f));
    commandBuffer.setScissor(
        0, vk::Rect2D(vk::Offset2D(0, 0), ctx.swapChainExtent));

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                               *viking_room.materialRef->graphicsPipeline);
    commandBuffer.bindVertexBuffers(0, *viking_room.vertexBuffer, {0});
    commandBuffer.bindIndexBuffer(
        *viking_room.indexBuffer, 0,
        vk::IndexTypeValue<decltype(model_ctx.indices)::value_type>::value);
    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        viking_room.materialRef->pipelineLayout, 0,
        *viking_room.descriptorSets[ctx.frameIndex], nullptr);
    commandBuffer.drawIndexed(static_cast<uint32_t>(model_ctx.indices.size()),
                              1, 0, 0, 0);

    // grid

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                               *m1_infiniteGrid.materialRef->graphicsPipeline);
    commandBuffer.bindVertexBuffers(0, *m1_infiniteGrid.vertexBuffer, {0});
    commandBuffer.bindIndexBuffer(*m1_infiniteGrid.indexBuffer, 0,
                                  vk::IndexType::eUint32);

    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        *m1_infiniteGrid.materialRef->pipelineLayout, 0,
        *viking_room.descriptorSets[ctx.frameIndex], nullptr);

    commandBuffer.drawIndexed(m1_infiniteGrid.indexCount, 1, 0, 0, 0);

    //
    //
    // IMGUI
    //
    //

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::Begin("Debug Info", nullptr,
                 ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground);

    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "FPS: %.1f (%.4f ms)",
                       1.0f / deltaTime, deltaTime * 1000.0f);
    ImGui::Text("VK1.4\nSDL3");
    ImGui::Separator();
    ImGui::PushItemWidth(50.0f);

    uint8_t minFPS = 1, maxFPS = 240;

    ImGui::SliderScalar("Max FPS Limit ( - , + )", ImGuiDataType_U8,
                        &configs.MaxFPS, &minFPS, &maxFPS, "%u",
                        ImGuiSliderFlags_AlwaysClamp);
    ImGui::PopItemWidth();
    ImGui::End();

    ImGui::Render();

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandBuffer);

    //
    //
    //
    //
    //

    commandBuffer.endRendering();

    /*transition_image_layout(swapChainImages[imageIndex],
                            vk::ImageLayout::eColorAttachmentOptimal,
                            vk::ImageLayout::ePresentSrcKHR,
                            vk::AccessFlagBits2::eColorAttachmentWrite, {},
                            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                            vk::PipelineStageFlagBits2::eBottomOfPipe,
                            vk::ImageAspectFlagBits::eColor);*/

    n0_commandBuffer.transition_image_layout(
        ctx, ctx.swapChainImages[imageIndex],
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite, vk::AccessFlagBits2::eNone,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe,
        vk::ImageAspectFlagBits::eColor);
    commandBuffer.end();
  }
  void drawFrame(float& deltaTime) {
    // Note: inFlightFences, presentCompleteSemaphores, and commandBuffers are
    // indexed by frameIndex,
    //       while renderFinishedSemaphores is indexed by imageIndex
    auto fenceResult = ctx.device.waitForFences(
        {ctx.inFlightFences[ctx.frameIndex]}, vk::True, UINT64_MAX);
    if (fenceResult != vk::Result::eSuccess) {
      throw std::runtime_error("failed to wait for fence!");
    }

    auto [result, imageIndex] = ctx.swapChain.acquireNextImage(
        UINT64_MAX, *ctx.presentCompleteSemaphores[ctx.frameIndex], nullptr);

    // Due to VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS being defined,
    // eErrorOutOfDateKHR can be checked as a result here and does not need to
    // be caught by an exception.
    if (result == vk::Result::eErrorOutOfDateKHR) {
      n0_swapchain.recreateSwapChain(n1_window.window, event);
      return;
    }
    // On other success codes than eSuccess and eSuboptimalKHR we just throw
    // an exception. On any error code, aquireNextImage already threw an
    // exception.
    if (result != vk::Result::eSuccess &&
        result != vk::Result::eSuboptimalKHR) {
      assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
      throw std::runtime_error("failed to acquire swap chain image!");
    }
    n0_commandBuffer.updateUniformBuffer(ctx.frameIndex, ctx, viking_room,
                                         camera);

    // Only reset the fence if we are submitting work
    ctx.device.resetFences(*ctx.inFlightFences[ctx.frameIndex]);

    ctx.commandBuffers[ctx.frameIndex].reset();
    recordCommandBuffer(imageIndex, deltaTime);

    vk::PipelineStageFlags waitDestinationStageMask(
        vk::PipelineStageFlagBits::eColorAttachmentOutput);
    const vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*ctx.presentCompleteSemaphores[ctx.frameIndex],
        .pWaitDstStageMask = &waitDestinationStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &*ctx.commandBuffers[ctx.frameIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*ctx.renderFinishedSemaphores[imageIndex]};
    ctx.queue.submit(submitInfo, *ctx.inFlightFences[ctx.frameIndex]);

    const vk::PresentInfoKHR presentInfoKHR{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*ctx.renderFinishedSemaphores[imageIndex],
        .swapchainCount = 1,
        .pSwapchains = &*ctx.swapChain,
        .pImageIndices = &imageIndex};
    result = ctx.queue.presentKHR(presentInfoKHR);
    // Due to VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS being defined,
    // eErrorOutOfDateKHR can be checked as a result here and does not need to
    // be caught by an exception.
    if ((result == vk::Result::eSuboptimalKHR) ||
        (result == vk::Result::eErrorOutOfDateKHR) || ctx.framebufferResized) {
      ctx.framebufferResized = false;
      n0_swapchain.recreateSwapChain(n1_window.window, event);
    } else {
      // There are no other success codes than eSuccess; on any error code,
      // presentKHR already threw an exception.
      assert(result == vk::Result::eSuccess);
    }
    ctx.frameIndex = (ctx.frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
  }
};

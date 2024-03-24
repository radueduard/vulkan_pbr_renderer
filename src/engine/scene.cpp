//
// Created by radue on 1/23/2024.
//

#include "scene.hpp"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

#include "../engine/graphics/renderPrograms/grid.hpp"
#include "../utils.hpp"
#include "settings.hpp"
#include "../engine/compute/computePrograms/matrixSum.hpp"

namespace ve {
    Scene *Scene::instance = nullptr;

    Scene::Scene(Window &window) : window(window), device(window), renderer(window, device),
                                   camera(75.0f, 0.001f, 1000.0f) {
        window.addInputController(&camera);
        window.addInputController(this);

        framePools.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        const auto framePoolBuilder = DescriptorPool::Builder(device)
            .setMaxSets(1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000);

        for (auto& framePool : framePools) {
            framePool = framePoolBuilder.build();
        }

        Image::loadDefaultImage(device);
    }

    void Scene::initImGui() {
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(window.getWindowHandle(), true);

//        DescriptorPool::Builder poolBuilder(device);
//        static auto imGuiPool = poolBuilder
//                .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000)
//                .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
//                .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
//                .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
//                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
//                .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
//                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
//                .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
//                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
//                .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
//                .addPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)
//                .build();

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = device.getInstance();
        initInfo.PhysicalDevice = device.getPhysicalDevice();
        initInfo.Device = device.getDevice();
        initInfo.Queue = device.getGraphicsQueue();
        initInfo.QueueFamily = device.getQueueFamilyIndices().graphicsFamily;
        initInfo.PipelineCache = VK_NULL_HANDLE;
        initInfo.DescriptorPool = renderer.getGlobalDescriptorPool()->getDescriptorPool(); //imGuiPool->getDescriptorPool();
        initInfo.Allocator = nullptr;
        initInfo.MinImageCount = 2;
        initInfo.ImageCount = 2;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_8_BIT;
        initInfo.CheckVkResultFn = nullptr;
        ImGui_ImplVulkan_Init(&initInfo, renderer.getSwapChainRenderPass());
    }

    void Scene::renderImGui(const VkCommandBuffer& commandBuffer) const {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Frame Time:");
        auto [size, usage] = device.getMemorySize();

        ImGui::Text("Memory usage: %llu / %llu MB", usage / 1024 / 1024, size / 1024 / 1024);
        ImGui::Text("Frame Time: %f", frameTime);
        ImGui::Text("FPS: %f", 1.0f / frameTime * 1000.0f);
        ImGui::End();

        ImGui::Begin("Settings");
        ImGui::Checkbox("V-sync", &(Settings::getInstance()->VSYNC));
        ImGui::End();

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    }

    void Scene::run() {
        std::vector<std::unique_ptr<Buffer>> uniformBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (auto& buffer : uniformBuffers) {
            buffer = std::make_unique<Buffer>(
                    device,
                    sizeof(Camera::CameraBufferData),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            buffer->map();
        }

        globalSetLayout = DescriptorSetLayout::Builder(device)
             .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_VERTEX_BIT)
             .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < globalDescriptorSets.size(); i++) {
            auto cameraTransformationsInfo = uniformBuffers[i]->descriptorInfoForIndex(0);

            DescriptorWriter(*globalSetLayout, *renderer.getGlobalDescriptorPool())
                    .writeBuffer(0, &cameraTransformationsInfo)
                    .build(globalDescriptorSets[i]);
        }

        init();

        initImGui();

        Grid grid(device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        MatrixSum sum(device, globalSetLayout->getDescriptorSetLayout());

        while (!window.shouldClose()) {
            window.pollEvents();
            window.computeDeltaTime();
            window.updateInputs();

            const auto [width, height] = renderer.getSwapChainExtent();
            camera.resize(width, height);
            camera.update(window.getDeltaTime());

            Timer timer;
            if (auto [graphicsCommandBuffer, computeCommandBuffer] = renderer.beginFrame(); 
                graphicsCommandBuffer != VK_NULL_HANDLE && computeCommandBuffer != VK_NULL_HANDLE) {
	            const int frameIndex = renderer.getFrameIndex();
                framePools[frameIndex]->resetPool();

                update(window.getDeltaTime());

                FrameInfo frameInfo{
                        frameIndex,
                        graphicsCommandBuffer,
                        computeCommandBuffer,
                        globalDescriptorSets[frameIndex],
                        *framePools[frameIndex]
                };

                sum.computeMatrixSum(frameInfo);

                renderer.beginSwapChainRenderPass(graphicsCommandBuffer);

                auto cameraBufferData = camera.getCameraBufferData();
                uniformBuffers[frameIndex]->writeToIndex(&cameraBufferData, 0);
                uniformBuffers[frameIndex]->flush();

                render(frameInfo);

                // grid.renderGrid(frameInfo);
                renderImGui(graphicsCommandBuffer);
                renderer.endSwapChainRenderPass(graphicsCommandBuffer);
                renderer.endFrame();

                result = sum.getResult();
            }

            frameTime = timer.ElapsedMillis();
        }
        vkDeviceWaitIdle(device.getDevice());
    }
} // ve
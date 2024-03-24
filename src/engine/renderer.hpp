//
// Created by radue on 1/23/2024.
//

#pragma once

#include "swapChain.hpp"
#include "../log.hpp"
#include "../engine/memory/descriptors.hpp"

namespace ve {
    struct FrameInfo {
        int frameIndex;
        VkCommandBuffer graphicsCommandBuffer;
        VkCommandBuffer computeCommandBuffer;
        VkDescriptorSet globalDescriptorSet;
        DescriptorPool &frameDescriptorPool;
    };

    class Renderer {
    public:
        Renderer(Window &window, Device &device);
        ~Renderer();

        bool shouldRecreateSwapChain = false;

        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;

        VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
        bool isFrameInProgress() const { return isFrameStarted; }
        VkExtent2D getSwapChainExtent() const { return swapChain->getSwapChainExtent(); }

        VkCommandBuffer getCurrentGraphicsCommandBuffer() const {
            if (!isFrameStarted) {
                Log::error("Cannot get command buffer when frame not in progress");
                throw std::runtime_error("");
            }
            return graphicsCommandBuffers[currentFrameIndex];
        }

        VkCommandBuffer getCurrentComputeCommandBuffer() const {
            if (!isFrameStarted) {
                Log::error("Cannot get command buffer when frame not in progress");
                throw std::runtime_error("");
            }
            return computeCommandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            if (!isFrameStarted) {
                Log::error("Cannot get frame index when frame not in progress");
                throw std::runtime_error("");
            }
            return currentFrameIndex;
        }

        std::pair<VkCommandBuffer, VkCommandBuffer> beginFrame();
        void endFrame();

        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer) const;

        float getAspectRatio() const { return swapChain->getExtentAspectRatio(); }

        std::unique_ptr<DescriptorPool> &getGlobalDescriptorPool() { return globalDescriptorPool; }
    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void createSwapChain();

        Window &window;
        Device &device;
        std::unique_ptr<SwapChain> swapChain;

        std::vector<VkCommandBuffer> graphicsCommandBuffers{};
        std::vector<VkCommandBuffer> computeCommandBuffers{};

        int currentImageIndex = 0;
        int currentFrameIndex = 0;
        bool isFrameStarted = false;

        std::unique_ptr<DescriptorPool> globalDescriptorPool;

        void createGlobalDescriptorPool();
    };
}

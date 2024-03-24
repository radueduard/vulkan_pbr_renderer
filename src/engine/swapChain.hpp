//
// Created by radue on 1/23/2024.
//

#pragma once

#include "device.hpp"

// std
#include <memory>

namespace ve {
    class SwapChain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        SwapChain(Device &device, VkExtent2D windowExtent, const std::shared_ptr<SwapChain>& previous = nullptr);
        ~SwapChain();

        SwapChain(const SwapChain &) = delete;
        void operator=(const SwapChain &) = delete;

        VkFramebuffer getFrameBuffer(int index) { return swapChainFrameBuffers[index]; }
        VkRenderPass getRenderPass() { return renderPass; }
        VkImageView getImageView(int index) { return swapChainImageViews[index]; }
        size_t getImageCount() { return swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() { return swapChainExtent; }
        uint32_t width() const { return swapChainExtent.width; }
        uint32_t height() const { return swapChainExtent.height; }

        float getExtentAspectRatio() const {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }

        VkFormat findDepthFormat() {
            return device.findSupportedFormat(
                    {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        }

        VkResult acquireNextImage(uint32_t *imageIndex);
        VkResult submitCommandBuffers(
                const VkCommandBuffer *graphicsBuffers,
                const VkCommandBuffer *computeBuffers,
                uint32_t *imageIndex);

        bool compareSwapFormats(const SwapChain &swapChain) const {
            return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
                   swapChain.swapChainImageFormat == swapChainImageFormat;
        }

    private:
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createDepthResources();
        void createColorResources();
        void createFrameBuffers();
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        VkFormat swapChainImageFormat;
        VkFormat swapChainDepthFormat;
        VkFormat swapChainColorFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFrameBuffers;
        VkRenderPass renderPass;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView> depthImageViews;

        std::vector<VkImage> colorImages;
        std::vector<VkDeviceMemory> colorImageMemorys;
        std::vector<VkImageView> colorImageViews;

        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        Device &device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;
        std::shared_ptr<SwapChain> oldSwapChain;

        std::vector<VkFence> imagesInFlight;
        std::vector<VkSemaphore> imageAvailableSemaphores;

        std::vector<VkFence> graphicsInFlightFences;
        std::vector<VkSemaphore> graphicsFinishedSemaphores;

        std::vector<VkFence> computeInFlightFences;
        std::vector<VkSemaphore> computeFinishedSemaphores;

        size_t currentFrame = 0;
    };
} // ve

//
// Created by radue on 1/23/2024.
//

#pragma once

#include "../window/window.hpp"

// std
#include <string>
#include <vector>

namespace ve {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        uint32_t computeFamily;

        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool computeFamilyHasValue = false;

        bool isComplete() const {
            return graphicsFamilyHasValue && presentFamilyHasValue && computeFamilyHasValue;
        }
    };

    class Device {
    public:

#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

        explicit Device(Window &window);
        ~Device();

        Device(const Device &) = delete;
        void operator=(const Device &) = delete;
        Device(Device &&) = delete;
        Device &operator=(Device &&) = delete;

        VkInstance getInstance() { return instance; }
        VkCommandPool getCommandPool() { return graphicsCommandPool; }
        VkPhysicalDevice getPhysicalDevice() { return physicalDevice; }
        VkDevice getDevice() { return device; }
        VkSurfaceKHR getSurface() { return surface; }
        VkQueue getGraphicsQueue() { return graphicsQueue; }
        VkQueue getPresentQueue() { return presentQueue; }
        VkQueue getComputeQueue() { return computeQueue; }
        QueueFamilyIndices getQueueFamilyIndices() { return findPhysicalQueueFamilies(); }
        std::pair<uint64_t, uint64_t> getMemorySize() const;

        SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }
        VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        void createBuffer(
                VkDeviceSize size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties,
                VkBuffer &buffer,
                VkDeviceMemory &bufferMemory);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void createImageWithInfo(
                const VkImageCreateInfo &imageInfo,
                VkMemoryPropertyFlags properties,
                VkImage &image,
                VkDeviceMemory &imageMemory);

        VkPhysicalDeviceProperties properties;

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount);


    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void createLogicalDevice();
        void createCommandPools();


        // helper functions
        void pickPhysicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char *> getRequiredExtensions() const;
        bool checkValidationLayerSupport();
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        void hasGLFWRequiredInstanceExtensions();
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice);

        Window &window;

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkCommandPool graphicsCommandPool;
        VkCommandPool computeCommandPool;

        VkDevice device;
        VkSurfaceKHR surface;

        VkQueue computeQueue;
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        const std::vector<const char *> validationLayers = {
                "VK_LAYER_KHRONOS_validation"
        };
        const std::vector<const char *> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        uint64_t memorySize = 0;
    };
} // ve

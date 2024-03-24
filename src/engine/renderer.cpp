//
// Created by radue on 1/23/2024.
//

#include <array>
#include "renderer.hpp"

#include "../engine/memory/descriptors.hpp"
#include "settings.hpp"

namespace ve {
    Renderer::Renderer(Window &window, Device &device)
    : window(window), device(device) {
        createSwapChain();
        createCommandBuffers();
        createGlobalDescriptorPool();
    }

    Renderer::~Renderer() {
        freeCommandBuffers();
    }

    std::pair<VkCommandBuffer, VkCommandBuffer> Renderer::beginFrame() {
        if(isFrameStarted) {
            Log::error("Can't call beginFrame while already in progress");
            throw std::runtime_error("");
        }

        auto result = swapChain->acquireNextImage(reinterpret_cast<uint32_t *>(&currentImageIndex));

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            createSwapChain();
            return { VK_NULL_HANDLE, VK_NULL_HANDLE };
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            Log::error("Failed to acquire swap chain image!");
            throw std::runtime_error("");
        }

        isFrameStarted = true;

        auto graphicsCommandBuffer = getCurrentGraphicsCommandBuffer();
        auto computeCommandBuffer = getCurrentComputeCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(graphicsCommandBuffer, &beginInfo) != VK_SUCCESS) {
            Log::error("Failed to begin recording graphics command buffer!");
            throw std::runtime_error("");
        }

        if (vkBeginCommandBuffer(computeCommandBuffer, &beginInfo) != VK_SUCCESS) {
            Log::error("Failed to begin recording compute command buffer!");
            throw std::runtime_error("");
        }

        return { graphicsCommandBuffer, computeCommandBuffer };
    }

    void Renderer::endFrame() {
        if (!isFrameStarted) {
            Log::error("Can't call endFrame while frame is not in progress");
            throw std::runtime_error("");
        }

        auto graphicsCommandBuffer = getCurrentGraphicsCommandBuffer();
        auto computeCommandBuffer = getCurrentComputeCommandBuffer();

        if (vkEndCommandBuffer(graphicsCommandBuffer) != VK_SUCCESS) {
            Log::error("Failed to record graphics command buffer!");
            throw std::runtime_error("");
        }

        if (vkEndCommandBuffer(computeCommandBuffer) != VK_SUCCESS) {
            Log::error("Failed to record compute command buffer!");
            throw std::runtime_error("");
        }

        auto result = swapChain->submitCommandBuffers(&graphicsCommandBuffer, &computeCommandBuffer, reinterpret_cast<uint32_t *>(&currentImageIndex));

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || Settings::changed()) {
            Settings::update();
            createSwapChain();
        } else if (result != VK_SUCCESS) {
            Log::error("Failed to present swap chain image!");
            throw std::runtime_error("");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        if (!isFrameStarted) {
            Log::error("Can't call beginSwapChainRenderPass if frame is not in progress");
            throw std::runtime_error("");
        }

        if (commandBuffer != getCurrentGraphicsCommandBuffer()) {
            Log::error("Can't begin render pass on command buffer from a different frame");
            throw std::runtime_error("");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->getRenderPass();
        renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChain->getSwapChainExtent().width;
        viewport.height = (float) swapChain->getSwapChainExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChain->getSwapChainExtent();

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) const {
        if (!isFrameStarted) {
            Log::error("Can't call endSwapChainRenderPass if frame is not in progress");
            throw std::runtime_error("");
        }

        if (commandBuffer != getCurrentGraphicsCommandBuffer()) {
            Log::error("Can't end render pass on command buffer from a different frame");
            throw std::runtime_error("");
        }

        vkCmdEndRenderPass(commandBuffer);
    }

    void Renderer::createCommandBuffers() {
        graphicsCommandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(graphicsCommandBuffers.size());

        if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, graphicsCommandBuffers.data()) != VK_SUCCESS) {
            Log::error("Failed to allocate command buffers!");
            throw std::runtime_error("");
        }

        computeCommandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        allocInfo.commandBufferCount = static_cast<uint32_t>(computeCommandBuffers.size());

        if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, computeCommandBuffers.data()) != VK_SUCCESS) {
            Log::error("Failed to allocate command buffers!");
            throw std::runtime_error("");
        }
    }

    void Renderer::freeCommandBuffers() {
        vkFreeCommandBuffers(
                device.getDevice(),
                device.getCommandPool(),
                static_cast<uint32_t>(graphicsCommandBuffers.size()),
                graphicsCommandBuffers.data());
        graphicsCommandBuffers.clear();

        vkFreeCommandBuffers(
                device.getDevice(),
                device.getCommandPool(),
                static_cast<uint32_t>(computeCommandBuffers.size()),
                computeCommandBuffers.data());
        computeCommandBuffers.clear();
    }

    void Renderer::createSwapChain() {
        auto extent = window.getExtent();

        vkDeviceWaitIdle(device.getDevice());

        if (swapChain == nullptr) {
            swapChain = std::make_unique<SwapChain>(device, extent);
        } else {
            std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain);
            swapChain = std::make_unique<SwapChain>(device, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormats(*swapChain)) {
                Log::error("Swap chain image(or depth) format has changed!");
                throw std::runtime_error("");
            }
        }
    }

    void Renderer::createGlobalDescriptorPool() {
        DescriptorPool::Builder poolBuilder(device);
        globalDescriptorPool = poolBuilder
            .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)
            .build();
    }
} // ve

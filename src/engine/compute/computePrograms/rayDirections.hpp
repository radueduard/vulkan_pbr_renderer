//
// Created by radue on 1/26/2024.
//

#pragma once

#include <vulkan/vulkan.h>
#include "../../../engine/device.hpp"
#include "../../../engine/renderer.hpp"
#include "../../../engine/compute/computePipeline.hpp"
#include "../../../engine/memory/buffer.hpp"

class RayDirections {

public:
    RayDirections(ve::Device& device, VkDescriptorSetLayout globalSetLayout);
    ~RayDirections();

    RayDirections(const RayDirections&) = delete;
    RayDirections& operator=(const RayDirections&) = delete;

    void computeRayDirections(const ve::FrameInfo& frameInfo);

    void setScreenSize(const glm::ivec2& screenSize) {
        this->screenSize = screenSize;
        screenSizeBuffer->writeToBuffer((void *) &screenSize, sizeof(glm::ivec2));
    }

    std::vector<glm::vec3> getRayDirections() {
        std::vector<glm::vec3> rayDirections(screenSize.x * screenSize.y);
        memcpy (rayDirections.data(), rayDirectionsBuffer->getMappedMemory(), rayDirections.size() * sizeof(glm::vec3));
        return rayDirections;
    }

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline();

    ve::Device& device;
    std::unique_ptr<ve::ComputePipeline> pipeline;
    VkPipelineLayout pipelineLayout;

    std::unique_ptr<ve::DescriptorSetLayout> programLayout;

    glm::ivec2 screenSize;
    std::unique_ptr<ve::Buffer> rayDirectionsBuffer;
    std::unique_ptr<ve::Buffer> screenSizeBuffer;
};

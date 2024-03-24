//
// Created by radue on 1/26/2024.
//

#include "rayDirections.hpp"

RayDirections::RayDirections(ve::Device &device, VkDescriptorSetLayout globalSetLayout) : device(device) {
    createPipelineLayout(globalSetLayout);
    createPipeline();

    rayDirectionsBuffer = std::make_unique<ve::Buffer>(
            device,
            2560 * 1440 * sizeof(glm::vec3),
            1,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    rayDirectionsBuffer->map();

    auto rayDirection = glm::vec3(0.0f);
    for (int i = 0; i < 2560 * 1440; i++) {
        rayDirectionsBuffer->writeToBuffer(&rayDirection, sizeof(glm::vec3), sizeof(glm::vec3) * i);
    }

    rayDirectionsBuffer->flush();
}

RayDirections::~RayDirections() {
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

void RayDirections::computeRayDirections(const ve::FrameInfo &frameInfo) {
    pipeline->bind(frameInfo.computeCommandBuffer);

    vkCmdBindDescriptorSets(
            frameInfo.computeCommandBuffer,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);

    auto rayDirectionsBufferInfo = rayDirectionsBuffer->descriptorInfoForIndex(0);
    auto screenSizeBufferInfo = screenSizeBuffer->descriptorInfoForIndex(0);

    VkDescriptorSet rayDirectionsDescriptorSet;
    ve::DescriptorWriter(*programLayout, frameInfo.frameDescriptorPool)
            .writeBuffer(0, &rayDirectionsBufferInfo)
            .writeBuffer(1, &screenSizeBufferInfo)
            .build(rayDirectionsDescriptorSet);

    vkCmdBindDescriptorSets(
            frameInfo.computeCommandBuffer,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            pipelineLayout,
            1,
            1,
            &rayDirectionsDescriptorSet,
            0,
            nullptr);

    vkCmdDispatch(frameInfo.computeCommandBuffer, 160, 160, 1);
}

void RayDirections::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    programLayout = ve::DescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
            .addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
            .build();

    std::vector<VkDescriptorSetLayout> layouts = {
            globalSetLayout,
            programLayout->getDescriptorSetLayout()
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = layouts.size();
    pipelineLayoutInfo.pSetLayouts = layouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void RayDirections::createPipeline() {
    if (programLayout == nullptr) {
        Log::error("Cannot create pipeline without program layout!");
        throw std::runtime_error("");
    }

    std::string shader = "shaders/compiled/rayDirections.comp.spv";
    pipeline = std::make_unique<ve::ComputePipeline>(device, shader, pipelineLayout);
}

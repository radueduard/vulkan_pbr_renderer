//
// Created by radue on 1/24/2024.
//

#include "matrixSum.hpp"

MatrixSum::MatrixSum(ve::Device &device, VkDescriptorSetLayout globalSetLayout) : device(device) {
    createPipelineLayout(globalSetLayout);
    createPipeline();

    matrixBuffer = std::make_unique<ve::Buffer>(
            device,
            sizeof(glm::mat4) * 3,
            1,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    matrixBuffer->map();

    matrixSumBuffer = std::make_unique<ve::Buffer>(
            device,
            sizeof(glm::mat4),
            1,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    matrixSumBuffer->map();

    glm::mat4 matrix = glm::mat4(1.0f);
    matrixBuffer->writeToBuffer(&matrix, sizeof(glm::mat4));
    matrixBuffer->writeToBuffer(&matrix, sizeof(glm::mat4), sizeof(glm::mat4));
    matrixBuffer->writeToBuffer(&matrix, sizeof(glm::mat4), sizeof(glm::mat4) * 2);

    matrixBuffer->flush();

    glm::mat4 matrixSum = glm::mat4(0.0f);
    matrixSumBuffer->writeToBuffer(&matrixSum, sizeof(glm::mat4));
    matrixSumBuffer->flush();
}

MatrixSum::~MatrixSum() {
    vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

void MatrixSum::computeMatrixSum(const ve::FrameInfo &frameInfo) {
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

    auto matrixBufferInfo = matrixBuffer->descriptorInfoForIndex(0);
    auto matrixSumBufferInfo = matrixSumBuffer->descriptorInfoForIndex(0);

    VkDescriptorSet matrixDescriptorSet;
    ve::DescriptorWriter(*programLayout, frameInfo.frameDescriptorPool)
            .writeBuffer(0, &matrixBufferInfo)
            .writeBuffer(1, &matrixSumBufferInfo)
            .build(matrixDescriptorSet);

    vkCmdBindDescriptorSets(
            frameInfo.computeCommandBuffer,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            pipelineLayout,
            1,
            1,
            &matrixDescriptorSet,
            0,
            nullptr);

    vkCmdDispatch(frameInfo.computeCommandBuffer, 1, 1, 1);
}

glm::mat4 MatrixSum::getResult() {
    glm::mat4 result = *(glm::mat4*)matrixSumBuffer->getMappedMemory();

//    matrixBuffer->writeToBuffer(&result, sizeof(glm::mat4));
//    matrixBuffer->writeToBuffer(&result, sizeof(glm::mat4), sizeof(glm::mat4));
//    matrixBuffer->writeToBuffer(&result, sizeof(glm::mat4), sizeof(glm::mat4) * 2);
//    matrixBuffer->flush();

    return result;
}

void MatrixSum::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    programLayout = ve::DescriptorSetLayout::Builder(device)
        .addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
        .addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
        .build();

    std::vector<VkDescriptorSetLayout> layouts = {
        globalSetLayout,
        programLayout->getDescriptorSetLayout()
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
    pipelineLayoutInfo.pSetLayouts = layouts.data();

    if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        Log::error("Failed to create pipeline layout!");
        throw std::runtime_error("");
    }
}

void MatrixSum::createPipeline() {
    if (programLayout == nullptr) {
        Log::error("Cannot create pipeline before creating program layout!");
        throw std::runtime_error("");
    }

    std::string shader = "shaders/compiled/matrixSum.comp.spv";

    pipeline = std::make_unique<ve::ComputePipeline>(device, shader, pipelineLayout);
}

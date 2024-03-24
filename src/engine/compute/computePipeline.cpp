//
// Created by radue on 1/24/2024.
//

#include "computePipeline.hpp"
#include "../../log.hpp"
#include "../../utils.hpp"

namespace ve {
    ComputePipeline::ComputePipeline(Device &device, const std::string &shaderFile, VkPipelineLayout layout) : device(device) {
        createComputePipeline(shaderFile, layout);
    }

    ComputePipeline::~ComputePipeline() {
        vkDestroyShaderModule(device.getDevice(), shaderModule, nullptr);
        vkDestroyPipeline(device.getDevice(), computePipeline, nullptr);
    }

    void ComputePipeline::createComputePipeline(const std::string &shaderFile, VkPipelineLayout layout) {
        if (shaderFile.empty()) {
            Log::error("Failed to create compute pipeline: no shader file given");
            throw std::runtime_error("");
        }

        auto shaderCode = readFile(shaderFile);
        createShaderModule(device, shaderCode, &shaderModule);

        VkPipelineShaderStageCreateInfo shaderStage{};
        shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        shaderStage.module = shaderModule;
        shaderStage.pName = "main";

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.stage = shaderStage;
        pipelineInfo.layout = layout;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        if (vkCreateComputePipelines(device.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline) != VK_SUCCESS) {
            Log::error("Failed to create compute pipeline!");
            throw std::runtime_error("");
        }
    }

    void ComputePipeline::bind(VkCommandBuffer commandBuffer) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    }
} // ve
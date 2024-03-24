//
// Created by radue on 1/24/2024.
//

#pragma once

#include "../device.hpp"

namespace ve {

	class ComputePipeline {
    public:
        ComputePipeline(Device& device, const std::string& shaderFile, VkPipelineLayout layout);
        ~ComputePipeline();

        ComputePipeline(const ComputePipeline&) = delete;
        ComputePipeline& operator=(const ComputePipeline&) = delete;

        void bind(VkCommandBuffer commandBuffer);

    private:
        void createComputePipeline(const std::string& shaderFile, VkPipelineLayout layout);

        Device& device;
        VkPipeline computePipeline = VK_NULL_HANDLE;

        VkShaderModule shaderModule;
    };

} // ve
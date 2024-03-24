//
// Created by radue on 1/23/2024.
//

#pragma once

#include <unordered_map>
#include "../../engine/device.hpp"

namespace ve {
    struct GraphicsPipelineConfigInfo {
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        VkPipelineTessellationStateCreateInfo tessellationInfo;
        uint32_t subpass = 0;
    };

    struct ShaderFiles {
        std::string vertFile;
        std::string tescFile;
        std::string teseFile;
        std::string geomFile;
        std::string fragFile;
    };

    class GraphicsPipeline {
    public:
        GraphicsPipeline(Device& device, ShaderFiles& shaderFiles, GraphicsPipelineConfigInfo& configInfo);
        ~GraphicsPipeline();

        GraphicsPipeline(const GraphicsPipeline&) = delete;
        GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        static void defaultPipelineConfigInfo(GraphicsPipelineConfigInfo& configInfo);

    private:
        void createGraphicsPipeline(const ShaderFiles& shaderFiles, const GraphicsPipelineConfigInfo& configInfo);

        Device& device;
        VkPipeline graphicsPipeline = VK_NULL_HANDLE;

        std::unordered_map<VkShaderStageFlagBits, VkShaderModule> shaderModules;
    };
}

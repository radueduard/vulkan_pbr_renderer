//
// Created by radue on 1/23/2024.
//

#include "graphicsPipeline.hpp"
#include "../../utils.hpp"
#include "../../log.hpp"
#include "Mesh.hpp"

namespace ve {
    GraphicsPipeline::GraphicsPipeline(Device &device, ShaderFiles& shaderFiles, GraphicsPipelineConfigInfo &configInfo) :device(device) {
        createGraphicsPipeline(shaderFiles, configInfo);
    }

    GraphicsPipeline::~GraphicsPipeline() {
        for (auto& [_, shaderModule] : shaderModules) {
            if (shaderModule != VK_NULL_HANDLE) {
                vkDestroyShaderModule(device.getDevice(), shaderModule, nullptr);
            }
        }
        vkDestroyPipeline(device.getDevice(), graphicsPipeline, nullptr);
    }


    void GraphicsPipeline::createGraphicsPipeline(const ShaderFiles& shaderFiles, const GraphicsPipelineConfigInfo &configInfo) {
        if (configInfo.pipelineLayout == nullptr) {
            Log::error("Cannot create graphics pipeline: no pipelineLayout provided!");
            throw std::runtime_error("");
        }

        if (configInfo.renderPass == nullptr) {
            Log::error("Cannot create graphics pipeline: no renderPass provided!");
            throw std::runtime_error("");
        }

        if (shaderFiles.vertFile.empty() || shaderFiles.fragFile.empty()) {
            Log::error("Cannot create graphics pipeline: no vertex or fragment shader provided!");
            throw std::runtime_error("");
        }

        int stageCount = 2;

        VkShaderModule shaderModule;

        auto vertShaderCode = readFile(shaderFiles.vertFile);
        createShaderModule(device, vertShaderCode, &shaderModule);
        shaderModules[VK_SHADER_STAGE_VERTEX_BIT] = shaderModule;

        if (!shaderFiles.tescFile.empty() && !shaderFiles.teseFile.empty()) {
            auto tescShaderCode = readFile(shaderFiles.tescFile);
            createShaderModule(device, tescShaderCode, &shaderModule);
            shaderModules[VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT] = shaderModule;

            auto teseShaderCode = readFile(shaderFiles.teseFile);
            createShaderModule(device, teseShaderCode, &shaderModule);
            shaderModules[VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT] = shaderModule;

            stageCount += 2;
        }

        if (!shaderFiles.geomFile.empty()) {
            auto geomShaderCode = readFile(shaderFiles.geomFile);
            createShaderModule(device, geomShaderCode, &shaderModule);
            shaderModules[VK_SHADER_STAGE_GEOMETRY_BIT] = shaderModule;

            stageCount += 1;
        }

        auto fragShaderCode = readFile(shaderFiles.fragFile);
        createShaderModule(device, fragShaderCode, &shaderModule);
        shaderModules[VK_SHADER_STAGE_FRAGMENT_BIT] = shaderModule;

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages(stageCount);
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = shaderModules[VK_SHADER_STAGE_VERTEX_BIT];
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[stageCount - 1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[stageCount - 1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[stageCount - 1].module = shaderModules[VK_SHADER_STAGE_FRAGMENT_BIT];
        shaderStages[stageCount - 1].pName = "main";
        shaderStages[stageCount - 1].flags = 0;
        shaderStages[stageCount - 1].pNext = nullptr;
        shaderStages[stageCount - 1].pSpecializationInfo = nullptr;

        if (!shaderFiles.tescFile.empty() && !shaderFiles.teseFile.empty()) {
            shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[1].stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            shaderStages[1].module = shaderModules[VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT];
            shaderStages[1].pName = "main";
            shaderStages[1].flags = 0;
            shaderStages[1].pNext = nullptr;
            shaderStages[1].pSpecializationInfo = nullptr;

            shaderStages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[2].stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            shaderStages[2].module = shaderModules[VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT];
            shaderStages[2].pName = "main";
            shaderStages[2].flags = 0;
            shaderStages[2].pNext = nullptr;
            shaderStages[2].pSpecializationInfo = nullptr;
        }

        if (!shaderFiles.geomFile.empty()) {
            shaderStages[stageCount - 2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[stageCount - 2].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            shaderStages[stageCount - 2].module = shaderModules[VK_SHADER_STAGE_GEOMETRY_BIT];
            shaderStages[stageCount - 2].pName = "main";
            shaderStages[stageCount - 2].flags = 0;
            shaderStages[stageCount - 2].pNext = nullptr;
            shaderStages[stageCount - 2].pSpecializationInfo = nullptr;
        }

        const auto bindingDescriptions = Mesh::Vertex::getBindingDescriptions();
        const auto attributeDescriptions = Mesh::Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &configInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
        pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
        pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
        pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;
        pipelineInfo.pTessellationState = &configInfo.tessellationInfo;
        pipelineInfo.layout = configInfo.pipelineLayout;
        pipelineInfo.renderPass = configInfo.renderPass;
        pipelineInfo.subpass = configInfo.subpass;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(
                device.getDevice(),
                VK_NULL_HANDLE,
                1,
                &pipelineInfo,
                nullptr,
                &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }
    }

    void GraphicsPipeline::defaultPipelineConfigInfo(GraphicsPipelineConfigInfo& configInfo) {
        configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        configInfo.viewportInfo.viewportCount = 1;
        configInfo.viewportInfo.pViewports = nullptr;
        configInfo.viewportInfo.scissorCount = 1;
        configInfo.viewportInfo.pScissors = nullptr;

        configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        configInfo.rasterizationInfo.lineWidth = 1.0f;
        configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
        configInfo.rasterizationInfo.depthBiasClamp = 0.0f;          // Optional
        configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;    // Optional

        configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_8_BIT;
        configInfo.multisampleInfo.minSampleShading = 1.0f;          // Optional
        configInfo.multisampleInfo.pSampleMask = nullptr;            // Optional
        configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;      // Optional

        configInfo.colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
        configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
        configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

        configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
        configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
        configInfo.colorBlendInfo.attachmentCount = 1;
        configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
        configInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
        configInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
        configInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
        configInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

        configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        configInfo.depthStencilInfo.minDepthBounds = 0.0f; // Optional
        configInfo.depthStencilInfo.maxDepthBounds = 1.0f; // Optional
        configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        configInfo.depthStencilInfo.front = {}; // Optional
        configInfo.depthStencilInfo.back = {};  // Optional

        configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
        configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
        configInfo.dynamicStateInfo.flags = 0;

        configInfo.tessellationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        configInfo.tessellationInfo.patchControlPoints = 3;
        configInfo.tessellationInfo.flags = 0;
        configInfo.tessellationInfo.pNext = nullptr;
    }

    void GraphicsPipeline::bind(VkCommandBuffer commandBuffer) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    }
}
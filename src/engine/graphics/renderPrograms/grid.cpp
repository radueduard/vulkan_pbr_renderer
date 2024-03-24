//
// Created by radue on 1/23/2024.
//

#include "grid.hpp"
#include "../../../log.hpp"

#include <stdexcept>
#include <numeric>

namespace ve {
    Grid::Grid(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout): device{device} {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);

        gridModels.emplace_back(Mesh::square(device, 1, {0, -1, 0}));
        gridModels.emplace_back(Mesh::square(device, 1, {0, 0, 1}));

        gridBuffer = std::make_unique<Buffer>(
                device,
                sizeof(int) * 2,
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        gridBuffer->map();
    }

    Grid::~Grid() {
        vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
    }

    void Grid::renderGrid(const FrameInfo& frameInfo) {
        pipeline->bind(frameInfo.graphicsCommandBuffer);

        gridBuffer->writeToBuffer(&size, sizeof(int));
        gridBuffer->writeToBuffer(&tesselation, sizeof(int), sizeof(int));
        gridBuffer->flush();

        vkCmdBindDescriptorSets(
                frameInfo.graphicsCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                0,
                1,
                &frameInfo.globalDescriptorSet,
                0,
                nullptr);

        auto bufferInfo = gridBuffer->descriptorInfoForIndex(0);

        VkDescriptorSet gridDescriptorSet;
        DescriptorWriter(*programLayout, frameInfo.frameDescriptorPool)
                .writeBuffer(0, &bufferInfo)
                .build(gridDescriptorSet);

        vkCmdBindDescriptorSets(
                frameInfo.graphicsCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                1,
                1,
                &gridDescriptorSet,
                0,
                nullptr);

        for (const auto& model : gridModels) {
            model->bind(frameInfo.graphicsCommandBuffer);
            model->draw(frameInfo.graphicsCommandBuffer);
        }
    }

    void Grid::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        programLayout = DescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT |
                        VK_SHADER_STAGE_VERTEX_BIT |
                        VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
            .build();

        const std::vector layouts = {
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

    void Grid::createPipeline(VkRenderPass renderPass) {
        if (pipelineLayout == nullptr) {
            Log::error("Cannot create pipeline before pipeline layout");
            throw std::runtime_error("");
        }

        ShaderFiles shaderFiles {};
        shaderFiles.vertFile = "shaders/compiled/grid.vert.spv";
        shaderFiles.tescFile = "shaders/compiled/grid.tesc.spv";
        shaderFiles.teseFile = "shaders/compiled/grid.tese.spv";
        shaderFiles.fragFile = "shaders/compiled/grid.frag.spv";

        GraphicsPipelineConfigInfo pipelineConfig {};
        GraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
        pipelineConfig.rasterizationInfo.lineWidth = 1.0f;
        pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
        pipelineConfig.tessellationInfo.patchControlPoints = 4;
        pipelineConfig.depthStencilInfo.depthTestEnable = VK_TRUE;

        pipeline = std::make_unique<GraphicsPipeline>(device,shaderFiles,pipelineConfig);
    }
}
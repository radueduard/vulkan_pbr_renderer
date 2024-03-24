//
// Created by radue on 1/30/2024.
//

#pragma once


#include <memory>

#include "../renderObject.hpp"
#include "../../../engine/device.hpp"
#include "../../../engine/graphics/graphicsPipeline.hpp"
#include "../../../engine/memory/descriptors.hpp"
#include "../../../engine/graphics/Mesh.hpp"
#include "../../../engine/renderer.hpp"
#include "../../../engine/graphics/material.hpp"

class SceneRenderProgram {
public:
	explicit SceneRenderProgram(ve::Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SceneRenderProgram();

    SceneRenderProgram(const SceneRenderProgram &) = delete;
    SceneRenderProgram &operator=(const SceneRenderProgram &) = delete;

    void renderScene(const ve::FrameInfo& frameInfo) const;

    void addRenderTargets(std::vector<std::unique_ptr<ve::RenderObject>>);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    ve::Device &device;
    std::unique_ptr<ve::GraphicsPipeline> pipeline;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    std::unique_ptr<ve::DescriptorSetLayout> objectSetLayout;
    std::unique_ptr<ve::DescriptorSetLayout> materialSetLayout;

    std::vector<std::unique_ptr<ve::RenderObject>> renderTargets;
};

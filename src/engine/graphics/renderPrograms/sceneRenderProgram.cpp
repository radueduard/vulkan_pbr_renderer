//
// Created by radue on 1/30/2024.
//

#include "sceneRenderProgram.hpp"

#include "../../renderer.hpp"
#include "../../../log.hpp"

SceneRenderProgram::SceneRenderProgram(ve::Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device)
{
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

SceneRenderProgram::~SceneRenderProgram()
{
	vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

void SceneRenderProgram::createPipelineLayout(const VkDescriptorSetLayout globalSetLayout)
{
    objectSetLayout = ve::DescriptorSetLayout::Builder(device)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
        .build();

    materialSetLayout = ve::DescriptorSetLayout::Builder(device)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .addBinding(2, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
        .addBinding(3, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
        .addBinding(4, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
        .addBinding(5, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();

	const std::vector layouts = {
    	globalSetLayout,
        objectSetLayout->getDescriptorSetLayout(),
        materialSetLayout->getDescriptorSetLayout(),
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

void SceneRenderProgram::createPipeline(VkRenderPass renderPass)
{
    if (pipelineLayout == nullptr) {
        Log::error("Cannot create pipeline before pipeline layout");
        throw std::runtime_error("");
    }

    ve::ShaderFiles shaderFiles {};
    shaderFiles.vertFile = "shaders/compiled/PBR.vert.spv";
    shaderFiles.fragFile = "shaders/compiled/PBR.frag.spv";

    ve::GraphicsPipelineConfigInfo pipelineConfig {};
    ve::GraphicsPipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    // pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;

    pipeline = std::make_unique<ve::GraphicsPipeline>(device, shaderFiles, pipelineConfig);
}

void SceneRenderProgram::renderScene(const ve::FrameInfo& frameInfo) const
{
    pipeline->bind(frameInfo.graphicsCommandBuffer);

    vkCmdBindDescriptorSets(
        frameInfo.graphicsCommandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &frameInfo.globalDescriptorSet,
        0,
        nullptr);

	for (const auto& renderTarget : renderTargets)
	{
        renderTarget->updateBuffers(*objectSetLayout, frameInfo);
        vkCmdBindDescriptorSets(
            frameInfo.graphicsCommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            1,
            1,
            &renderTarget->descriptorSet,
            0,
            nullptr);

        for (const auto& mesh : renderTarget->meshes) 
        {
            const auto& material = mesh->getMaterial();
            material->updateBuffers(*materialSetLayout, frameInfo);
            vkCmdBindDescriptorSets(
                frameInfo.graphicsCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                2,
                1,
                &mesh->getMaterial()->descriptorSet,
                0,
                nullptr);
            
            mesh->bind(frameInfo.graphicsCommandBuffer);
            mesh->draw(frameInfo.graphicsCommandBuffer);
        }
	}
    return;
}

void SceneRenderProgram::addRenderTargets(std::vector<std::unique_ptr<ve::RenderObject>> models)
{
	std::for_each(models.begin(), models.end(), [this](std::unique_ptr<ve::RenderObject>& model)
	{
		renderTargets.emplace_back(std::move(model));
	});
}

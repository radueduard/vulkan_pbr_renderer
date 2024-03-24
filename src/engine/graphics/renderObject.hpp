#pragma once
#include <memory>

#include "material.hpp"
#include "mesh.hpp"
#include "../renderer.hpp"
#include "../memory/descriptors.hpp"

namespace ve
{
	class RenderObject
	{
	public:
		explicit RenderObject(Device& device) {
			objectUniformBuffer = std::make_unique<Buffer>(
				device,
				sizeof(glm::mat4),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			objectUniformBuffer->map();
		}

		glm::mat4 localModelMatrix = glm::mat4(1);
		std::vector<std::shared_ptr<ve::Mesh>> meshes {};

		std::unique_ptr<ve::Buffer> objectUniformBuffer;
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

		void updateBuffers(DescriptorSetLayout& programLayout, const FrameInfo& frameInfo)
		{
			objectUniformBuffer->writeToBuffer(&localModelMatrix, sizeof(glm::mat4));
			objectUniformBuffer->flush();

			auto bufferInfo = objectUniformBuffer->descriptorInfoForIndex(0);

			DescriptorWriter(programLayout, frameInfo.frameDescriptorPool)
				.writeBuffer(0, &bufferInfo)
				.build(descriptorSet);
		}
	};
}

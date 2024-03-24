//
// Created by radue on 1/30/2024.
//

#pragma once
#include <string>
#include <glm/glm.hpp>

#include "image.hpp"

#include  "../memory/buffer.hpp"
#include "../renderer.hpp"
#include "../memory/descriptors.hpp"

namespace ve {

	struct Material {
        Material(Device& device)
        {
            materialUniformBuffer = std::make_unique<Buffer>(
                device,
                sizeof(Parameters),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            materialUniformBuffer->map();
        }

        std::string name;
        std::string id;

        struct Parameters
		{
            float alphaCutoff;
            bool doubleSided;
            glm::vec3 emissiveFactor;
            glm::vec4 baseColorFactor;
            float roughnessFactor;
            float metallicFactor;
        } parameters{} ;

        struct Samplers
        {
            std::shared_ptr<Image> emissiveTexture;
            std::shared_ptr<Image> normalTexture;
            std::shared_ptr<Image> occlusionTexture;
            std::shared_ptr<Image> baseColorTexture;
            std::shared_ptr<Image> metallicRoughnessTexture;
        } samplers {};


        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        std::unique_ptr<ve::Buffer> materialUniformBuffer = nullptr;

        void updateBuffers(DescriptorSetLayout& setLayout, const FrameInfo& frameInfo)
        {
            materialUniformBuffer->writeToBuffer(&parameters, sizeof(Parameters));
            materialUniformBuffer->flush();

            auto bufferInfo = materialUniformBuffer->descriptorInfoForIndex(0);

            VkDescriptorImageInfo emissiveTextureInfo;
            if (samplers.emissiveTexture != nullptr)
                emissiveTextureInfo = samplers.emissiveTexture->getImageInfo();
			else
                emissiveTextureInfo = Image::getDefaultImage()->getImageInfo();

        	VkDescriptorImageInfo normalTextureInfo;
        	if (samplers.normalTexture != nullptr)
				normalTextureInfo = samplers.normalTexture->getImageInfo();
			else
				normalTextureInfo = Image::getDefaultImage()->getImageInfo();

        	VkDescriptorImageInfo occlusionTextureInfo;
        	if (samplers.occlusionTexture != nullptr)
                occlusionTextureInfo = samplers.occlusionTexture->getImageInfo();
            else
                occlusionTextureInfo = Image::getDefaultImage()->getImageInfo();

            VkDescriptorImageInfo baseColorTextureInfo;
            if (samplers.baseColorTexture != nullptr)
                baseColorTextureInfo = samplers.baseColorTexture->getImageInfo();
            else
                baseColorTextureInfo = Image::getDefaultImage()->getImageInfo();

            VkDescriptorImageInfo metallicRoughnessTextureInfo;
            if (samplers.metallicRoughnessTexture != nullptr)
                metallicRoughnessTextureInfo = samplers.metallicRoughnessTexture->getImageInfo();
			else
				metallicRoughnessTextureInfo = Image::getDefaultImage()->getImageInfo();

            DescriptorWriter(setLayout, frameInfo.frameDescriptorPool)
                .writeBuffer(0, &bufferInfo)
        		.writeImage(1, &emissiveTextureInfo)
        		.writeImage(2, &normalTextureInfo)
				.writeImage(3, &occlusionTextureInfo)
        		.writeImage(4, &baseColorTextureInfo)
                .writeImage(5, &metallicRoughnessTextureInfo)
                .build(descriptorSet);
        }
    };
}

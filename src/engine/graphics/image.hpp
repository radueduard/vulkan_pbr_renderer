#pragma once

#include "../../engine/device.hpp"

// vendor
#include <vulkan/vulkan.h>

// std
#include <memory>
#include <string>

namespace ve {
    class Image {
    public:
        Image(Device &device, const std::string &textureFilepath);
        Image(Device& device, const unsigned char* pixels, int width, int height);
        Image(
                Device &device,
                VkFormat format,
                VkExtent3D extent,
                VkImageUsageFlags usage,
                VkSampleCountFlagBits sampleCount);
        ~Image();

        Image(const Image &) = delete;
        Image &operator=(const Image &) = delete;

        VkImageView imageView() const { return mTextureImageView; }
        VkSampler sampler() const { return mTextureSampler; }
        VkImage getImage() const { return mTextureImage; }
        VkImageView getImageView() const { return mTextureImageView; }
        VkDescriptorImageInfo getImageInfo() const { return mDescriptor; }
        VkImageLayout getImageLayout() const { return mTextureLayout; }
        VkExtent3D getExtent() const { return mExtent; }
        VkFormat getFormat() const { return mFormat; }

        void updateDescriptor();
        void transitionLayout(
                VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

        static std::shared_ptr<Image> createTextureFromFile(Device &device, const std::string &filepath);
        static std::shared_ptr<Image> createTextureFromMemory(Device& device, unsigned char* pixels, int width, int height);

    	static std::shared_ptr<Image> getDefaultImage() {
            return defaultImage;
        }

        static void loadDefaultImage(Device& device)
        {
            size_t width = 128, height = 128;
            auto pixels = new unsigned char[width * height * 4];
            memset(pixels, 255, width * height * 4);
            defaultImage = std::make_shared<Image>(device, pixels, width, height);
            delete[] pixels;
        }

    private:
        static std::shared_ptr<Image> defaultImage;

        void createTextureImage(const unsigned char* pixels, size_t width, size_t height);
        void createTextureImageView(VkImageViewType viewType);
        void createTextureSampler();

        VkDescriptorImageInfo mDescriptor{};

        Device &mDevice;
        VkImage mTextureImage = nullptr;
        VkDeviceMemory mTextureImageMemory = nullptr;
        VkImageView mTextureImageView = nullptr;
        VkSampler mTextureSampler = nullptr;
        VkFormat mFormat;
        VkImageLayout mTextureLayout;
        uint32_t mMipLevels{1};
        uint32_t mLayerCount{1};
        VkExtent3D mExtent{};
    };

}  // namespace lve
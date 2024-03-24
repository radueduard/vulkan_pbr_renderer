//
// Created by radue on 1/23/2024.
//

#pragma once

#include <memory>

#include "../../../engine/device.hpp"
#include "../../../engine/graphics/graphicsPipeline.hpp"
#include "../../../engine/memory/descriptors.hpp"
#include "../../../engine/graphics/Mesh.hpp"
#include "../../../engine/renderer.hpp"

namespace ve {
    class Grid {
    public:
        Grid(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~Grid();

        Grid(const Grid &) = delete;
        Grid &operator=(const Grid &) = delete;

        void renderGrid(const FrameInfo&);

        int size = 64;
        int tesselation = 64;

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        Device &device;
        std::unique_ptr<GraphicsPipeline> pipeline;
        VkPipelineLayout pipelineLayout;

        std::unique_ptr<DescriptorSetLayout> programLayout;

        std::vector<std::shared_ptr<Mesh>> gridModels;
        std::unique_ptr<Buffer> gridBuffer;
    };
}

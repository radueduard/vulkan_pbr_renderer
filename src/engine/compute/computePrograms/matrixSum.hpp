//
// Created by radue on 1/24/2024.
//

#pragma once


#include "../../../engine/device.hpp"
#include "../../../engine/compute/computePipeline.hpp"
#include "../../../engine/memory/descriptors.hpp"
#include "../../../engine/memory/buffer.hpp"
#include "../../../engine/renderer.hpp"

class MatrixSum {

public:
    MatrixSum(ve::Device& device, VkDescriptorSetLayout globalSetLayout);
    ~MatrixSum();

    MatrixSum(const MatrixSum&) = delete;
    MatrixSum& operator=(const MatrixSum&) = delete;

    void computeMatrixSum(const ve::FrameInfo& frameInfo);

    glm::mat4 getResult();

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline();

    ve::Device& device;
    std::unique_ptr<ve::ComputePipeline> pipeline;
    VkPipelineLayout pipelineLayout;

    std::unique_ptr<ve::DescriptorSetLayout> programLayout;

    std::unique_ptr<ve::Buffer> matrixBuffer;
    std::unique_ptr<ve::Buffer> matrixSumBuffer;

};

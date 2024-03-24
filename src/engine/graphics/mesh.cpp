//
// Created by radue on 1/23/2024.
//

#include "mesh.hpp"

#include <cassert>

#include <glm/ext/matrix_transform.hpp>

namespace ve {

    Mesh::Mesh(Device &device, const Mesh::Builder& builder) : device(device) {
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    Mesh::~Mesh() = default;

    void Mesh::bind(VkCommandBuffer commandBuffer) const
    {
	    const VkBuffer buffers[] = {vertexBuffer->getBuffer()};
	    constexpr VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if (hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
        }
    }

    void Mesh::draw(VkCommandBuffer commandBuffer) const {
        if (hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        } else {
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
    }

    void Mesh::createVertexBuffers(const std::vector<Vertex> &vertices) {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        const VkDeviceSize bufferSize = sizeof(Vertex) * vertexCount;
        uint32_t vertexSize = sizeof(Vertex);

        Buffer stagingBuffer(
                device,
                vertexSize,
                vertexCount,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void *)vertices.data());

        vertexBuffer = std::make_unique<Buffer>(
                device,
                vertexSize,
                vertexCount,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
    }

    void Mesh::createIndexBuffers(const std::vector<uint16_t> &indices) {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;
        if (!hasIndexBuffer) return;
        VkDeviceSize bufferSize = sizeof(uint16_t) * indexCount;
        uint32_t indexSize = sizeof(uint16_t);

        Buffer stagingBuffer(
                device,
                indexSize,
                indexCount,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*)indices.data());

        indexBuffer = std::make_unique<Buffer>(
                device,
                indexSize,
                indexCount,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
    }

    std::vector<VkVertexInputBindingDescription> Mesh::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Mesh::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(6);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, tangent);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, tex_coord_0);

        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(Vertex, tex_coord_1);

        attributeDescriptions[5].binding = 0;
        attributeDescriptions[5].location = 5;
        attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[5].offset = offsetof(Vertex, color_0);

        return attributeDescriptions;
    }

    std::shared_ptr<Mesh> Mesh::Builder::build(Device &device) {
        return std::make_shared<Mesh>(device, *this);
    }

    std::unique_ptr<Mesh> Mesh::square(Device& device, int size, glm::vec3 up = {0, 1, 0}) {
        std::vector<Vertex> vertices = {};
        {
            Vertex v;
            v.position = { -size, 0.0f, -size };
            v.normal = { .0f,-1.f,.0f };
            v.tex_coord_0 = { 1, -1 };
            vertices.push_back(v);
        }

        {
            Vertex v;
            v.position = { size, 0.0f, -size };
            v.normal = { .0f,-1.f,.0f };
            v.tex_coord_0 = { 1, -1 };
            vertices.push_back(v);
        }

        {
            Vertex v;
            v.position = { size, 0.0f, size };
            v.normal = { .0f,-1.f,.0f };
            v.tex_coord_0 = { 1, 1 };
            vertices.push_back(v);
        }

        {
            Vertex v;
            v.position = { -size, 0.0f, size };
            v.normal = { .0f,-1.f,.0f };
            v.tex_coord_0 = { -1, 1 };
            vertices.push_back(v);
        }

        const glm::vec3 rotationDir = glm::cross({0, -1, 0}, up);

        if (const float rotationAngle = glm::acos(glm::dot({0, -1, 0}, up)); rotationAngle > 0) {
            for (auto &vertex: vertices) {
                vertex.position = glm::mat3(glm::rotate(glm::mat4(1), rotationAngle, rotationDir)) * vertex.position;
            }
        }

        const std::vector<uint16_t> indices = {
                0, 1, 2, 3,
                1, 2, 3, 0,
        };

        Builder builder {};
        builder.vertices = vertices;
        builder.indices = indices;
        return std::make_unique<Mesh>(device, builder);
    }

    std::shared_ptr<Mesh> Mesh::sphere(Device& device, int size, int segmentCount) {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;

        const float horizontalIncrement = glm::two_pi<float>() / static_cast<float>(segmentCount);
        const float verticalIncrement = glm::pi<float>() / static_cast<float>(segmentCount);

        for (int i = 0; i <= segmentCount; ++i) {
	        const float verticalAngle = verticalIncrement * i;
            for (int j = 0; j <= segmentCount; ++j) {
	            const float horizontalAngle = horizontalIncrement * j;
                Vertex vertex{};
                vertex.position = {
                        size * glm::sin(verticalAngle) * glm::cos(horizontalAngle),
                        size * glm::cos(verticalAngle),
                        size * glm::sin(verticalAngle) * glm::sin(horizontalAngle)
                };
                vertex.normal = glm::normalize(vertex.position);
                vertex.tex_coord_0 = {horizontalAngle / glm::two_pi<float>(), -verticalAngle / glm::pi<float>()};
                vertices.push_back(vertex);
            }
        }

        for (int i = 0; i < segmentCount; ++i) {
            for (int j = 0; j < segmentCount; ++j) {
	            const int topLeft = i * (segmentCount + 1) + j;
	            const int topRight = topLeft + 1;
	            const int bottomLeft = topLeft + segmentCount + 1;
	            const int bottomRight = bottomLeft + 1;
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        Builder builder{};
        builder.vertices = vertices;
        builder.indices = indices;
        return std::make_shared<Mesh>(device, builder);
    }
}
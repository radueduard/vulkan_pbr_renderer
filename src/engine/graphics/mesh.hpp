//
// Created by radue on 1/23/2024.
//

#pragma once

#include "../../engine/device.hpp"
#include "../../engine/memory/buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "material.hpp"

namespace ve {
    class Mesh {
    public:
        struct Vertex {
            glm::vec3 position{};
            glm::vec3 normal{};
            glm::vec4 tangent{};
            glm::vec2 tex_coord_0{};
            glm::vec2 tex_coord_1{};
            glm::vec4 color_0 {};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint16_t> indices{};

            std::shared_ptr<Mesh> build(Device& device);
        };

        Mesh(Device& device, const Builder& builder);
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        ~Mesh();

        void bind(VkCommandBuffer commandBuffer) const;
        void draw(VkCommandBuffer commandBuffer) const;

    private:
        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint16_t>& indices);

        Device& device;

        std::unique_ptr<Buffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;

        std::unique_ptr<Buffer> indexBuffer;
        uint32_t indexCount;
        std::shared_ptr<Material> material = nullptr;

    public:
        void setMaterial(std::shared_ptr<Material> material) { this->material = material; }
        std::shared_ptr<Material> getMaterial() { return material; }

        static std::unique_ptr<Mesh> square(Device& device, int size, glm::vec3 up);
        static std::shared_ptr<Mesh> sphere(Device& device, int size, int segmentCount);
    };
}

//
// Created by radue on 1/23/2024.
//

#pragma once

#include "renderer.hpp"
#include "../camera/camera.hpp"

namespace ve {
    class Scene : public InputController {
    private:
        Window& window;
    protected:
        Device device;
        Renderer renderer;
        explicit Scene(Window & window);
        ~Scene() = default;
        static Scene* instance;
    private:

        void initImGui();
        void renderImGui(const VkCommandBuffer& commandBuffer) const;

    public:
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        void run();

    protected:
        virtual void init() {}
        virtual void update(float deltaTime) {}
        virtual void render(FrameInfo& frameInfo) {}

        Camera camera;
        std::unique_ptr<DescriptorSetLayout> globalSetLayout;
        std::vector<std::unique_ptr<DescriptorPool>> framePools;

    private:
        float frameTime = 0.0f;

        glm::mat4 result {0};
    };
} // ve

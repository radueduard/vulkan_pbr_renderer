//
// Created by radue on 1/30/2024.
//

#pragma once

#include "../engine/scene.hpp"
#include "../engine/graphics/renderPrograms/sceneRenderProgram.hpp"

class Sponza : public ve::Scene
{
public:
    explicit Sponza(ve::Window &window);
    ~Sponza() = default;

    Sponza(const Sponza &) = delete;
    Sponza &operator=(const Sponza &) = delete;

    static ve::Scene &getInstance(ve::Window &window);

    void init() override;
    void update(float deltaTime) override;
    void render(ve::FrameInfo &frameInfo) override;

private:
    std::unique_ptr<SceneRenderProgram> srp;
};

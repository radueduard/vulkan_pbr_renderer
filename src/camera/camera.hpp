//
// Created by radue on 1/23/2024.
//

#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include "../window/inputController.hpp"
#include "../engine/compute/computePrograms/rayDirections.hpp"

class Camera : public ve::InputController
{
public:
    struct CameraBufferData {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 inverseView;
        glm::mat4 inverseProjection;
    };

    CameraBufferData getCameraBufferData() const {
        return { view, projection, inverseView, inverseProjection };
    }

    Camera(float verticalFOV, float nearClip, float farClip);

    bool update(float deltaTime);
    void resize(uint32_t width, uint32_t height);

    const glm::mat4& getProjection() const { return projection; }
    const glm::mat4& getInverseProjection() const { return inverseProjection; }
    const glm::mat4& getView() const { return view; }
    const glm::mat4& getInverseView() const { return inverseView; }

    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getDirection() const { return forwardDirection; }

    float getRotationSpeed();
private:
    void recalculateProjection();
    void recalculateView();

protected:
    void onInputUpdate(float deltaTime, int mods) override;
    void onMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
    void onKeyPress(int key, int mods) override;
    void onMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
    void onMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;

    void onScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;


private:
    glm::mat4 projection{1.0f };
    glm::mat4 view{1.0f };
    glm::mat4 inverseProjection{1.0f };
    glm::mat4 inverseView{1.0f };

    float verticalFOV = 45.0f;
    float nearClip = 0.1f;
    float farClip = 100.0f;

    glm::vec3 position{0.0f, 0.0f, 0.0f };
    glm::vec3 forwardDirection{0.0f, 0.0f, 0.0f };

    glm::uvec2 viewportSize{0, 0};

    bool moved = true;
};

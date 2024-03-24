//
// Created by radue on 1/23/2024.
//

#include "camera.hpp"
#include "../window/window.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


Camera::Camera(float verticalFOV, float nearClip, float farClip)
        : verticalFOV(verticalFOV), nearClip(nearClip), farClip(farClip)
{
    forwardDirection = glm::vec3(0, 0, -1);
    position = glm::vec3(0, 0, 0);
}

bool Camera::update(float deltaTime)
{
    if (moved) {
        recalculateView();
        moved = false;
        return true;
    }

    return false;
}

void Camera::resize(uint32_t width, uint32_t height)
{
    if (width == viewportSize.x && height == viewportSize.y)
        return;

    viewportSize = { width, height };

    recalculateProjection();
}

float Camera::getRotationSpeed()
{
    return 5.0f;
}

void Camera::recalculateProjection()
{
    projection = glm::perspectiveFov(glm::radians(verticalFOV), (float)viewportSize.x, (float)viewportSize.y, nearClip, farClip);
    inverseProjection = glm::inverse(projection);
}

void Camera::recalculateView()
{
    view = glm::lookAt(position, position + forwardDirection, glm::vec3(0, -1, 0));
    inverseView = glm::inverse(view);
}

//void Camera::recalculateRayDirections()
//{
//    rayDirections.resize(viewportSize.x * viewportSize.y);
//
//    for (uint32_t y = 0; y < viewportSize.y; y++)
//    {
//        for (uint32_t x = 0; x < viewportSize.x; x++)
//        {
//            glm::vec2 coord = { (float)x / (float)viewportSize.x, (float)y / (float)viewportSize.y };
//            coord = coord * 2.0f - 1.0f; // -1 -> 1
//
//            glm::vec4 target = inverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
//            glm::vec3 rayDirection = glm::vec3(inverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
//            rayDirections[x + y * viewportSize.x] = rayDirection;
//        }
//    }
//}

void Camera::onKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_ESCAPE) {
        ve::Window::getInstance().close();
	}
}

void Camera::onInputUpdate(float deltaTime, int mods) {
    constexpr float speed = 5.f;

    ve::Window& window = ve::Window::getInstance();

    if (!window.mouseHold(GLFW_MOUSE_BUTTON_RIGHT)) return;

    constexpr glm::vec3 upDirection(0.0f, -1.0f, 0.0f);
    glm::vec3 rightDirection = glm::cross(forwardDirection, upDirection);

    if (window.keyHold(GLFW_KEY_W)) {
        position -= forwardDirection * speed * deltaTime;
        moved = true;
    }

    if (window.keyHold(GLFW_KEY_S)) {
        position += forwardDirection * speed * deltaTime;
        moved = true;
    }

    if (window.keyHold(GLFW_KEY_A)) {
        position += rightDirection * speed * deltaTime;
        moved = true;
    }

    if (window.keyHold(GLFW_KEY_D)) {
        position -= rightDirection * speed * deltaTime;
        moved = true;
    }

    if (window.keyHold(GLFW_KEY_Q)) {
        position -= upDirection * speed * deltaTime;
        moved = true;
    }

    if (window.keyHold(GLFW_KEY_E)) {
        position += upDirection * speed * deltaTime;
        moved = true;
    }
}

void Camera::onMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    ve::Window& window = ve::Window::getInstance();

    if (!window.mouseHold(GLFW_MOUSE_BUTTON_RIGHT)) return;

    constexpr glm::vec3 upDirection(0.0f, -1.0f, 0.0f);
    glm::vec3 rightDirection = glm::cross(forwardDirection, upDirection);


    if (deltaX != 0 || deltaY != 0)
    {
        float pitchDelta = ((float)deltaY / (float)viewportSize.y) * getRotationSpeed();
        float yawDelta = ((float)deltaX / (float) viewportSize.x) * getRotationSpeed();

        glm::quat q = glm::normalize(glm::cross(
                glm::angleAxis(pitchDelta, rightDirection),
                glm::angleAxis(-yawDelta, upDirection)));
        forwardDirection = glm::rotate(q, forwardDirection);
        moved = true;
    }
}

void Camera::onMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    ve::Window& window = ve::Window::getInstance();

    if ((button | 1 << GLFW_MOUSE_BUTTON_RIGHT) == button) {
        window.disablePointer();
    }
}

void Camera::onMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
    ve::Window& window = ve::Window::getInstance();

    if ((button | 1 << GLFW_MOUSE_BUTTON_RIGHT) == button) {
        window.enablePointer();
    }
}

void Camera::onScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
    verticalFOV -= (float) offsetY;
    verticalFOV = glm::clamp(verticalFOV, 45.0f, 120.0f);
    recalculateProjection();
}

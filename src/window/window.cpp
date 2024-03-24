//
// Created by radue on 1/23/2024.
//

#include <stdexcept>

#include "../log.hpp"
#include "window.hpp"
#include "windowCallbacks.hpp"

namespace ve {
    Window *Window::instance = nullptr;

    Window::Window(const glm::ivec2 size, const std::string &&name)
            : size(size), name(name), window(nullptr), currentTime(0), deltaTime(0) {
        initWindow();
    }

    Window::~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        window = glfwCreateWindow((int)size.x, (int)size.y, name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        setWindowCallbacks();
    }

    float Window::computeDeltaTime() {
        auto newTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - newTime;
        currentTime = newTime;
        return deltaTime;
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            Log::error("Failed to create window surface!");
            throw std::runtime_error("");
        }
    }

    void Window::updateInputs() {
        if (inputStates.mouseMove != std::nullopt) {
            auto [x, y] = inputStates.mousePosition;
            auto [dx, dy] = inputStates.mouseMove.value();

            for (auto inputController : inputControllers) {
                inputController->onMouseMove(x, y, dx, dy);
            }

            inputStates.mouseMove = std::nullopt;
        }

        auto pressEvent = inputStates.mouseButtons.mouseButtonAction &
                          inputStates.mouseButtons.mouseButtonStates;

        if (pressEvent != 0) {
            auto [x, y] = inputStates.mousePosition;
            auto keyMods = inputStates.keyMods;

            for (auto inputController : inputControllers) {
                inputController->onMouseBtnPress(x, y, pressEvent, keyMods);
            }
        }

        auto releaseEvent = inputStates.mouseButtons.mouseButtonAction &
                            ~inputStates.mouseButtons.mouseButtonStates;

        if (releaseEvent != 0) {
            auto [x, y] = inputStates.mousePosition;
            auto keyMods = inputStates.keyMods;

            for (auto inputController : inputControllers) {
                inputController->onMouseBtnRelease(x, y, releaseEvent, keyMods);
            }
        }

        inputStates.mouseButtons.mouseButtonAction = 0;

        if (inputStates.mouseScroll != std::nullopt) {
            auto [x, y] = inputStates.mousePosition;
            auto [dx, dy] = inputStates.mouseScroll.value();

            for (auto inputController : inputControllers) {
                inputController->onScroll(x, y, dx, dy);
            }

            inputStates.mouseScroll = std::nullopt;
        }

        while (!inputStates.keyEvents.empty()) {
            int key = inputStates.keyEvents.front();
            inputStates.keyEvents.pop();

            if (inputStates.keyStates[key]) {
                for (auto inputController : inputControllers) {
                    inputController->onKeyPress(key, inputStates.keyMods);
                }
            } else {
                for (auto inputController : inputControllers) {
                    inputController->onKeyRelease(key, inputStates.keyMods);
                }
            }
        }

        for (auto inputController : inputControllers) {
            inputController->onInputUpdate(deltaTime, inputStates.keyMods);
        }
    }

    void Window::addInputController(InputController *inputController) {
        inputControllers.push_back(inputController);
    }

    void Window::enablePointer() {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void Window::disablePointer() {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    bool Window::mouseHold(int button) {
        return (inputStates.mouseButtons.mouseButtonStates & (1 << button)) != 0;
    }

    bool Window::keyHold(int key) {
        return inputStates.keyStates[key];
    }

    int Window::getSpecialKeyState() const {
        return inputStates.keyMods;
    }

    void Window::keyCallback(int key, int scancode, int action, int mods) {
        inputStates.keyMods = mods;
        if (inputStates.keyStates[key] == (action != GLFW_RELEASE)) return;
        inputStates.keyStates[key] = action != GLFW_RELEASE;
        inputStates.keyEvents.push(key);
    }

    void Window::mouseMoveCallback(double mouseX, double mouseY) {
        if (inputStates.mouseMove == std::nullopt)
            inputStates.mouseMove = MouseMove{0, 0};
        auto& [x, y] = inputStates.mousePosition;
        auto& [dx, dy] = inputStates.mouseMove.value();
        dx = static_cast<int>(mouseX) - x;
        dy = static_cast<int>(mouseY) - y;
        x = static_cast<int>(mouseX);
        y = static_cast<int>(mouseY);
    }

    void Window::mouseButtonCallback(int button, int action, int mods) {
        inputStates.keyMods = mods;
        inputStates.mouseButtons.mouseButtonAction |= (1 << button);
        if (action == GLFW_PRESS) {
            inputStates.mouseButtons.mouseButtonStates |= (1 << button);
        } else {
            inputStates.mouseButtons.mouseButtonStates &= ~(1 << button);
        }
    }

    void Window::scrollCallback(double offsetX, double offsetY) {
        inputStates.mouseScroll = MouseScroll{static_cast<int>(offsetX), static_cast<int>(offsetY)};
    }

    void Window::setWindowCallbacks() const {
        glfwSetKeyCallback(window, WindowCallbacks::keyCallback);
        glfwSetCursorPosCallback(window, WindowCallbacks::mouseMoveCallback);
        glfwSetMouseButtonCallback(window, WindowCallbacks::mouseButtonCallback);
        glfwSetScrollCallback(window, WindowCallbacks::scrollCallback);
    }

    void Window::close() const {
    	glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
} // ve
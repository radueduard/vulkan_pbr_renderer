#pragma once

// std
#include <string>
#include <optional>
#include <queue>
#include <chrono>

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// vulkan
#include <vulkan/vulkan.h>

// glm
#define GLM_FORCE_RADIANSs
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>

#include "inputController.hpp"

namespace ve
{
    class Window
    {
        friend class WindowCallbacks;
        friend class InputController;

    private:
        GLFWwindow *window;
        std::vector<InputController *> inputControllers;

        std::string name;
        glm::uvec2 size;

        float currentTime;
        float deltaTime;

        void initWindow();

        explicit Window(glm::ivec2 size, const std::string &&name = "Vulkan Engine");
        ~Window();

        static Window *instance;

    public:
        static Window &getInstance(const glm::ivec2 size = {1920, 1080}, std::string &&name = "Vulkan Engine")
        {
            if (!instance)
            {
                instance = new Window(size, std::move(name));
            }
            return *instance;
        }

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        bool shouldClose() const { return glfwWindowShouldClose(window); }
        void pollEvents() const { glfwPollEvents(); }
        GLFWwindow *getWindowHandle() const { return window; }
        VkExtent2D getExtent() const { return {size.x, size.y}; }
        float getDeltaTime() const { return deltaTime; }

        void createWindowSurface(VkInstance, VkSurfaceKHR *);
        void updateInputs();

        void addInputController(InputController *inputController);

        void enablePointer();
        void disablePointer();

        bool mouseHold(int button);
        bool keyHold(int key);
        int getSpecialKeyState() const;

    private:
        void keyCallback(int key, int scancode, int action, int mods);
        void mouseMoveCallback(double mouseX, double mouseY);
        void mouseButtonCallback(int button, int action, int mods);
        void scrollCallback(double offsetX, double offsetY);

        struct MousePosition
        {
            int x;
            int y;
        };

        struct MouseMove
        {
            int deltaX;
            int deltaY;
        };

        struct MouseButtons
        {
            int mouseButtonAction;
            int mouseButtonStates;
        };

        struct MouseScroll
        {
            int scrollDeltaX;
            int scrollDeltaY;
        };

        struct InputStates
        {
            MousePosition mousePosition{};
            std::optional<MouseMove> mouseMove = std::nullopt;
            MouseButtons mouseButtons{};
            std::optional<MouseScroll> mouseScroll = std::nullopt;
            int keyMods = 0;
            std::queue<int> keyEvents = std::queue<int>();
            bool keyStates[384] = {false};
            bool keyScanCode[512] = {false};
        };

    public:
        InputStates inputStates;
        void setWindowCallbacks() const;
        void close() const;
        float computeDeltaTime();
    };
} // ve

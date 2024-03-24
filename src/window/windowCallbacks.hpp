//
// Created by radue on 1/23/2024.
//

#pragma once

#include <GLFW/glfw3.h>
#include "window.hpp"

namespace ve {
    class WindowCallbacks {
    public:
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
            app->keyCallback(key, scancode, action, mods);
        }

        static void mouseMoveCallback(GLFWwindow* window, double xPos, double yPos) {
            auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
            app->mouseMoveCallback(xPos, yPos);
        }

        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
            auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
            app->mouseButtonCallback(button, action, mods);
        }

        static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
            auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
            app->scrollCallback(xOffset, yOffset);
        }
    };
}



#pragma once

namespace ve {
    class InputController {
        friend class Window;

    public:
        InputController() = default;
        InputController(const InputController&) = delete;
        InputController& operator=(const InputController&) = delete;
        virtual ~InputController() = default;

    protected:
        virtual void onInputUpdate(float deltaTime, int mods) {}
        virtual void onKeyPress(int key, int mods) {}
        virtual void onKeyRelease(int key, int mods) {}
        virtual void onMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {}
        virtual void onMouseBtnPress(int mouseX, int mouseY, int button, int mods) {}
        virtual void onMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {}
        virtual void onScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}
    };
} // ve

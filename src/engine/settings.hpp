//
// Created by radue on 1/24/2024.
//

#pragma once

namespace ve {

    class Settings {
    public:
        bool VSYNC = true;

        static Settings* getInstance() {
            if (instance == nullptr) {
                instance = new Settings();
            }
            if (oldSettings == nullptr) {
                oldSettings = new Settings();
            }
            return instance;
        }

        Settings(const Settings &) = delete;
        Settings &operator=(const Settings &) = delete;

        static bool changed() {
            return instance->VSYNC != oldSettings->VSYNC;
        }

        static void update() {
            oldSettings->VSYNC = instance->VSYNC;
        }

    private:
        Settings() = default;
        ~Settings() = default;

        static Settings* instance;
        static Settings* oldSettings;
    };
} // ve

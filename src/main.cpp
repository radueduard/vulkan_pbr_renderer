#include "scenes/sponza.hpp"
#include "log.hpp"

int main()
{
    ve::Window &window = ve::Window::getInstance({1920, 1080});
    ve::Scene &scene = Sponza::getInstance(window);

    try
    {
        scene.run();
    }
    catch (const std::exception &e)
    {
        Log::error(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

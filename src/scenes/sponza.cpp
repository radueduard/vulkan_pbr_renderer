//
// Created by radue on 1/30/2024.
//

#include "sponza.hpp"

#include "../loader/gltfLoader.hpp"

ve::Scene &Sponza::getInstance(ve::Window &window) {
    if (instance == nullptr) {
        instance = new Sponza(window);
    }
    return *instance;
}

Sponza::Sponza(ve::Window& window) : Scene(window) {}

void Sponza::init()
{
    GLTFLoader sceneLoader(device, "Sponza/NewSponza_Main_glTF_002.gltf");

    sceneLoader.loadLights(device);

    srp = std::make_unique<SceneRenderProgram>(device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
	srp->addRenderTargets(sceneLoader.loadRenderTargets(device));
}

void Sponza::update(const float deltaTime) {}

void Sponza::render(ve::FrameInfo& frameInfo)
{
    srp->renderScene(frameInfo);
}

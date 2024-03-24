#pragma once

#include <GLTFSDK/Deserialize.h>

#include "../engine/graphics/material.hpp"
#include "../engine/graphics/mesh.hpp"
#include "../engine/graphics/image.hpp"
#include "../engine/graphics/texture.hpp"
#include "../engine/graphics/renderObject.hpp"
#include "../engine/graphics/light.hpp"

#define SIMDJSON_USING_LIBRARY
#include <simdjson.h>

class GLTFLoader
{
public:
	explicit GLTFLoader(ve::Device&, const std::string&);

	std::vector<std::unique_ptr<ve::RenderObject>> loadRenderTargets(ve::Device&) const;
	std::vector<std::unique_ptr<ve::Light>> loadLights(ve::Device&) const;

	std::unordered_map<std::string, std::vector<std::shared_ptr<ve::Mesh>>> meshes;
	std::unordered_map<std::string, std::shared_ptr<ve::Image>> images;
	std::unordered_map<std::string, std::vector<uint8_t>> buffers;
	std::unordered_map<std::string, std::shared_ptr<ve::Material>> materials;
	std::unordered_map<std::string, std::shared_ptr<ve::Texture>> textures;

private:
	void loadDocument(const std::string&);
	void loadBuffers();
	void loadMeshes(ve::Device&);
	void loadImages(ve::Device&);
	void loadTextures();
	void loadMaterials(ve::Device&);

	Microsoft::glTF::Document document;

};

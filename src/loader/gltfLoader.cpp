#include "gltfLoader.hpp"

#include <fstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <unordered_map>
#include <execution>
#include <stb_image.h>


GLTFLoader::GLTFLoader(ve::Device &device, const std::string &filepath)
{
    loadDocument(filepath);
    loadBuffers();
    loadImages(device);
    loadTextures();
    loadMaterials(device);
    loadMeshes(device);
}

void GLTFLoader::loadDocument(const std::string &filepath)
{
    std::ifstream file(filepath);
    std::string fileContent, line;
    while (!file.eof())
    {
        file >> line;
        fileContent.append(line);
    }
    file.close();

    document = Microsoft::glTF::Deserialize(fileContent);
}

void GLTFLoader::loadBuffers()
{
    for (const auto &buffer : document.buffers.Elements())
    {
        std::string name = buffer.uri;
        std::ifstream file("Sponza/" + name, std::ios::binary);

        std::vector<uint8_t> data(buffer.byteLength);
        file.read(reinterpret_cast<char *>(data.data()), data.size());

        buffers[name] = data;
    }
}

void GLTFLoader::loadMeshes(ve::Device &device)
{
    for (const auto &mesh : document.meshes.Elements())
    {
        std::vector<std::shared_ptr<ve::Mesh>> meshes{};

        for (const auto &primitive : mesh.primitives)
        {
            std::vector<ve::Mesh::Vertex> vertices{};
            std::vector<uint16_t> indices{};

            const auto &indicesAccessorId = primitive.indicesAccessorId;
            const auto &indicesAccessor = document.accessors.Get(indicesAccessorId);

            const auto &indexBufferViewId = indicesAccessor.bufferViewId;
            const auto &indexBufferView = document.bufferViews.Get(indexBufferViewId);

            const auto &indexBufferId = indexBufferView.bufferId;
            const auto &indexBuffer = buffers.at(document.buffers.Get(indexBufferId).uri);
            const auto indexOffset = indexBufferView.byteOffset + indicesAccessor.byteOffset;

            const auto &indexCount = indicesAccessor.count;
            indices.resize(indexCount);

            std::memcpy(indices.data(), indexBuffer.data() + indexOffset, indexCount * sizeof(uint16_t));

            vertices.resize(document.accessors.Get(primitive.GetAttributeAccessorId("POSITION")).count);

            if (primitive.HasAttribute("POSITION"))
            {
                const auto &positionAccessorId = primitive.GetAttributeAccessorId("POSITION");
                const auto &positionAccessor = document.accessors.Get(positionAccessorId);

                const auto &positionBufferViewId = positionAccessor.bufferViewId;
                const auto &positionBufferView = document.bufferViews.Get(positionBufferViewId);

                const auto &positionBufferId = positionBufferView.bufferId;
                const auto &positionBuffer = buffers.at(document.buffers.Get(positionBufferId).uri);
                const auto positionOffset = positionBufferView.byteOffset + positionAccessor.byteOffset;

                const auto &positionCount = positionAccessor.count;

                for (size_t i = 0; i < positionCount; ++i)
                {
                    const auto position = reinterpret_cast<const glm::vec3 *>(positionBuffer.data() + positionOffset);
                    vertices[i].position = position[i];
                    vertices[i].position.x = -vertices[i].position.x;
                }
            }

            if (primitive.HasAttribute("NORMAL"))
            {
                const auto &normalAccessorId = primitive.GetAttributeAccessorId("NORMAL");
                const auto &normalAccessor = document.accessors.Get(normalAccessorId);

                const auto &normalBufferViewId = normalAccessor.bufferViewId;
                const auto &normalBufferView = document.bufferViews.Get(normalBufferViewId);

                const auto &normalBufferId = normalBufferView.bufferId;
                const auto &normalBuffer = buffers.at(document.buffers.Get(normalBufferId).uri);
                const auto normalOffset = normalBufferView.byteOffset + normalAccessor.byteOffset;

                const auto &normalCount = normalAccessor.count;

                for (size_t i = 0; i < normalCount; ++i)
                {
                    const auto normal = reinterpret_cast<const glm::vec3 *>(normalBuffer.data() + normalOffset);
                    vertices[i].normal = normal[i];
                    vertices[i].normal.x = -vertices[i].normal.x;
                }
            }

            if (primitive.HasAttribute("TANGENT"))
            {
                const auto &tangentAccessorId = primitive.GetAttributeAccessorId("TANGENT");
                const auto &tangentAccessor = document.accessors.Get(tangentAccessorId);

                const auto &tangentBufferViewId = tangentAccessor.bufferViewId;
                const auto &tangentBufferView = document.bufferViews.Get(tangentBufferViewId);

                const auto &tangentBufferId = tangentBufferView.bufferId;
                const auto &tangentBuffer = buffers.at(document.buffers.Get(tangentBufferId).uri);
                const auto tangentOffset = tangentBufferView.byteOffset + tangentAccessor.byteOffset;

                const auto &tangentCount = tangentAccessor.count;

                for (size_t i = 0; i < tangentCount; ++i)
                {
                    const auto tangent = reinterpret_cast<const glm::vec4 *>(tangentBuffer.data() + tangentOffset);
                    vertices[i].tangent = tangent[i];
                }
            }

            if (primitive.HasAttribute("TEXCOORD_0"))
            {
                const auto &texCoord0AccessorId = primitive.GetAttributeAccessorId("TEXCOORD_0");
                const auto &texCoord0Accessor = document.accessors.Get(texCoord0AccessorId);

                const auto &texCoord0BufferViewId = texCoord0Accessor.bufferViewId;
                const auto &texCoord0BufferView = document.bufferViews.Get(texCoord0BufferViewId);

                const auto &texCoord0BufferId = texCoord0BufferView.bufferId;
                const auto &texCoord0Buffer = buffers.at(document.buffers.Get(texCoord0BufferId).uri);
                const auto texCoord0Offset = texCoord0BufferView.byteOffset + texCoord0Accessor.byteOffset;

                const auto &texCoord0Count = texCoord0Accessor.count;

                for (size_t i = 0; i < texCoord0Count; ++i)
                {
                    const auto texCoord0 = reinterpret_cast<const glm::vec2 *>(texCoord0Buffer.data() + texCoord0Offset);
                    vertices[i].tex_coord_0 = texCoord0[i];
                }
            }

            if (primitive.HasAttribute("TEXCOORD_1"))
            {
                const auto &texCoord1AccessorId = primitive.GetAttributeAccessorId("TEXCOORD_1");
                const auto &texCoord1Accessor = document.accessors.Get(texCoord1AccessorId);

                const auto &texCoord1BufferViewId = texCoord1Accessor.bufferViewId;
                const auto &texCoord1BufferView = document.bufferViews.Get(texCoord1BufferViewId);

                const auto &texCoord1BufferId = texCoord1BufferView.bufferId;
                const auto &texCoord1Buffer = buffers.at(document.buffers.Get(texCoord1BufferId).uri);
                const auto texCoord1Offset = texCoord1BufferView.byteOffset + texCoord1Accessor.byteOffset;

                const auto &texCoord1Count = texCoord1Accessor.count;

                for (size_t i = 0; i < texCoord1Count; ++i)
                {
                    const auto texCoord1 = reinterpret_cast<const glm::vec2 *>(texCoord1Buffer.data() + texCoord1Offset);
                    vertices[i].tex_coord_1 = texCoord1[i];
                }
            }

            if (primitive.HasAttribute("COLOR_0"))
            {
                const auto &color0AccessorId = primitive.GetAttributeAccessorId("COLOR_0");
                const auto &color0Accessor = document.accessors.Get(color0AccessorId);

                const auto &color0BufferViewId = color0Accessor.bufferViewId;
                const auto &color0BufferView = document.bufferViews.Get(color0BufferViewId);

                const auto &color0BufferId = color0BufferView.bufferId;
                const auto &color0Buffer = buffers.at(document.buffers.Get(color0BufferId).uri);
                const auto color0Offset = color0BufferView.byteOffset + color0Accessor.byteOffset;

                const auto &color0Count = color0Accessor.count;

                for (size_t i = 0; i < color0Count; ++i)
                {
                    const auto color0 = reinterpret_cast<const glm::vec4 *>(color0Buffer.data() + color0Offset);
                    vertices[i].color_0 = color0[i];
                }
            }

            ve::Mesh::Builder modelBuilder;
            modelBuilder.vertices = vertices;
            modelBuilder.indices = indices;

            auto mMesh = std::make_shared<ve::Mesh>(device, modelBuilder);
            mMesh->setMaterial(materials.at(primitive.materialId));

            meshes.push_back(mMesh);
        }

        this->meshes.emplace(mesh.id, meshes);
    }
}

void GLTFLoader::loadImages(ve::Device &device)
{
    std::vector<std::pair<int, int>> sizes;
    std::vector<stbi_uc*> binaries;

    sizes.resize(document.images.Elements().size());
    binaries.resize(document.images.Elements().size());

    std::for_each(std::execution::par, document.images.Elements().begin(), document.images.Elements().end(), [&sizes, &binaries](const Microsoft::glTF::Image &image) {
        std::pair size{ 0, 0 };
        stbi_uc* pixels = stbi_load(("Sponza/" + image.uri).c_str(), &size.first, &size.second, nullptr, STBI_rgb_alpha);

        sizes[std::stoi(image.id)] = size;
        binaries[std::stoi(image.id)] = pixels;
	});

    for (size_t i = 0; i < document.images.Elements().size(); ++i)
	{
		const auto &image = document.images.Elements()[i];
		const auto &[width, height] = sizes[i];
    	auto* pixels = binaries[i];

		std::shared_ptr<ve::Image> mImage = ve::Image::createTextureFromMemory(device, pixels, width, height);
		images.emplace(image.id, mImage);

		stbi_image_free(pixels);
	}
}

void GLTFLoader::loadTextures()
{
    for (const auto &texture : document.textures.Elements())
    {
        const auto mTexture = std::make_shared<ve::Texture>();
        mTexture->id = texture.id;
        mTexture->name = texture.name;
        mTexture->samplerId = texture.samplerId;
        mTexture->imageId = texture.imageId;

        textures.emplace(texture.id, mTexture);
    }
}

void GLTFLoader::loadMaterials(ve::Device& device)
{
    for (const auto &material : document.materials.Elements())
    {
        auto mMaterial = std::make_shared<ve::Material>(device);
        mMaterial->name = material.name;
        mMaterial->id = material.id;

        mMaterial->parameters.alphaCutoff = material.alphaCutoff;
        mMaterial->parameters.doubleSided = material.doubleSided;

        mMaterial->parameters.emissiveFactor = {material.emissiveFactor.r, material.emissiveFactor.g, material.emissiveFactor.b};

        if (!material.emissiveTexture.textureId.empty()) {
            const auto& texture = textures.at(material.emissiveTexture.textureId);
            mMaterial->samplers.emissiveTexture = images.at(texture->imageId);
        }

        if (!material.normalTexture.textureId.empty()) {
            const auto& texture = textures.at(material.normalTexture.textureId);
            mMaterial->samplers.normalTexture = images.at(texture->imageId);
        }

        if (!material.occlusionTexture.textureId.empty()) {
            const auto& texture = textures.at(material.occlusionTexture.textureId);
            mMaterial->samplers.occlusionTexture = images.at(texture->imageId);
        }

        mMaterial->parameters.baseColorFactor = {
            material.metallicRoughness.baseColorFactor.r,
            material.metallicRoughness.baseColorFactor.g,
            material.metallicRoughness.baseColorFactor.b,
            material.metallicRoughness.baseColorFactor.a};

        if (!material.metallicRoughness.baseColorTexture.textureId.empty()) {
            const auto& texture = textures.at(material.metallicRoughness.baseColorTexture.textureId);
            mMaterial->samplers.baseColorTexture = images.at(texture->imageId);
        }

        mMaterial->parameters.roughnessFactor = material.metallicRoughness.roughnessFactor;
        mMaterial->parameters.metallicFactor = material.metallicRoughness.metallicFactor;

        if (!material.metallicRoughness.metallicRoughnessTexture.textureId.empty()) {
            const auto& texture = textures.at(material.metallicRoughness.metallicRoughnessTexture.textureId);
            mMaterial->samplers.metallicRoughnessTexture = images.at(texture->imageId);
        }

        materials.emplace(material.id, mMaterial);
    }
}

std::vector<std::unique_ptr<ve::Light>> GLTFLoader::loadLights(ve::Device& device) const
{
	std::vector<std::unique_ptr<ve::Light>> lights{};

    const auto& lightsJson = document.extensions.at("KHR_lights_punctual");

    auto parser = simdjson::dom::parser();
    const auto& result = parser.parse(lightsJson);

    const auto& lightList = result.at_key("lights").get_array();

    for (const auto light : lightList)
    {
        const auto& arrColor = light.at_key("color").get_array();
        glm::vec<3, double> color;
        arrColor.at(0).get(color.x);
        arrColor.at(1).get(color.y);
        arrColor.at(2).get(color.z);

    	double intensity;
    	light.at_key("intensity").get(intensity);

        const auto type = light.at_key("type").get_string().value();

        std::cout << "Light type: " << type << std::endl;
        std::cout << "Light intensity: " << intensity << std::endl;
        std::cout << "Light color: " << color.x << " " << color.y << " " << color.z << std::endl << std::endl;
    }

	// for (const auto &node : document.nodes.Elements())
	// {
	// 	if (node.extensions.find("KHR_lights_punctual") != node.extensions.end())
	// 	{
	//
	// 	}
	// }

	return lights;
}

std::vector<std::unique_ptr<ve::RenderObject>> GLTFLoader::loadRenderTargets(ve::Device &device) const
{
    const auto &scene = document.GetDefaultScene();
    const auto &nodes = document.nodes;

    std::vector<std::unique_ptr<ve::RenderObject>> renderObjects{};

    std::queue<std::pair<glm::mat4, std::string>> queue{};

    for (const auto& nodeName : scene.nodes)
    {
        queue.emplace(glm::mat4(1), nodeName);
    }

    while (!queue.empty())
    {
        auto [transformation, nodeName] = queue.front();
        queue.pop();

        // if (nodeName == "48" || nodeName == "98")
        //     continue;

        const auto &node = nodes.Get(nodeName);

        if (node.HasValidTransformType())
        {
            switch (node.GetTransformationType())
            {
            case Microsoft::glTF::TRANSFORMATION_IDENTITY:
                break;
            case Microsoft::glTF::TRANSFORMATION_MATRIX:
                transformation *= glm::mat4 {
                    node.matrix.values[0], node.matrix.values[1], node.matrix.values[2], node.matrix.values[3],
                    node.matrix.values[4], node.matrix.values[5], node.matrix.values[6], node.matrix.values[7],
                    node.matrix.values[8], node.matrix.values[9], node.matrix.values[10], node.matrix.values[11],
                    node.matrix.values[12], node.matrix.values[13], node.matrix.values[14], node.matrix.values[15] };
                break;
            case Microsoft::glTF::TRANSFORMATION_TRS:
                glm::vec3 translation = {
                    -node.translation.x,
                    node.translation.y,
                    node.translation.z };
                glm::quat rotation = {
                   -node.rotation.w,
                   - node.rotation.x,
                    node.rotation.y,
                    node.rotation.z};
                glm::vec3 scale = {
                    node.scale.x,
                    node.scale.y,
                    node.scale.z };

                transformation = glm::translate(transformation, translation);
                transformation = transformation * glm::mat4(rotation);
                transformation = glm::scale(transformation, scale);

                break;
            }
        }

        if (!node.meshId.empty())
        {
            const auto &meshes = this->meshes.at(node.meshId);
            auto renderObject = std::make_unique<ve::RenderObject>(device);
            renderObject->meshes = meshes;
            renderObject->localModelMatrix = transformation;

            renderObjects.emplace_back(std::move(renderObject));
        }


    	for (const auto &childName : node.children)
    	{
            queue.emplace(transformation, childName);
    	}
    }
    return renderObjects;
}

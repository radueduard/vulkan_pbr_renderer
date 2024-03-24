#pragma once
#include <glm/glm.hpp>

namespace ve
{
	enum LightType
	{
		Directional,
		Point
	};

	struct Light
	{
		glm::vec3 color;
		float intensity;
		LightType type;
		glm::vec3 position;
		glm::vec3 direction;
	};
}

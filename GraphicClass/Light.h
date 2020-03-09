#pragma once
#include "glm/glm.hpp"
namespace cg {

	struct PointLight {
		alignas(16) glm::vec3 position;
		alignas(16) glm::vec3 ambient;
		alignas(16) glm::vec3 diffuse;
		alignas(16) glm::vec3 specular;
		alignas(4) float constant;
		alignas(4) float linear;
		alignas(4) float quadratic;
	};

	struct DirectionalLight {
		alignas(16) glm::vec3 direction;
		alignas(16) glm::vec3 ambient;
		alignas(16) glm::vec3 diffuse;
		alignas(16) glm::vec3 specular;
	};

}
#include "Util.h"

namespace Util {
	glm::vec3 randomVec3(float minVal, float maxVal) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(minVal, maxVal);

		return glm::vec3(dist(gen), dist(gen), dist(gen));
	}; 
	
	float randomFloat(float minVal, float maxVal) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(minVal, maxVal);

		return dist(gen);
	};
}
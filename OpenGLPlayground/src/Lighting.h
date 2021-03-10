#pragma once

#include <glm/glm.hpp>

class Lighting {

	struct Direction_Light {
        glm::vec3 dir;
        glm::vec4 ambient_color;
        glm::vec4 diffuse_color;
        glm::vec4 specular_color;
        float ambient_strength;
	};


	struct Point_Light {
        glm::vec3 pos;
        glm::vec4 ambient_color;
        glm::vec4 diffuse_color;
        glm::vec4 specular_color;
        float ambient_strength;
	};

public:

	Direction_Light direction_light;
	Point_Light point_light;

	Lighting() {}

	~Lighting() {}

	void init(glm::vec3& pos, glm::vec3& front)
	{
        direction_light.ambient_color = glm::vec4(1.0f);
        direction_light.diffuse_color = glm::vec4(1.0f);
        direction_light.specular_color = glm::vec4(1.0f);
        direction_light.ambient_strength = 0.1;
        point_light.ambient_color = glm::vec4(1.0f);
        point_light.diffuse_color = glm::vec4(1.0f);
        point_light.specular_color = glm::vec4(1.0f);
        point_light.ambient_strength = 0.1;
        reset(pos, front);
	};
    
    void reset(glm::vec3& pos, glm::vec3& front)
    {
        direction_light.dir = front + glm::vec3(0.0f, -1.0f, -1.0f);
        point_light.pos = pos;
    }
};

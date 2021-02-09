#pragma once

#include <algorithm> 

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	ROTATE_X_UP,
	ROTATE_X_DOWN,
	ROTATE_Y_UP,
	ROTATE_Y_DOWN,
	ROTATE_Z_UP,
	ROTATE_Z_DOWN,
};

class Camera {
public:
	unsigned int width;
	unsigned int height;
    GLfloat cameraSpeed = 0.05f;
    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

	Camera(
		unsigned int width_ = 800,
		unsigned int height_ = 600,
        GLfloat cameraSpeed_ = 0.05f
	)
	{
		this->width = width_;
		this->height = height_;
        this->cameraSpeed = cameraSpeed_;
	}

	void init() {
		reset();
	};

	void reset() {
        cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
	}

	void process_keyboard(Camera_Movement direction, GLfloat velocity)
	{
		if (direction == FORWARD) {
            cameraPos += cameraSpeed * cameraFront;
		}
		if (direction == BACKWARD) {
            cameraPos -= cameraSpeed * cameraFront;
		}
		if (direction == LEFT) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (direction == RIGHT) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (direction == UP) {
            cameraPos += cameraSpeed * cameraUp;
		}
		if (direction == DOWN) {
            cameraPos -= cameraSpeed * cameraUp;
		}
		if (direction == ROTATE_X_UP) {

		}
		if (direction == ROTATE_X_DOWN) {

		}
		if (direction == ROTATE_Y_UP) {

		}
		if (direction == ROTATE_Y_DOWN) {

		}
		if (direction == ROTATE_Z_UP) {

		}
		if (direction == ROTATE_Z_DOWN) {

		}
	}
};

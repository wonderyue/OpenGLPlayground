#pragma once

#include <algorithm>
#include <glm/glm.hpp>

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
    GLfloat angleSpeed = 1.0f;
    GLfloat pitch = 0.0f;
    GLfloat yaw = -90.0f;
    GLfloat roll = 90.0f;

	Camera(
		unsigned int width_ = 800,
		unsigned int height_ = 600
	)
	{
		this->width = width_;
		this->height = height_;
	}

	void init() {
		reset();
	};

	void reset() {
        cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
        pitch = 0.0f;
        yaw = -90.0f;
        roll = 90.0f;
	}
    
    void update_direction()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
        glm::vec3 up;
        up.x = -cos(glm::radians(yaw)) * sin(glm::radians(pitch)) * sin(glm::radians(roll)) - sin(glm::radians(yaw)) * cos(glm::radians(roll));
        up.y = cos(glm::radians(pitch)) * sin(glm::radians(roll));
        up.z = -sin(glm::radians(yaw)) * sin(glm::radians(pitch)) * sin(glm::radians(roll)) + cos(glm::radians(yaw)) * cos(glm::radians(roll));
        cameraUp = glm::normalize(up);
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
            pitch += angleSpeed;
		}
		if (direction == ROTATE_X_DOWN) {
            pitch -= angleSpeed;
		}
		if (direction == ROTATE_Y_UP) {
            yaw += angleSpeed;
		}
		if (direction == ROTATE_Y_DOWN) {
            yaw -= angleSpeed;
		}
		if (direction == ROTATE_Z_UP) {
            roll += angleSpeed;
		}
		if (direction == ROTATE_Z_DOWN) {
            roll -= angleSpeed;
		}
	}
};

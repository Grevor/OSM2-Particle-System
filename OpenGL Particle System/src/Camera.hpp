/*
 * Camera.hpp
 *
 *  Created on: 8 maj 2014
 *      Author: David
 */

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class Camera {
	vec3 position;
	float horizontalAngle; //rotation around y, (left angle)
	float verticalAngle; //rotation around -x, (up angle)
	float fovy;
	float aspect;
	float viewDistance;
	mat4 viewMatrix;
	mat4 projectionMatrix;
public:
	Camera();
	Camera(vec3 pos, float horiAngle, float vertAngle, float aspectRatio, float fovy, float viewDistance);
	virtual ~Camera();
	void setAspectRatio(float aspectRatio);
	void move(float forwardAmount, float rightAmount, float upAngle, float leftAngle);
	void move(float forward, float right, float up, float upAngle, float leftAngle);
	vec3 getPosition();
	mat4 getViewMatrix();
	mat4 getProjectionMatrix();
private:
	void updateViewMatrix();
	void updateViewMatrix(vec3 forward, vec3 up);
	void updateProjectionMatrix();
};

#endif /* CAMERA_HPP_ */

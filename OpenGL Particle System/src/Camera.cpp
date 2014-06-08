/*
 * Camera.cpp
 *
 *  Created on: 8 maj 2014
 */


#include "Camera.hpp"

Camera::Camera() :
	Camera(vec3(0,2,-5), 0, 0, 4.0f/3.0f, 45.0f, 100.0f)
{

}

Camera::Camera(vec3 pos, float horiAngle, float vertAngle, float aspectRatio, float fovy, float viewDistance) :
	position(pos)
{
	this->verticalAngle = vertAngle;
	this->horizontalAngle = horiAngle;
	this->aspect = aspectRatio;
	this->fovy = fovy;
	this->viewDistance = viewDistance;
	updateViewMatrix();
	updateProjectionMatrix();
}

Camera::~Camera()
{
	// TODO Auto-generated destructor stub
}

void Camera::move(float forwardAmount, float rightAmount, float upAngle, float leftAngle) {
	horizontalAngle += leftAngle;
	verticalAngle += upAngle;
	vec3 forward(
				cos(verticalAngle) * sin(horizontalAngle),
				sin(verticalAngle),
				cos(verticalAngle) * cos(horizontalAngle)
		);
	vec3 right(
				sin(horizontalAngle - 3.14f/2.0f),
				0,
				cos(horizontalAngle - 3.14f/2.0f)
		);
	vec3 up = glm::cross(right, forward);

	position += forward * forwardAmount;
	position += right * rightAmount;

	updateViewMatrix(forward, up);
	updateProjectionMatrix();
}

void Camera::move(float forward, float right, float up, float upAngle, float leftAngle) {
	position += vec3(0,up,0);

	vec3 forwardVec(
			 sin(horizontalAngle),
			0,
			cos(horizontalAngle)
	);
	vec3 rightVec(
			sin(horizontalAngle - 3.14f/2.0f),
			0,
			cos(horizontalAngle - 3.14f/2.0f)
	);
	position += forwardVec * forward;
	position += rightVec * right;
	horizontalAngle += leftAngle;
	verticalAngle += upAngle;
	updateViewMatrix();
	updateProjectionMatrix();
}

void Camera::setAspectRatio(float aspectRatio) {
	this->aspect = aspectRatio;
	updateProjectionMatrix();
}

vec3 Camera::getPosition() {
	return position;
}

mat4 Camera::getViewMatrix() {
	return viewMatrix;
}

mat4 Camera::getProjectionMatrix() {
	return projectionMatrix;
}

void Camera::updateViewMatrix() {
	vec3 forward(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
	);
	vec3 right(
			sin(horizontalAngle - 3.14f/2.0f),
			0,
			cos(horizontalAngle - 3.14f/2.0f)
	);
	vec3 up = glm::cross( right, forward );
	/*vec3 up = vec3(sin(-verticalAngle) * cos(horizontalAngle)
			,cos(verticalAngle),
			sin(-verticalAngle) * sin(horizontalAngle));*/

	updateViewMatrix(forward, up);
}

void Camera::updateViewMatrix(vec3 forward, vec3 up) {
	this->viewMatrix = glm::lookAt(position, position+forward, up);
}

void Camera::updateProjectionMatrix() {
	this->projectionMatrix = glm::perspective(fovy, aspect, 0.1f, viewDistance);
}

vec3 Camera::getForwardVector() {
	return vec3(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
	);
}

vec3 Camera::getRightVector() {
	return vec3(
				sin(horizontalAngle - 3.14f/2.0f),
				0,
				cos(horizontalAngle - 3.14f/2.0f)
		);
}

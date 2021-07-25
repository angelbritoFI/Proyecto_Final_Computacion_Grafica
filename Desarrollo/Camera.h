#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include <glfw3.h>

class Camera
{
public:
	//se ponen publicas para el manejo de posicion y direccion de camaras
	glm::vec3 position;
	glm::vec3 front;
	GLfloat yaw;
	GLfloat pitch;
	glm::vec3 worldUp;
	//
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void keyControlXZ(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();
	glm::mat4 calculateViewMatrix();
	void      setCameraPosition(glm::vec3 pos);
	void	  setCameraDirection(glm::vec3 front);
	void	  setGiro(float x,float y);

	~Camera();

private:
	glm::vec3 up;
	glm::vec3 right;


	

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	void update();
};


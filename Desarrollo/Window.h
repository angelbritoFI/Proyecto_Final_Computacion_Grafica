#pragma once
/*
* Materia: Laboratorio de Computaci�n Gr�fica e Interacci�n Humano-Computadora
* Semestre 2021-2
* Fecha de creaci�n: 12/07/2021
* Descripci�n: Objeto Window
* Autores:
*	Brito Segura Angel
*	Hern�ndez Torres Agust�n de Jes�s
* 	Huarte Nolasco Mario
*/
#include <stdio.h>
#include <glew.h>
#include <glfw3.h>

class Window {
	public:
		Window();
		Window(GLint windowWidth, GLint windowHeight);
		int Initialise();
		GLfloat getBufferWidth() { 
			return bufferWidth; 
		}
		GLfloat getBufferHeight() { 
			return bufferHeight; 
		}
		GLfloat getXChange();
		GLfloat getYChange();

		//M�todo p�blico para ver si hay que cambiar de color
		bool getCambioColor() { 
			return cambioColor;
		}

		//M�todos p�blicos para mover al avatar
		GLfloat getMovAvatarX() {
			return movimientoAvatar[0]; // X
		}
		GLfloat getMovAvatarZ() {
			return movimientoAvatar[1]; // Z
		}
				
		bool getShouldClose() {
			return  glfwWindowShouldClose(mainWindow);
		}
		bool* getsKeys() {
			return keys;
		}
		void swapBuffers() { 
			return glfwSwapBuffers(mainWindow); 
		}
	
		~Window();
	private: 
		GLFWwindow *mainWindow;
		GLint width, height;
		bool keys[1024];
		GLint bufferWidth, bufferHeight;
		void createCallbacks();
		GLfloat lastX;
		GLfloat lastY;
		GLfloat xChange;
		GLfloat yChange;
		bool cambioColor; //Control del color
		GLfloat movimientoAvatar[1]; //Variable float para el movimiento de Wall-E
		bool mouseFirstMoved;
		static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
		static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
};


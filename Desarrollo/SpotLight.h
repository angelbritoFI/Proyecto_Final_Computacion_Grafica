#pragma once
/*
* Materia: Laboratorio de Computaci�n Gr�fica e Interacci�n Humano-Computadora
* Semestre 2021-2
* Fecha de creaci�n: 12/07/2021
* Descripci�n: Objeto SpotLight
* Autores:
*	Brito Segura Angel
*	Hern�ndez Torres Agust�n de Jes�s
* 	Huarte Nolasco Mario
*/
#include "PointLight.h"
class SpotLight :
	public PointLight {
		public:
			SpotLight();

			SpotLight(GLfloat red, GLfloat green, GLfloat blue,
				GLfloat aIntensity, GLfloat dIntensity,
				GLfloat xPos, GLfloat yPos, GLfloat zPos,
				GLfloat xDir, GLfloat yDir, GLfloat zDir,
				GLfloat con, GLfloat lin, GLfloat exp,
				GLfloat edg);

			void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
				GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation,
				GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
				GLuint edgeLocation);

			void SetFlash(glm::vec3 pos, glm::vec3 dir);
			void SetPos(glm::vec3 pos);
			void SetColor(glm::vec3 col); //M�todo para cambiar el color de la luz

			~SpotLight();

		private:
			glm::vec3 direction;

			GLfloat edge, procEdge;
	};


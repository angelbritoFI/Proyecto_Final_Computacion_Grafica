/*
* Materia: Laboratorio de Computación Gráfica e Interacción Humano-Computadora
* Semestre 2021-2
* Fecha de creación: 12/07/2021
* Descripción: Archivo principal de la ejecución del Proyecto Final
* Autores:
*	Brito Segura Angel
*	Hernández Torres Agustín de Jesús
* 	Huarte Nolasco Mario
*/
#define STB_IMAGE_IMPLEMENTATION //para cargar imagen

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

//Piso
Texture plainTexture;
Texture pisoTexture;

//Personajes
//Wall-E
Model Avatar_M; 
Model BrazoD_M;
Model BrazoI_M;
Model PieD_M;
Model PieI_M;

Model Eva_M;
Model FinnJake_M;
Model Clon_M;

//Naves
Model InterceptorJedi_M;
Model SpeederBike_M;

Model Basura1_M;
Model Basura2_M;
Model Basura3_M;
Model DeathStar_M;
Model Luna_M;
Model Sol_M;

//skybox dia noche
Skybox skybox;
Skybox skyboxNight;

//Valores auxiliares skybox
bool skyboxNoche = false;
float tmp_skybox = 0;


//materiales objeto interactúe con la luz
Material Material_brillante; 
Material Material_opaco;

// 3 tipos de luz
//luz direccional (sol, ilumana todo el escenario por igual, debe de existir)
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS]; //luz puntual (ilumna toda la esfera)
SpotLight spotLights[MAX_SPOT_LIGHTS]; //solo una parte como cono

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//Variables auxiliares tiempo
float tiempo_offset = 0.05;

//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

// Crear el piso
void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

//Cubos de basura
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};


	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.48f,	0.64f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.27f,	0.64f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}

void CreateShaders() {
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main() {
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCubo();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 1.0f, 0.5f);

	//Piso
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso_marte.jpg");
	pisoTexture.LoadTextureA();

	//Wall-E
	Avatar_M = Model();
	Avatar_M.LoadModel("Models/Wall-E.obj");
	BrazoI_M = Model();
	BrazoI_M.LoadModel("Models/Brazo-Derecho-Frente.obj");
	BrazoD_M = Model();
	BrazoD_M.LoadModel("Models/Brazo-Izquierdo-Frente.obj");
	PieD_M = Model();
	PieD_M.LoadModel("Models/Pie-Derecho.obj");
	PieI_M = Model();
	PieI_M.LoadModel("Models/Pie-Izquierdo.obj");

	Eva_M = Model();
	Eva_M.LoadModel("Models/EVA.obj");

	Clon_M = Model();
	Clon_M.LoadModel("Models/Stormtrooper.obj");
	
	InterceptorJedi_M = Model();
	InterceptorJedi_M.LoadModel("Models/InterceptorJedi.fbx");

	SpeederBike_M = Model();
	SpeederBike_M.LoadModel("Models/SpeederBike.obj");

	FinnJake_M = Model();
	FinnJake_M.LoadModel("Models/Finn-Jake.obj");

	Basura1_M = Model();
	Basura1_M.LoadModel("Models/CuboBasura1.obj");

	Basura2_M = Model();
	Basura2_M.LoadModel("Models/CuboBasura2.obj");

	Basura3_M = Model();
	Basura3_M.LoadModel("Models/CuboBasura3.obj");

	DeathStar_M = Model();
	DeathStar_M.LoadModel("Models/DeathStar.obj");

	Luna_M = Model();
	Luna_M.LoadModel("Models/Luna.obj");

	Sol_M = Model();
	Sol_M.LoadModel("Models/Sol.obj");

	//Skybox dia
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);

	//skybox noche
	std::vector<std::string> skyboxFacesNight;
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_rt.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_lf.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_dn.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_up.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_bk.tga");
	skyboxFacesNight.push_back("Textures/Skybox/cupertin-lake-night_ft.tga");
	skyboxNight = Skybox(skyboxFacesNight);

	// Se declaran los materiales de iluminación
	Material_brillante = Material(4.0f, 256); //Componente espectacular, reflejo (luz azul)
	Material_opaco = Material(0.3f, 4); //Si es muy pequeño no se refleja nada

	// Se declaran las luces
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, //de que color va ser la luz, color adicional que se le agregarán a los objetos
		0.3f, 0.3f, //coeficiente ambiental que tan intenso es la luz y coeficiente difuso que tan claros son los objetos
		0.0f, 0.0f, -1.0f); //vector de posisción en Z negativo para ver todo el rango
	
	//contador de luces puntuales (no se requiere si solo se tiene una sola luz)
	unsigned int pointLightCount = 0;
	
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f, //color azul
		0.0f, 1.0f, //coeficientes
		2.0f, 1.5f, 1.5f, //posición dentro del escenario
		0.3f, 0.2f, 0.1f); //ecuación de segundo grado para la atenuación ax2 + bx + c
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna (luz ligada a la cámara)
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f, //color blanco
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, //alcance
		5.0f); //apertura (radio) del cono entre más grande será más grande la circunferencia
	spotLightCount++;

	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f, //color verde
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, -5.0f, 0.0f, //ecuación de segundo grado
		1.0f, 0.0f, 0.0f,
		30.0f);
	spotLightCount++;
	
	spotLights[2] = SpotLight(1.0f, 1.0f, 0.0f, //color amarillo
		1.0f, 1.0f, //coeficientes
		0.0f, 0.0f, 0.0f, //posición dentro del escenario
		-1.0f, -1.0f, 0.0f, //ecuación de segundo grado
		1.0f, 0.0f, 0.0f, //alcance
		35.0f); // radio del cono
	spotLightCount++;
	
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);
	
	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose()) {

		//Variables para manejar el tiempo entre computadoras
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ------------------------------------------ CAMBIO DE SKYBOX ------------------------------------------
		// skybox de día
		if (tmp_skybox <= 15.0f && !skyboxNoche)
		{
			tmp_skybox += tiempo_offset * deltaTime;
			skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
			shaderList[0].UseShader();

			glm::mat4 model(1.0);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-200.0f, 100.0f, 20.0f));
			model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Sol_M.RenderModel();
			// Si el tiempo es el último tiempo para el skybox actual, entonces se debe cambiar el tipo de skybox
			if (tmp_skybox >= 14.99f)
				skyboxNoche = true;
		}
		// Proyección de skybox de noche
		if (tmp_skybox >= 0.0f && skyboxNoche)
		{
			tmp_skybox -= tiempo_offset * deltaTime;
			skyboxNight.DrawSkybox(camera.calculateViewMatrix(), projection);
			shaderList[0].UseShader();

			glm::mat4 model(1.0);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(-200.0f, 100.0f, 20.0f));
			model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Luna_M.RenderModel();

			// Si el tiempo es el último tiempo para el skybox actual, entonces se debe cambiar el tipo de skybox
			if (tmp_skybox <= 0.01f)
				skyboxNoche = false;
		}

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//luz ligada a la cámara de tipo flash 
		glm::vec3 lowerLight = camera.getCameraPosition(); //Se toma la posición de la cámara
		lowerLight.y -= 0.3f; //se baja para que no apunten al mismo lado
		//función programada con posición y dirección de esos valores
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		// spotLights[0].SetPos solo recibe un vector de posición

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		
		glm::mat4 model(1.0);
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		//agregar material al plano de piso
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		if (mainWindow.getCambioColor()) {
			spotLights[2].SetPos(glm::vec3(-1.4f, 2.0f, 9.0f));
			spotLights[2].SetColor(glm::vec3(1.0f, 1.0f, 0.0f)); //Cambiar el color de la luz
		} else { // No hay cambio de color
			spotLights[2].SetPos(glm::vec3(-1.4f, 2.0f, 9.0f)); 
			spotLights[2].SetColor(glm::vec3(1.0f, 0.0f, 0.0f)); //Cambiar el color de la luz
		}

		spotLights[1].SetPos(glm::vec3(-20.0f, 6.0f, -20.0));

		// ------------------------------------------ CARGA DE MODELOS ------------------------------------------
		//Wall-E
		glm::mat4 modelaux(1.0);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getMovAvatarX(), -1.0f, 10.0f + mainWindow.getMovAvatarZ()));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Avatar_M.RenderModel(); //Cuerpo de Wall-E		

		//Brazo derecho Wall-E
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 4.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoD_M.RenderModel();

		//Brazo izquierdo Wall-E
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -12.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BrazoI_M.RenderModel();

		//Pie derecho Wall-E
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.5f));
		model = glm::rotate(model, 10 * (mainWindow.getMovAvatarX() + mainWindow.getMovAvatarZ()) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotación de sus engranes
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PieD_M.RenderModel();

		//Pie izquierdo Wall-E
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
		model = glm::rotate(model, 10 * (mainWindow.getMovAvatarX() + mainWindow.getMovAvatarZ()) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); //Rotación de sus engranes
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PieI_M.RenderModel();

		//Eva
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Eva_M.RenderModel();

		//Nave Interceptor Jedi
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 50.0f, 20.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		InterceptorJedi_M.RenderModel();

		//Speeder Bike
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SpeederBike_M.RenderModel();

		// Stormtrooper
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Clon_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(25.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		FinnJake_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35.0f, 4.0f, 20.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura1_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 4.0f, 20.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura2_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.0f, 4.0f, 20.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Basura3_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, 50.0f, 20.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DeathStar_M.RenderModel();

		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(200.0f, 100.0f, 20.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luna_M.RenderModel();*/

		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-200.0f, 100.0f, 20.0f));
		model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Sol_M.RenderModel();*/
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

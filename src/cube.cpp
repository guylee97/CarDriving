//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include<iostream>
#include<vector>
#include "cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "texture.hpp"
using namespace std;

glm::mat4 projectMat;
glm::mat4 viewMat;
glm::mat4 modelMat = glm::mat4(1.0f); //
glm::vec3 car_pos(0, 0, 0),snapshot(0,0,0),cam(0,0,0),rot(0,0,1); //


enum eShadeMode { NO_LIGHT, GOURAUD, PHONG, NUM_LIGHT_MODE }; //
GLuint pvmMatrixID;


float rotAngle= 0.0f , fire = 0.0f, fire_temp=0.0f,water = 0.0f, angle = glm::radians(0.0f),angle_temp, timer = 0.0f;
float car_velocityx = cos(angle);
float car_velocityy = sin(angle);

typedef glm::vec4  color4;
typedef glm::vec4  point4;


const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

//point4 points[NumVertices];
color4 colors[NumVertices];
vector<glm::vec4> normals; //
vector<glm::vec4> points; //
vector<glm::vec2> texCoords; //

int shadeMode = GOURAUD; //
int isRotate = false; //
int isDrawingCar = true;
int isTexture = true; //

GLuint projectMatrixID; //
GLuint viewMatrixID; //
GLuint modelMatrixID; //
GLuint shadeModeID; //
GLuint textureModeID; //

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA colors
color4 vertex_colors[8] = {
	color4(1.0, 0.0, 1.0, 1.0),  // black
	color4(1.0, 0.0, 1.0, 1.0),   // cyan
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(1.0, 0.0, 1.0, 1.0),  // red
	color4(1.0, 0.0, 1.0, 1.0),  // green
	color4(1.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0)  // white
};

//----------------------------------------------------------------------------
void computeTexCoordQuad(glm::vec2 texcoord[4], int a, int c) //
{
	const int U = 0, V = 1;
	int u = 0, v = 0, u2 = 1, v2 = 1;
		// v0=(u, v)    v1=(u2, v)   <= quadangle
		// v2=(u, v2)   v3=(u2, v2)
	if (a == 1) {
		u = 0;  v = 1;
	}	
	else if (a == 2) {
		u = 1;  v = 0;
	}
	else if (a == 3) {
		u = 1;  v = 1;
	}
	else if (a == 4) {
		u = 3;  v = 1;
	}
	else if (a == 5) {
		u = 3;  v = 0;
	}
	else if (a == 6) {
		u = 2;  v = 0;
	}
	else if (a == 7) {
		u = 2;  v = 1;
	}
	else if (a == 0) {
		u = 0;  v = 0;
	}

	if (c == 1) {
		u2 = 4;  v2 = 1;
	}
	else if (c == 2) {
		u2 = 1;  v2 = 3;
	}
	else if (c == 3) {
		u2 = 1;  v2 = 1;
	}
	else if (c == 4) {
		u2 = 2;  v2 = 2;
	}
	else if (c == 5) {
		u2 = 2;  v2 = 0;
	}
	else if (c == 6) {
		u2 = 3;  v2 = 0;
	}
	else if (c == 7) {
		u2 = 2;  v2 = 1;
	}
	else if (c == 0) {
		u2 = 4;  v2 = 0;
	}
	
	texcoord[0][U] = texcoord[2][U] = (float)u / 4;
	texcoord[1][U] = texcoord[3][U] = (float)u / 3;

	texcoord[0][V] = texcoord[1][V] = (float)v2 / 4;
	texcoord[2][V] = texcoord[3][V] = (float)v2 / 3;

	if (u2 == 0) // last column
	{
		texcoord[1][U] = texcoord[3][U] = 1.0;
	}
	//texcoord[u] = atan2(normals[y], normals[x]) / (2 * PI) + 0.5;
	//texcoord[v] = acos(normals[z]/sqrt(length(normal))) / PI;
}
// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points.push_back(vertices[a]);  Index++;
	colors[Index] = vertex_colors[b]; points.push_back(vertices[b]);  Index++;
	colors[Index] = vertex_colors[c]; points.push_back(vertices[c]);  Index++;
	colors[Index] = vertex_colors[a]; points.push_back(vertices[a]);  Index++;
	colors[Index] = vertex_colors[c]; points.push_back(vertices[c]);  Index++;
	colors[Index] = vertex_colors[d]; points.push_back(vertices[d]);  Index++;
	
	glm::vec2 texcoord[4]; //
	computeTexCoordQuad(texcoord, a,c); //
	texCoords.push_back(texcoord[0]); //
	texCoords.push_back(texcoord[2]);
	texCoords.push_back(texcoord[3]);
	texCoords.push_back(texcoord[0]);
	texCoords.push_back(texcoord[3]);
	texCoords.push_back(texcoord[1]);
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

void computeNormals(vector<glm::vec4> points) //
{
	int size = 36;
	for (int i = 0; i < size; i++)
	{
		glm::vec4 n;
		for (int k = 0; k < 3; k++)
		{
			n[k] = points[i][k];
		}
		n[3] = 0.0;
		glm::normalize(n);
		normals.push_back(n);
	}
}


//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	colorcube();
	computeNormals(points);
	

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer); 
	int vertSize = sizeof(points[0]) * points.size(); //
	int normalSize = sizeof(normals[0]) * normals.size(); //
	int texSize = sizeof(texCoords[0]) * texCoords.size(); //


	glBufferData(GL_ARRAY_BUFFER, vertSize + normalSize+texSize,
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, points.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertSize, normalSize, normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertSize + normalSize, texSize, texCoords.data());

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(vertSize));

	projectMatrixID = glGetUniformLocation(program, "mProject");
	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(projectMatrixID, 1, GL_FALSE, &projectMat[0][0]);

	viewMatrixID = glGetUniformLocation(program, "mView");
	viewMat = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);

	modelMatrixID = glGetUniformLocation(program, "mModel");
	modelMat = glm::mat4(1.0f);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);

	shadeModeID = glGetUniformLocation(program, "shadeMode");
	glUniform1i(shadeModeID, shadeMode);
	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);

	textureModeID = glGetUniformLocation(program, "isTexture");
	glUniform1i(textureModeID, isTexture);

	// Load the texture using any two methods
	GLuint Texture = loadBMP_custom("earth.bmp");
	//GLuint Texture = loadDDS("uvtemplate.DDS");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(program, "sphereTexture");

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

//----------------------------------------------------------------------------

void drawCar(glm::mat4 carMat)
{
	glm::mat4 modelMat, pvmMat,rotMat=glm::mat4(1.0f);
	glm::vec3 wheelPos[4];
	float speed = fire;

	wheelPos[0] = glm::vec3(0.3, 0.24, -0.1); // rear right
	wheelPos[1] = glm::vec3(0.3, -0.24, -0.1); // rear left
	wheelPos[2] = glm::vec3(-0.3, 0.24, -0.1); // front right
	wheelPos[3] = glm::vec3(-0.3, -0.24, -0.1); // front left
	
	// car body
	modelMat = glm::scale(carMat, glm::vec3(1, 0.6, 0.2));
	modelMat = glm::translate(modelMat, glm::vec3(car_pos[0], car_pos[1], 0.2));
	modelMat = glm::rotate(modelMat, angle, glm::vec3(0, 0, 1));

	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	

	// car top
	  //P*V*C*T*S*v
	modelMat = glm::scale(carMat, glm::vec3(0.5, 0.6, 0.2));
	modelMat = glm::translate(modelMat, glm::vec3(car_pos[0] * 2, car_pos[1], 1));
	modelMat = glm::rotate(modelMat, angle, glm::vec3(0, 0, 1));
	
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// car wheel
	for (int i = 0; i < 4; i++)
	{
		modelMat = glm::translate(carMat, wheelPos[i]);  //P*V*C*T*S*v
		modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.1, 0.2));
		modelMat = glm::translate(modelMat, glm::vec3(car_pos[0]*5, car_pos[1]*6, 0.2));
		modelMat = glm::rotate(modelMat, (speed *5*glm::radians(360.0f)) / (2.0f * 3.14f * 0.5f), glm::vec3(0, 1, 0));
		modelMat = glm::rotate(modelMat, angle, glm::vec3(0, 0, 1));
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	for (int i = 0; i <= 20; i++)
	{
		for (int j = 0; j <= 20; j++) {
			modelMat = glm::translate(carMat, glm::vec3(-10 + i, -10 + j, -0.2));  //P*V*C*T*S*v
			modelMat = glm::scale(modelMat, glm::vec3(1, 1, 0.001));
			glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMat[0][0]);
			glDrawArrays(GL_LINE_LOOP, 0, NumVertices);
		}
	}

}


void display(void)
{
	glm::mat4 worldMat, pvmMat,carMat,groundMat;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	worldMat = glm::mat4(1.0f);

	
	drawCar(worldMat);
	
	
		

	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void idle()
{
	static int prevTime = glutGet(GLUT_ELAPSED_TIME);
	int currTime = glutGet(GLUT_ELAPSED_TIME);


	if (abs(currTime - prevTime) >= 20)
	{
		float t = abs(currTime - prevTime);
		rotAngle += glm::radians(t*360.0f / 10000.0f);
		fire = t/(10000)*water;
		float car_velocityx = cos(angle);
		float car_velocityy = sin(angle);
		car_pos = glm::vec3(car_pos.x + fire * car_velocityx, car_pos.y + fire * car_velocityy, 0.2);
		viewMat = glm::lookAt(glm::vec3(car_pos.x - 2, car_pos.y*0.6, 2), glm::vec3(car_pos.x, car_pos.y*0.6, 0.2), glm::vec3(0, 0, 1));
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMat[0][0]);
		prevTime = currTime;
		glutPostRedisplay();
	}
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 072: 
		water += 1000;
		break;
	
	case 80:
		water -= 1000;
		break;
	case 033:  // Escape key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 't': case 'T':
		isTexture = !isTexture;
		glUniform1i(textureModeID, isTexture);
		glutPostRedisplay();
		break;
	case 'l': case 'L':
		shadeMode = (++shadeMode % NUM_LIGHT_MODE);
		glUniform1i(shadeModeID, shadeMode);
		glutPostRedisplay();
		break;
	}
}

void keyboard_spe(int key, int x, int y)
{
	glm::vec3 displacement = fire * glm::vec3(sin(angle), cos(angle),0.0f);

	switch (key)
	{
	case GLUT_KEY_LEFT:
		angle += 0.1f;
		break;
	case GLUT_KEY_RIGHT:
		angle -= 0.1f;
		break;
	case GLUT_KEY_UP:
		water += 10;
		break;
	case GLUT_KEY_DOWN:
		water -= 10;
		break;

	}
}

//----------------------------------------------------------------------------

void resize(int w, int h)
{
	float ratio = (float)w / (float)h;
	glViewport(0, 0, w, h);

	projectMat = glm::perspective(glm::radians(65.0f), ratio, 0.1f, 100.0f);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Color Car");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard_spe);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
	

	glutMainLoop();
	return 0;
}

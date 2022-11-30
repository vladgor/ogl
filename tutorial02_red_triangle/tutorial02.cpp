// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

#define W_WIDTH 1024
#define W_HEIGHT 768

float get_x(float angle) {
	return 4 * glm::cos(angle);
}

float get_y(float angle) {
	return 4 * glm::sin(angle);
}

float get_z(float angle) {
	return 4 * glm::sin(angle);
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Tutorial 02 - Red triangle", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

	GLuint matrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)W_WIDTH / (float)W_HEIGHT, 0.1f, 100.0f);

	float angle = 0;
	float step = 0.01f;
	glm::mat4 view = glm::lookAt(
		glm::vec3(get_x(angle), get_y(angle), get_z(angle)),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 1)
	);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 MVP = projection * view * model;

	static const GLfloat g_vertex_buffer_data[] = { 
		-0.7f, -0.7f, 0.5f, //
		 0.7f, -0.7f, 0.5f,	// first triangle
		 0.7f,  0.7f, 0.5f,	//

		 0.3f,  0.3f, 0.3f,	//
		-0.3f,  0.3f, 0.3f,	// second triangle
		-0.3f, -0.3f, 0.3f	//
	};

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	static const GLfloat g_vertex_color_data[] = {
		 1.0f, 0.0f, 0.0f, //
		 1.0f, 0.0f, 0.0f, // first triangle
		 1.0f, 0.0f, 0.0f, //

		 0.0f, 0.0f, 1.0f, //
		 0.0f, 0.0f, 1.0f, // second triangle
		 0.0f, 0.0f, 1.0f  //
	};

	GLuint vertexColors;
	glGenBuffers(1, &vertexColors);
	glBindBuffer(GL_ARRAY_BUFFER, vertexColors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_color_data), g_vertex_color_data, GL_STATIC_DRAW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);

	glm::vec3 cameraVec;

	do{

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Use our shader
		glUseProgram(programID);

		angle += step;
		cameraVec[0] = get_x(angle);
		cameraVec[1] = get_y(angle);
		cameraVec[2] = get_z(angle);
		view = glm::lookAt(
			cameraVec,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 0, 1)
		);
		MVP = projection * view * model;
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
			0,                  // attribute 0
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexColors);
		glVertexAttribPointer(
			1,                  // attribute 1
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 2*3); // 2*3 indices starting at 0 -> 2 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &vertexColors);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

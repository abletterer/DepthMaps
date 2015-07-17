// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

//// Include OpenCL
//#if defined(__APPLE__) || defined(__MACOSX)
//#include <OpenCL/cl.h>
//#else
//#include <CL/cl.hpp>
//#endif

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/matrix.hpp>
#include <common/utils.hpp>

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Depth Map to Point Cloud", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. "
						 "If you have an Intel GPU, they are not 3.3 compatible. "
						 "Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	ShaderProgram shader_program;
	shader_program.loadShader(GL_VERTEX_SHADER, "../shaders/StandardShading.vert");
	shader_program.loadShader(GL_FRAGMENT_SHADER, "../shaders/StandardShading.frag");

	ShaderProgram compute_program;
	compute_program.loadShader(GL_COMPUTE_SHADER, "../shaders/ComputeShading.cs");

//	GLuint shader_programID = shader_program.getProgramId();
	GLuint compute_programID = compute_program.getProgramId();

//	glUseProgram(compute_programID);

//	glUniform1i(glGetUniformLocation(compute_programID, "inTex"), 0);

//	glUseProgram(shader_programID);

//	// Get a handle for our "MVP" uniform
//	GLuint mvpID = glGetUniformLocation(shader_programID, "MVP");

	// Read our .obj file
	std::vector<GLfloat> depth_map;

	std::string str = getenv("HOME");
	str += "/Projets/Results/bunny/DepthMaps/256x256/bunny-DepthCamera-0-originalDepthMap.dat";

	// Load the texture

	int width, height;
	loadDepthMap(str, depth_map, width, height);

	int nb_points = 0;
	for(std::vector<GLfloat>::const_iterator it = depth_map.begin(); it != depth_map.end(); ++it)
	{
		if(fabs(1.f-*it) > FLT_EPSILON)
		{
			++nb_points;
		}
	}

//	GLuint textureID;

//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

//	glGenTextures(1, &textureID);
//	glBindTexture(GL_TEXTURE_2D, textureID);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
//					GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
//					GL_NEAREST);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,
//				 height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
//				 &depth_map[0]);

	// Load it into a VBO
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, nb_points * sizeof(vec4), NULL, GL_STATIC_DRAW);


	//	std::vector<glm::vec4> vertices(nb_points);
	GLuint counter = 0;

//	GLuint ac_buffer = 0;
//	glGenBuffers(1, &ac_buffer);
//	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ac_buffer);
//	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &counter, GL_DYNAMIC_DRAW);
//	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	counter = 1;	//Just another value


	glUseProgram(compute_programID);

//	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ac_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);
	glDispatchCompute(1, 1, 1);
//	glMemoryBarrier(GL_ALL_BARRIER_BITS);

//	glm::vec4* ptr = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
//	std::vector<glm::vec4> vertices(ptr, ptr+nb_points);
//	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
//	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

//	for(std::vector<glm::vec4>::const_iterator it = vertices.begin(); it != vertices.end(); ++it)
//	{
//		glm::vec4 vec = *it;
//		std::cout << vec.x << " | " << vec.y << " | " << vec.z << " | " << vec.w << std::endl;
//	}

//	do{
//		// Clear the screen
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//		// 1rst attribute buffer : vertices
//		glEnableVertexAttribArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glVertexAttribPointer(
//			0,                  // attribute
//			4,                  // size
//			GL_FLOAT,           // type
//			GL_FALSE,           // normalized?
//			0,                  // stride
//			(void*)0            // array buffer offset
//		);

//		// Draw the triangles !
//		glDrawArrays(GL_LINES, 0, vertices.size());

//		// Use our shader
//		glUseProgram(shader_programID);

//		// Compute the MVP matrix from keyboard and mouse input
//		computeMatricesFromInputs();
//		glm::mat4 ProjectionMatrix = getProjectionMatrix();
//		glm::mat4 ViewMatrix = getViewMatrix();
//		glm::mat4 ModelMatrix = glm::mat4(1.0);
//		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

//		// Send our transformation to the currently bound shader,
//		// in the "MVP" uniform
//		glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVP[0][0]);

//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glDisableVertexAttribArray(0);

//		// Swap buffers
//		glfwSwapBuffers(window);
//		glfwPollEvents();

//	} // Check if the ESC key was pressed or the window was closed
//	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
//		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
//	glDeleteBuffers(1, &vertexbuffer);
//	glDeleteProgram(shader_programID);
	glDeleteProgram(compute_programID);
//	glDeleteTextures(1, &textureID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


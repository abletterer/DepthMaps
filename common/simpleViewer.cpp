// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <chrono>

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
using namespace glm;

#include <QGLViewer/camera.h>

#include <common/shader.hpp>
#include <common/utils.hpp>

#include "simpleViewer.hpp"


using namespace std;

// Viewer::~Viewer()
// {
//   // Cleanup VBO and shader
//   glDeleteBuffers(1, &vertex_buffer);
//   glDeleteBuffers(1, &ac_buffer);
//   glDeleteProgram(render_programID);
//   glDeleteProgram(compute_programID);
//   glDeleteTextures(1, &textureID);
//   glDeleteVertexArrays(1, &VertexArrayID);
// }

void Viewer::draw()
{
   // Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	dmat4 mvp_matrix_d;
	this->camera()->getModelViewProjectionMatrix(value_ptr(mvp_matrix_d));

	mat4 mvp_matrix_o;

	for(int j = 0; j < 4; ++j)
	{
		for(int k = 0; k < 4; ++k)
		{
			mvp_matrix_o[j][k] = (GLfloat)mvp_matrix_d[j][k];
		}
	}

	// Use our shader
	glUseProgram(m_render_programID);

	glUniform1i(glGetUniformLocation(m_render_programID, "width"), m_width);
	glUniform1i(glGetUniformLocation(m_render_programID, "height"), m_height);

	for(int i = 0; i < m_index_buffers[i]; ++i)
	{

		mat4 mvp_matrix = mvp_matrix_o*m_mvp_matrices[i];

		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(glGetUniformLocation(m_render_programID, "MVP"), 1, GL_FALSE, value_ptr(mvp_matrix));  //&MVP[0][0]

		glBindVertexArray(m_vertex_arrays[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_index_buffers[i]);
//		glBindTexture(GL_TEXTURE_2D, m_textures[i]);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,			// Must match the layout location in the shader.
			2,			// size
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,			//stride
			(void*)0	// array buffer offset
		);

		glDrawArrays(GL_POINTS, 0, m_nb_points_buffers[i]);	//Launch OpenGL pipeline on those primitives
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void Viewer::init()
{
	// Opens help window
//	help();

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return;
	}

	this->camera()->setZClippingCoefficient(1000.f);

  // Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // Enable depth test
	glEnable(GL_DEPTH_TEST);

  // Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

  // Create and compile our GLSL program from the shaders
	ShaderProgram shader_program;
	shader_program.loadShader(GL_VERTEX_SHADER, "../shaders/StandardShading.vert");
	shader_program.loadShader(GL_FRAGMENT_SHADER, "../shaders/StandardShading.frag");

	ShaderProgram compute_program;
	compute_program.loadShader(GL_COMPUTE_SHADER, "../shaders/ComputeShading.cs");

	m_render_programID = shader_program.getProgramId();
	m_compute_programID = compute_program.getProgramId();

	std::vector<std::vector<GLfloat>> depth_maps;

	std::string str = getenv("HOME");
	str += "/Projets/Results/ramsesses/DepthMaps/1024x1024/";
//	str += "/Projets/Models/Kinect/";

	std::vector<GLfloat> params;

	std::cout << "Chargement des cartes de profondeur depuis le disque dur .." << std::flush;

	std::chrono::high_resolution_clock::time_point start_t = std::chrono::high_resolution_clock::now();

	loadSimulatedDepthMaps(str, depth_maps, m_mvp_matrices, m_width, m_height, ORIGINAL);
//	loadRealDepthMaps(str, depth_maps, m_mvp_matrices, m_width, m_height, params);

	std::chrono::high_resolution_clock::time_point end_t = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_t-start_t).count();

	std::cout << ".. fait en " << duration/1000 << " ms" << std::endl;

	std::cout << "Génération des VBO de chaque carte de profondeur .." << std::flush;

	start_t = std::chrono::high_resolution_clock::now();

	//Count the number of non-black points (points not belonging to the background);
	int nb_points_total = 0;
	m_nb_points_buffers.reserve(depth_maps.size());
	for(std::vector<std::vector<GLfloat>>::const_iterator it = depth_maps.begin(); it != depth_maps.end(); ++it )
	{
		std::vector<GLfloat> depth_map = *it;
		int nb_points = 0;
		for(std::vector<GLfloat>::const_iterator it2 = depth_map.begin(); it2 != depth_map.end(); ++it2)
		{
//			GLfloat value = *it2;
//			if(value > 0 && value < 1.f)
//			{
				++nb_points;
//			}
		}
		nb_points_total += nb_points;
		m_nb_points_buffers.push_back(nb_points);
	}

	glUseProgram(m_compute_programID);

	int counter = 0;

	GLuint ac_buffer = 0;
	glGenBuffers(1, &ac_buffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ac_buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &counter, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, ac_buffer);

	m_vertex_arrays.resize(depth_maps.size(), 0);
	m_index_buffers.resize(depth_maps.size(), 0);
	m_textures.resize(depth_maps.size(), 0);

	glUniform1i(glGetUniformLocation(m_compute_programID, "width"), m_width);
	glUniform1i(glGetUniformLocation(m_compute_programID, "height"), m_height);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	for(int i = 0; i < m_index_buffers.size(); ++i)
	{
		GLuint& vertex_array = m_vertex_arrays[i];
		GLuint& vertex_buffer = m_index_buffers[i];

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_width,
			m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
			&depth_maps[i][0]);

		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &counter, GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &vertex_array);
		glBindVertexArray(vertex_array);

		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);

		vec2* init_vec = new vec2[m_nb_points_buffers[i]];

		glBufferData(GL_SHADER_STORAGE_BUFFER, m_nb_points_buffers[i] * sizeof(vec2), NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_nb_points_buffers[i] * sizeof(vec2), init_vec);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertex_buffer);
		glDispatchCompute(m_width/16, m_height/16, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

//		GLuint* ptr = (GLuint*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

//		for(int j = 0; j < nb_points_buffers[i]; ++j)
//		{
//			std::cout << ptr[j] << std::endl;
//		}
//		std::cout << "-----------" << std::endl;
//		std::cout << "-----------" << std::endl;
//		std::cout << "-----------" << std::endl;

//		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		glBindVertexArray(0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	end_t = std::chrono::high_resolution_clock::now();

	duration = std::chrono::duration_cast<std::chrono::microseconds>(end_t-start_t).count();

	std::cout << ".. fait en " << duration/1000 << " ms" << std::endl;

	std::cout << "Nuage composé de " << nb_points_total << " point(s) répartis sur "
			  << depth_maps.size() << " cartes de profondeur de " << m_width << "x" << m_height << " pixels." << std::endl;
}

QString Viewer::helpString() const
{
	QString text("<h2>Help</h2>");
	text += "Use the mouse to move the camera around the object. ";
	text += "You can respectively revolve around, zoom and translate with the three mouse buttons. ";
	text += "Left and middle buttons pressed together rotate around the camera view direction axis<br><br>";
	text += "Pressing <b>Alt</b> and one of the function keys (<b>F1</b>..<b>F12</b>) defines a camera keyFrame. ";
	text += "Simply press the function key again to restore it. Several keyFrames define a ";
	text += "camera path. Paths are saved when you quit the application and restored at next start.<br><br>";
	text += "Press <b>F</b> to display the frame rate, <b>A</b> for the world axis, ";
	text += "<b>Alt+Return</b> for full screen mode and <b>Control+S</b> to save a snapshot. ";
	text += "See the <b>Keyboard</b> tab in this window for a complete shortcut list.<br><br>";
	text += "Double clicks automates single click actions: A left button double click aligns the closer axis with the camera (if close enough). ";
	text += "A middle button double click fits the zoom of the camera and the right button re-centers the scene.<br><br>";
	text += "A left button double click while holding right button pressed defines the camera <i>Revolve Around Point</i>. ";
	text += "See the <b>Mouse</b> tab and the documentation web pages for details.<br><br>";
	text += "Press <b>Escape</b> to exit the viewer.";
	return text;
}

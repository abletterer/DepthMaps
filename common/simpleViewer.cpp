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

#include <QKeyEvent>

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

	int size = 1<<m_level;

	// Use our shader
	glUseProgram(m_render_programID);

	glUniform1i(glGetUniformLocation(m_render_programID, "width"), m_width);
	glUniform1i(glGetUniformLocation(m_render_programID, "height"), m_height);

	glUniform1i(glGetUniformLocation(m_render_programID, "size"), size);

	for(int i = 0; i < m_index_buffers[i]; ++i)
	{
		mat4 mvp_matrix = mvp_matrix_o*m_mvp_matrices[i];

		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(glGetUniformLocation(m_render_programID, "MVP"), 1, GL_FALSE, value_ptr(mvp_matrix));  //&MVP[0][0]

		glBindVertexArray(m_vertex_arrays[i]);
		glBindBuffer(GL_ARRAY_BUFFER, m_index_buffers[i]);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,			// Must match the layout location in the shader.
			2,			// size
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			sizeof(vec2)*size,			//stride
			(void*)0	// array buffer offset
		);

		glDrawArrays(GL_POINTS, 0, m_nb_points_buffers[i]/size);	//Launch OpenGL pipeline on those primitives
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	QGLViewer::draw();
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
//	glEnable(GL_CULL_FACE);

  // Create and compile our GLSL program from the shaders
	ShaderProgram shader_program;
	shader_program.loadShader(GL_VERTEX_SHADER, "../shaders/StandardShading.vert");
	shader_program.loadShader(GL_FRAGMENT_SHADER, "../shaders/StandardShading.frag");

	ShaderProgram compute_program;
	compute_program.loadShader(GL_COMPUTE_SHADER, "../shaders/ComputeShading.cs");

	ShaderProgram lifting_program;
	lifting_program.loadShader(GL_COMPUTE_SHADER, "../shaders/LiftingScheme.cs");

	m_render_programID = shader_program.getProgramId();
	m_compute_programID = compute_program.getProgramId();
	m_compute_lifting_programID = lifting_program.getProgramId();

	std::vector<std::vector<GLfloat>> depth_maps;

	std::string str = getenv("HOME");
	str += "/Projets/Results/ramsesses/DepthMaps/1024x1024/";
//	str += "/Projets/Models/Kinect/";

	std::cout << "Chargement des cartes de profondeur depuis le disque dur .." << std::flush;

	std::chrono::high_resolution_clock::time_point start_t = std::chrono::high_resolution_clock::now();

//	std::vector<GLfloat> params;

	loadSimulatedDepthMaps(str, depth_maps, m_mvp_matrices, m_width, m_height, ORIGINAL);
//	loadRealDepthMaps(str, depth_maps, m_mvp_matrices, m_width, m_height, params);

	m_level = 0;

	std::chrono::high_resolution_clock::time_point end_t = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_t-start_t).count();

	std::cout << ".. fait en " << duration/1000 << " ms" << std::endl;

	std::cout << "Génération des VBO représentant chaque carte de profondeur .." << std::flush;

	start_t = std::chrono::high_resolution_clock::now();

	m_nb_points_buffers.reserve(depth_maps.size());
	int nb_points = m_width*m_height;

	for(std::vector<std::vector<GLfloat>>::const_iterator it = depth_maps.begin(); it != depth_maps.end(); ++it )
	{
		m_nb_points_buffers.push_back(nb_points);
	}

	glUseProgram(m_compute_programID);

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

//		vec2* ptr = (vec2*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

//		for(int j = 0; j < m_nb_points_buffers[i]; ++j)
//		{
//			std::cout << ptr[j].x << " | " << ptr[j].y << std::endl;
//		}
//		std::cout << "-----------" << std::endl;
//		std::cout << "-----------" << std::endl;
//		std::cout << "-----------" << std::endl;

//		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		glBindVertexArray(0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	end_t = std::chrono::high_resolution_clock::now();

	duration = std::chrono::duration_cast<std::chrono::microseconds>(end_t-start_t).count();

	std::cout << ".. fait en " << duration/1000 << " ms" << std::endl;

	std::cout << "Nuage composé de " << depth_maps.size() << " cartes de profondeur de " << m_width << "x" << m_height << " pixels." << std::endl;
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

void Viewer::decompose()
{
	int size = 1<<m_level;

	if(size < m_width && size < m_height)
	{
		glUseProgram(m_compute_lifting_programID);

		glUniform1i(glGetUniformLocation(m_compute_lifting_programID, "width"), m_width);
		glUniform1i(glGetUniformLocation(m_compute_lifting_programID, "height"), m_height);

		glUniform1i(glGetUniformLocation(m_compute_lifting_programID, "size"), size);
		glUniform1i(glGetUniformLocation(m_compute_lifting_programID, "to_decompose"), true);

		for(int i = 0; i < m_index_buffers.size(); ++i)
		{
			GLuint& vertex_array = m_vertex_arrays[i];
			GLuint& vertex_buffer = m_index_buffers[i];

			glBindVertexArray(vertex_array);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertex_buffer);

			glUniform1i(glGetUniformLocation(m_compute_lifting_programID, "horizontal"), true);

			glDispatchCompute(m_width/16, m_height/16, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);

			glUniform1i(glGetUniformLocation(m_compute_lifting_programID, "horizontal"), false);

			glDispatchCompute(m_width/16, m_height/16, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
		}

		glBindVertexArray(0);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		m_level++;
	}
}

void Viewer::reconstruct()
{
	int size = 1<<m_level;

	if(m_level > 0)
	{
		glUseProgram(m_compute_lifting_programID);

		glUniform1i(glGetUniformLocation(m_compute_lifting_programID, "width"), m_width);
		glUniform1i(glGetUniformLocation(m_compute_lifting_programID, "height"), m_height);

		glUniform1i(glGetUniformLocation(m_compute_lifting_programID, "to_decompose"), false);
		glUniform1i(glGetUniformLocation(m_compute_lifting_programID, "size"), size);

		for(int i = 0; i < m_index_buffers.size(); ++i)
		{
			GLuint& vertex_array = m_vertex_arrays[i];
			GLuint& vertex_buffer = m_index_buffers[i];

			glBindVertexArray(vertex_array);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertex_buffer);

			glDispatchCompute(m_width/16, m_height/16, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
		}

		glBindVertexArray(0);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		m_level--;
	}
}

void Viewer::keyPressEvent(QKeyEvent* event)
{
	switch(event->key())
	{
		case Qt::Key_D:
			std::cout << "Décomposition .." << std::flush;
			decompose();
			std::cout << ".. fait (Niveau " << m_level << ")" << std::endl;
			updateGL();
		break;
		case Qt::Key_R:
			std::cout << "Reconstruction .." << std::flush;
			reconstruct();
			std::cout << ".. fait (Niveau " << m_level << ")" << std::endl;
			updateGL();
		break;
		default:
			QGLViewer::keyPressEvent(event);
		break;
	}
}

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

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

// Draws a spiral
void Viewer::draw()
{
   // Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);

   // Draw the triangles !
	glDrawArrays(GL_POINTS, 0, nb_points);

   // Use our shader
	glUseProgram(render_programID);

   // Send our transformation to the currently bound shader,
   // in the "MVP" uniform
	GLdouble mvp_matrix[16];
	this->camera()->getModelViewMatrix(mvp_matrix);

	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			std::cout << mvp_matrix[i*4+j] << " " << std::flush;
		}
		std::cout << std::endl;
	}

   glUniformMatrix4fv(glGetUniformLocation(render_programID, "MVP"), 1, GL_FALSE, (float*) &mvp_matrix[0]);  //&MVP[0][0]

   glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Viewer::init()
{
  // Restore previous viewer state.
	restoreStateFromFile();

  // Opens help window
	help();

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) 
  {
  	fprintf(stderr, "Failed to initialize GLEW\n");
  	return;
  }

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

  render_programID = shader_program.getProgramId();
  compute_programID = compute_program.getProgramId();

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  glUseProgram(render_programID);

  std::vector<GLfloat> depth_map;

  std::string str = getenv("HOME");
  str += "/Projets/Results/bunny/DepthMaps/512x512/bunny-DepthCamera-0-originalDepthMap.dat";

  int width, height;
  loadDepthMap(str, depth_map, width, height);

  //Count the number of non-black points (points not belonging to the background)
  nb_points = 0;
  for(std::vector<GLfloat>::const_iterator it = depth_map.begin(); it != depth_map.end(); ++it)
  {
  	GLfloat value = *it;
  	if(fabs(1.f-value) > FLT_EPSILON)
  	{
  		++nb_points;
  	}
  }

  glUseProgram(compute_programID);

  glUniform1i(glGetUniformLocation(compute_programID, "width"), width);
  glUniform1i(glGetUniformLocation(compute_programID, "height"), height);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  GLuint textureID = 0;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
  	GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
  	GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width,
  	height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
  	&depth_map[0]);

  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, nb_points * sizeof(vec4), NULL, GL_STATIC_DRAW);

  int counter = 0;

  GLuint ac_buffer = 0;
  glGenBuffers(1, &ac_buffer);
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ac_buffer);
  glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &counter, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

  glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, ac_buffer);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertex_buffer);

  glDispatchCompute(width/16, height/16, 1);
  glMemoryBarrier(GL_ALL_BARRIER_BITS);

  glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
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
#include <QGLViewer/qglviewer.h>

#include <glm/glm.hpp>

class Viewer : public QGLViewer
{
	protected :
	virtual void draw();
	virtual void init();
	virtual QString helpString() const;

	private :
	GLuint render_programID;
	GLuint compute_programID;
	std::vector<GLuint> vertex_arrays;
	std::vector<GLuint> vertex_buffers;
	std::vector<int> nb_points_buffers;
	std::vector<glm::mat4> mvp_matrices;

};

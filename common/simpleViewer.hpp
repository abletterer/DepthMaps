#include <QGLViewer/qglviewer.h>

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
	GLfloat mvp_matrix_f[16];

};

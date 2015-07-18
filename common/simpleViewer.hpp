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
	GLuint vertex_buffer;
	int nb_points;

};
#include <QGLViewer/qglviewer.h>

#include <glm/glm.hpp>

class Viewer : public QGLViewer
{

	protected :
	virtual void draw();
	virtual void init();
	virtual QString helpString() const;

	private :
	GLuint m_render_programID;
	GLuint m_compute_programID;
	int m_width;
	int m_height;
	std::vector<GLuint> m_vertex_arrays;
	std::vector<GLuint> m_index_buffers;
	std::vector<GLuint> m_textures;
	std::vector<int> m_nb_points_buffers;
	std::vector<glm::mat4> m_mvp_matrices;

};

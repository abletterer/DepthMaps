#ifndef SHADER_HPP
#define SHADER_HPP

class ShaderProgram {
    GLuint m_program_id;
    
public:
    
    ShaderProgram();
    ~ShaderProgram();

	void loadShader(GLenum shaderType, const char* file_path);
    
    void loadShaders(const char* vertex_file_path, const char* fragment_file_path);
    
    GLuint getProgramId() { return m_program_id; }
};

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

#endif

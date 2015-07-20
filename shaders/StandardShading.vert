#version 430 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertex_in;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vertex_in;
}

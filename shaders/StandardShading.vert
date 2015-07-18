#version 430 core

// Input vertex data, different for all executions of this shader.
layout (std430, binding=0)
buffer vertex_buffer { vec4 vertex[]; };

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vertex[gl_VertexID];
}

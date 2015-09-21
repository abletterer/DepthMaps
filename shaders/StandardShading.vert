#version 430 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 pixel;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform int width;
uniform int height;

void main(){
	float image_x = pixel.x/(width-1);
	float image_y = mod(pixel.x,(width-1));

	float x = float(image_x)/float(width-1);
	float y = float(image_y)/float(height-1);

	float depth = pixel.y;

	if(depth > 0 && depth < 1)
	{
		vec4 vertex_out = vec4(x*2-1, y*2-1, depth, 1);
		gl_Position =  MVP * vertex_out;
	}
	else
	{
		gl_Position = vec4(2, 0, 0, 1);	//Put the point outside of the view frustum (clip coordinates \in [-1;1])
	}
}

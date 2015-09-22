#version 430 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 pixel;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform int width;
uniform int height;

uniform int size;

void main(){
	int image_x = int(int(pixel.x)/width);
	int image_y = int(mod(int(pixel.x),width));

	if(mod(image_x, size)==0 && mod(image_y, size)==0)
	{
		float x = float(image_x)/float(width);
		float y = float(image_y)/float(height);

		float depth = pixel.y;

		if(depth > 0 && depth < 1)
		{
			vec4 vertex_out = vec4(x*2-1, y*2-1, depth, 1);
			gl_Position =  MVP * vertex_out;
		}
		else
		{
			gl_Position = vec4(0, 0, -2, 1);	//Put the point outside of the view frustum (clip coordinates \in [-1;1])
		}
	}
	else
	{
		gl_Position = vec4(0, 0, -2, 1);	//Put the point outside of the view frustum (clip coordinates \in [-1;1])
	}
}

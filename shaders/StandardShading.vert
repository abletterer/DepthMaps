#version 430 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in uint index;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform sampler2D in_tex;

uniform int width;
uniform int height;

#define dc1 -0.0030711016
#define dc2 3.3309495161

void main(){
	float image_x = index/width;
	float image_y = mod(index,width);

	float x = float(image_x)/float(width-1);
	float y = float(image_y)/float(height-1);

	float depth = texture2D(in_tex, vec2(x, -y)).x;
//	float depth = 1-texture2D(in_tex, vec2(x, -y)).x;
//	depth = 1/(depth*dc1-dc2);

	if(depth > 0 && depth < 1)
	{
//		vec4 vertex_out = vec4(image_x*depth, image_y*depth, depth, 1);
		vec4 vertex_out = vec4(x*2-1, y*2-1, depth, 1);
		gl_Position =  MVP * vertex_out;
	}
	else
	{
		gl_Position = vec4(2, 0, 0, 1);	//Put the point outside of the view frustum (clip coordinates \in [-1;1])
	}
}

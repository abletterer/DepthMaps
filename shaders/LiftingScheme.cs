#version 430 core

layout (std430, binding=0) buffer depth_buffer { vec2 pixel[]; };

layout (binding=0) uniform sampler2D in_tex;

uniform int width;
uniform int height;

uniform bool decompose;

uniform int level;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void decompose(int x, int y)
{
	int index = x*width+y;
	if(x%(2*(level+1))==1)
	{
		//Horizontal
		if(x+1==height)
		{
			pixel[index].y = (pixel[index].y-pixel[(x-1)*width+y].y)/2.;
		}
		else
		{
			float pair_1 = pixel[(x-1)*width+y].y;
			float pair_2 = pixel[(x+1)*width+y].y;
			pixel[index].y = (pixel[index].y-(pair_1+pair_2)/2.)/2.;
		}
	}

	if(y%(2*(level+1))==1)
	{
		//Vertical
		if(y+1==width)
		{
			pixel[index].y = (pixel[index].y-pixel[x*width+(y-1)].y)/2.;
		}
		else
		{
			float pair_1 = pixel[x*width+(y-1)].y;
			float pair_2 = pixel[x*width+(y+1)].y;
			pixel[index].y = (pixel[index].y-(pair_1+pair_2)/2.)/2.;
		}
	}
}

void reconstruct(int x, int y)
{
	int index = x*width+y;
	if(x%(2*(level+1))==1)
	{
		//Horizontal
		if(x+1==height)
		{
			pixel[index].y = pixel[index].y*2 + pixel[(x-1)*width+y].y;
		}
		else
		{
			float pair_1 = pixel[(x-1)*width+y].y;
			float pair_2 = pixel[(x+1)*width+y].y;
			pixel[index].y = pixel[index].y*2 + (pair_1+pair_2)/2.;
		}
	}

	if(y%(2*(level+1))==1)
	{
		//Vertical
		if(y+1==width)
		{
			pixel[index].y = pixel[index].y*2 + pixel[x*width+(y-1)].y/2.;
		}
		else
		{
			float pair_1 = pixel[x*width+(y-1)].y;
			float pair_2 = pixel[x*width+(y+1)].y;
			pixel[index].y = pixel[index].y*2 + (pair_1+pair_2)/2.;
		}
	}
}

void main() {
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;

	if(decompose)
	{
		decompose(x,y);
	}
	else
	{
		reconstruct(x,y)
	}
}

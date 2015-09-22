#version 430 core

layout (std430, binding=0) buffer depth_buffer { vec2 pixel[]; };

layout (binding=0) uniform sampler2D in_tex;

uniform int width;
uniform int height;

uniform int size;
uniform bool to_decompose;
uniform bool horizontal;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void decompose(in uint x, in uint y)
{
	uint index = x*width+y;

	if(horizontal)
	{
		//Horizontal
//		if(mod(x,size*2)!=0)
//		{
//			if(x+size>=height)
//			{
//				pixel[index].y = (pixel[index].y-pixel[(x-size)*width+y].y)/2.;
//			}
//			else
//			{
//				float pair_1 = pixel[(x-size)*width+y].y;
//				float pair_2 = pixel[(x+size)*width+y].y;
//				pixel[index].y = (pixel[index].y-(pair_1+pair_2)/2.)/2.;
//			}
//		}
	}
	else
	{
		//Vertical
		if(mod(y,size*2)!=0)
		{
//			if(y+size>=width)
//			{
//				pixel[index].y = (pixel[index].y-pixel[x*width+(y-size)].y)/2.;
//			}
//			else
//			{
//				float pair_1 = pixel[x*width+(y-size)].y;
//				float pair_2 = pixel[x*width+(y+size)].y;
//				pixel[index].y = (pixel[index].y-(pair_1+pair_2)/2.)/2.;
//			}
		}
	}
}

void reconstruct(in uint x, in uint y)
{
	uint index = x*width+y;

	//Horizontal
	if(mod(x,size)!=0)
	{
		if(x+size==height)
		{
			pixel[index].y = pixel[index].y*2 + pixel[(x-size)*width+y].y;
		}
		else
		{
			float pair_1 = pixel[(x-size)*width+y].y;
			float pair_2 = pixel[(x+size)*width+y].y;
			pixel[index].y = pixel[index].y*2 + (pair_1+pair_2)/2.;
		}
	}

	//Vertical
	if(mod(y,size)!=0)
	{
		if(y+size==width)
		{
			pixel[index].y = pixel[index].y*2 + pixel[x*width+(y-size)].y/2.;
		}
		else
		{
			float pair_1 = pixel[x*width+(y-size)].y;
			float pair_2 = pixel[x*width+(y+size)].y;
			pixel[index].y = pixel[index].y*2 + (pair_1+pair_2)/2.;
		}
	}
}

void main()
{
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;

	if(to_decompose)
	{
		decompose(x,y);
	}
	else
	{
//		reconstruct(x,y);
	}
}

#version 430 core

layout (std430, binding=0) buffer depth_buffer { vec2 pixel[]; };

uniform int width;
uniform int height;

uniform int size;
uniform bool to_decompose;
uniform bool horizontal;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void decompose()
{
	uint index = uint(gl_GlobalInvocationID.x*width+gl_GlobalInvocationID.y);
	int image_index = int(pixel[index].x);

	int x = int(image_index)/width;
	int y = int(mod(int(image_index),width));

	if(horizontal)
	{
		//Horizontal
		if(mod(x,size*2)!=0)
		{
			//Si l'élément courant est un impair
//			if((x+1)>=height)
//			{
//				pixel[image_index].y = (pixel[image_index].y-pixel[(x-1)*width+y].y)/2.;
//			}
//			else
//			{
//				float pair_1 = pixel[(x-1)*width+y].y;
//				float pair_2 = pixel[(x+1)*width+y].y;
////				pixel[image_index].y = (pixel[image_index].y-(pair_1+pair_2)/2.)/2.;
//			}
			pixel[image_index].y = 2;
		}
	}
	else
	{
		//Vertical
		if(mod(y,size*2)!=0)
		{
			//Si l'élément courant est un impair
//			if((y+1)>=width)
//			{
//				pixel[image_index].y = (pixel[image_index].y-pixel[x*width+(y-1)].y)/2.;
//			}
//			else
//			{
//				float pair_1 = pixel[x*width+(y-1)].y;
//				float pair_2 = pixel[x*width+(y+1)].y;
////				pixel[image_index].y = (pixel[image_index].y-(pair_1+pair_2)/2.)/2.;
//			}
			pixel[image_index].y = 2;
		}
	}
}

//void reconstruct(in uint x, in uint y)
//{
//	uint index = x*width+y;

//	//Horizontal
//	if(mod(x,size)!=0)
//	{
//		if(x+size==height)
//		{
//			pixel[index].y = pixel[index].y*2 + pixel[(x-size)*width+y].y;
//		}
//		else
//		{
//			float pair_1 = pixel[(x-size)*width+y].y;
//			float pair_2 = pixel[(x+size)*width+y].y;
//			pixel[index].y = pixel[index].y*2 + (pair_1+pair_2)/2.;
//		}
//	}

//	//Vertical
//	if(mod(y,size)!=0)
//	{
//		if(y+size==width)
//		{
//			pixel[index].y = pixel[index].y*2 + pixel[x*width+(y-size)].y/2.;
//		}
//		else
//		{
//			float pair_1 = pixel[x*width+(y-size)].y;
//			float pair_2 = pixel[x*width+(y+size)].y;
//			pixel[index].y = pixel[index].y*2 + (pair_1+pair_2)/2.;
//		}
//	}
//}

void main()
{
	if(to_decompose)
	{
//		decompose();
	}
	else
	{
//		reconstruct();
	}
}

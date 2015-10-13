#version 430 core

layout (std430, binding=0) buffer depth_buffer { vec2 pixel[]; };
layout (std430, binding=1) buffer copy_buffer { vec2 copy_pixel[]; };

uniform int width;
uniform int height;

uniform int size;
uniform bool to_decompose;
uniform bool horizontal;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

//Calcul du modulo car la fonction mod retourne un flottant
int modulo(int dividend, int divisor)
{
	return dividend - dividend/divisor*divisor;
}


void decompose()
{
	uint index = uint(copy_pixel[gl_GlobalInvocationID.x*width+gl_GlobalInvocationID.y].x);

	int x = int(index)/width;
	int y = modulo(int(index),width);

	if(horizontal)
	{
		if(modulo(x,size)==0)
		{
			pixel[(x/2)*width+y].x = (x/2)*width+y;
			pixel[(x/2)*width+y].y = copy_pixel[x*width+y].y;
		}
		else
		{
			//Si l'élément courant est un impair
//			if((x+1)>=height)
//			{
//				//Au bord de l'image on considère un effet miroir
//				pixel[(height/2+x/2)*width+y].y = (copy_pixel[index].y-copy_pixel[(x-1)*width+y].y)/2.;
//			}
//			else
//			{
//				float pair_1 = copy_pixel[(x-1)*width+y].y;
//				float pair_2 = copy_pixel[(x+1)*width+y].y;
//				pixel[(height/2+x/2)*width+y].y = (copy_pixel[index].y-(pair_1+pair_2)/2.)/2.;
//			}
		}
	}
	else
	{
		if(modulo(y,size)==0)
		{
			pixel[x*width+y/2].x = x*width+y/2;
			pixel[x*width+y/2].y = copy_pixel[x*width+y].y;
		}
		else
		{
			//Si l'élément courant est un impair
//			if((y+1)>=width)
//			{
//				//Au bord de l'image on considère un effet miroir
//				pixel[x*width+(width/2+y/2)].y = (copy_pixel[index].y-copy_pixel[x*width+(y-1)].y)/2.;
//			}
//			else
//			{
//				float pair_1 = copy_pixel[x*width+(y-1)].y;
//				float pair_2 = copy_pixel[x*width+(y+1)].y;
//				pixel[x*width+(width/2+y/2)].y = (copy_pixel[index].y-(pair_1+pair_2)/2.)/2.;
//			}
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
		decompose();
	}
	else
	{
//		reconstruct();
	}
}

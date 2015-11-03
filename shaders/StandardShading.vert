#version 430 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 pixel;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform vec3 camera_position;
uniform int width;
uniform int height;

uniform int size;

//Calcul du modulo car la fonction mod retourne un flottant
int modulo(int dividend, int divisor)
{
    return dividend - dividend/divisor*divisor;
}

void main(){
	int image_x = int(pixel.x)/width;
        int image_y = int(pixel.x)-image_x*width;

	float depth = pixel.y;

        if(depth > 0 && depth < 1)
        {
            float x = float(image_x)/float(width-1)*2-1;
            float y = float(image_y)/float(height-1)*2-1;

            vec4 vertex_out = vec4(x, y, depth, 1);
            gl_Position =  MVP * vertex_out;
        }
        else
        {
            gl_Position = vec4(0, 0, 10, 1);	//Put the point outside of the view frustum (clip coordinates \in [-1;1])
        }
}

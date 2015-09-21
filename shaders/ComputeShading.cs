#version 430 core

layout (std430, binding=0) buffer depth_buffer { vec2 pixel[]; };

layout (binding=0) uniform atomic_uint cur_index;

layout (binding=0) uniform sampler2D in_tex;

uniform int width;
uniform int height;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main() {
//	uint index = atomicCounterIncrement(cur_index);

	vec2 tex_coords = vec2(float(gl_GlobalInvocationID.x)/float(width), float(gl_GlobalInvocationID.y)/float(height));
	float depth = texture(in_tex, vec2(tex_coords.x, -tex_coords.y)).r;	//z in [-1;1]
	uint index = gl_GlobalInvocationID.x*width+gl_GlobalInvocationID.y;
	pixel[index] = vec2(index, depth);
}

#version 430 core

layout (std430, binding=0) buffer indices_buffer { uint indices[]; };

layout (binding=0) uniform atomic_uint cur_index;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main() {
	uint index = atomicCounterIncrement(cur_index);
	indices[index] = index;
}

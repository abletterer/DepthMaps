#ifndef KERNEL_UTILS_HPP
#define KERNEL_UTILS_HPP

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <iostream>

void checkResult(const cl_int result)
{
	if(result != CL_SUCCESS)
	{
		std::cerr << "OpenCL error : ";
		switch (result) {
			case CL_DEVICE_NOT_FOUND:
				std::cerr << "Device not found.";
			break;
			case CL_DEVICE_NOT_AVAILABLE:
				std::cerr << "Device not available";
			break;
			case CL_COMPILER_NOT_AVAILABLE:
				std::cerr << "Compiler not available";
			break;
			case CL_MEM_OBJECT_ALLOCATION_FAILURE:
				std::cerr << "Memory object allocation failure";
			break;
			case CL_OUT_OF_RESOURCES:
				std::cerr << "Out of resources";
			break;
			case CL_OUT_OF_HOST_MEMORY:
				std::cerr << "Out of host memory";
			break;
			case CL_PROFILING_INFO_NOT_AVAILABLE:
				std::cerr << "Profiling information not available";
			break;
			case CL_MEM_COPY_OVERLAP:
				std::cerr << "Memory copy overlap";
			break;
			case CL_IMAGE_FORMAT_MISMATCH:
				std::cerr << "Image format mismatch";
			break;
			case CL_IMAGE_FORMAT_NOT_SUPPORTED:
				std::cerr << "Image format not supported";
			break;
			case CL_BUILD_PROGRAM_FAILURE:
				std::cerr << "Program build failure";
			break;
			case CL_MAP_FAILURE:
				std::cerr << "Map failure";
			break;
			case CL_INVALID_VALUE:
				std::cerr << "Invalid value";
			break;
			case CL_INVALID_DEVICE_TYPE:
				std::cerr << "Invalid device type";
			break;
			case CL_INVALID_PLATFORM:
				std::cerr << "Invalid platform";
			break;
			case CL_INVALID_DEVICE:
				std::cerr << "Invalid device";
			break;
			case CL_INVALID_CONTEXT:
				std::cerr << "Invalid context";
			break;
			case CL_INVALID_QUEUE_PROPERTIES:
				std::cerr << "Invalid queue properties";
			break;
			case CL_INVALID_COMMAND_QUEUE:
				std::cerr << "Invalid command queue";
			break;
			case CL_INVALID_HOST_PTR:
				std::cerr << "Invalid host pointer";
			break;
			case CL_INVALID_MEM_OBJECT:
				std::cerr << "Invalid memory object";
			break;
			case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
				std::cerr << "Invalid image format descriptor";
			break;
			case CL_INVALID_IMAGE_SIZE:
				std::cerr << "Invalid image size";
			break;
			case CL_INVALID_SAMPLER:
				std::cerr << "Invalid sampler";
			break;
			case CL_INVALID_BINARY:
				std::cerr << "Invalid binary";
			break;
			case CL_INVALID_BUILD_OPTIONS:
				std::cerr << "Invalid build options";
			break;
			case CL_INVALID_PROGRAM:
				std::cerr << "Invalid program";
			break;
			case CL_INVALID_PROGRAM_EXECUTABLE:
				std::cerr << "Invalid program executable";
			break;
			case CL_INVALID_KERNEL_NAME:
				std::cerr << "Invalid kernel name";
			break;
			case CL_INVALID_KERNEL_DEFINITION:
				std::cerr << "Invalid kernel definition";
			break;
			case CL_INVALID_KERNEL:
				std::cerr << "Invalid kernel";
			break;
			case CL_INVALID_ARG_INDEX:
				std::cerr << "Invalid argument index";
			break;
			case CL_INVALID_ARG_VALUE:
				std::cerr << "Invalid argument value";
			break;
			case CL_INVALID_ARG_SIZE:
				std::cerr << "Invalid argument size";
			break;
			case CL_INVALID_KERNEL_ARGS:
				std::cerr << "Invalid kernel arguments";
			break;
			case CL_INVALID_WORK_DIMENSION:
				std::cerr << "Invalid work dimension";
			break;
			case CL_INVALID_WORK_GROUP_SIZE:
				std::cerr << "Invalid work group size";
			break;
			case CL_INVALID_WORK_ITEM_SIZE:
				std::cerr << "Invalid work item size";
			break;
			case CL_INVALID_GLOBAL_OFFSET:
				std::cerr << "Invalid global offset";
			break;
			case CL_INVALID_EVENT_WAIT_LIST:
				std::cerr << "Invalid event wait list";
			break;
			case CL_INVALID_EVENT:
				std::cerr << "Invalid event";
			break;
			case CL_INVALID_OPERATION:
				std::cerr << "Invalid operation";
			break;
			case CL_INVALID_GL_OBJECT:
				std::cerr << "Invalid OpenGL object";
			break;
			case CL_INVALID_BUFFER_SIZE:
				std::cerr << "Invalid buffer size";
			break;
			case CL_INVALID_MIP_LEVEL:
				std::cerr << "Invalid mip-map level";
			break;
		}
		std::cerr << std::endl;
		exit(-1);
	}
}

#endif // KERNEL_UTILS_HPP


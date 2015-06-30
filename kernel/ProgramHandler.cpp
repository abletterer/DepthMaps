#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include "kernel/kernel_utils.hpp"

#define MEM_SIZE (13)
#define MAX_SOURCE_SIZE (0x100000)

class ProgramHandler
{
public:
	ProgramHandler()
	{
		std::vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);
		if(all_platforms.size() == 0)
		{
			std::cout << " No platforms found. Check OpenCL installation!" << std::endl;
			exit(-1);
		}
		m_platform = all_platforms[0];

//		m_platform = cl::Platform::getDefault(&ret);
//		checkResult(ret);

		std::vector<cl::Device> all_devices;
		m_platform.getDevices(CL_DEVICE_TYPE_DEFAULT, &all_devices);
		if(all_devices.size() == 0)
		{
			std::cout << " No devices found. Check OpenCL installation!" << std::endl;
			exit(-1);
		}
		m_device = all_devices[0];

		// Create OpenCL context
		m_context = cl::Context(m_device);

		m_command_queue = cl::CommandQueue(m_context, m_device);
	}

	void addKernel(const std::string& file_name, const std::string& kernel_name)
	{
		std::ifstream ifs;
		ifs.open(file_name.c_str(), std::ios::in);
		if (!ifs.is_open())
		{
			std::cout << "Impossible to open '" << file_name << "'" << std::endl;
		}

		std::string kernel_str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

		//If the kernel has not already been added
		m_kernels[kernel_name] = kernel_str;
		m_program_sources.push_back({m_kernels[kernel_name].c_str(), kernel_str.size()});
	}

	void buildProgram()
	{
		if(m_program_sources.size() > 0)
		{
			m_program = cl::Program(m_context, m_program_sources);
			checkResult(m_program.build({m_device}));
		}
	}

	cl::Platform& getPlatform() { return m_platform; }
	cl::Device& getDevice() { return m_device; }
	cl::Context& getContext() { return m_context; }
	cl::Program& getProgram() { return m_program; }
	cl::CommandQueue& getCommandQueue() { return m_command_queue; }

private:
	cl::Platform m_platform;
	cl::Device m_device;
	cl::Context m_context;
	cl::Program m_program;
	cl::Program::Sources m_program_sources;
	cl::CommandQueue m_command_queue;
	std::unordered_map<std::string, std::string> m_kernels;
};

int main()
{
	cl_int err;

	ProgramHandler program_handler;

	program_handler.addKernel("../kernel/hello.cl", "hello");

	program_handler.buildProgram();

	cl::CommandQueue& queue = program_handler.getCommandQueue();

	cl::Buffer device_buffer(program_handler.getContext(), CL_MEM_READ_WRITE, MEM_SIZE*sizeof(char), NULL, &err);
	checkResult(err);

	cl::Kernel hello(program_handler.getProgram(), "hello", &err);
	checkResult(err);

	err = hello.setArg(0, device_buffer);
	checkResult(err);

	err = queue.enqueueNDRangeKernel(hello, cl::NullRange, cl::NDRange(10), cl::NullRange);
	checkResult(err);

	std::string string;
	string.resize(MEM_SIZE);
	err = queue.enqueueReadBuffer(device_buffer, CL_TRUE, 0, MEM_SIZE*sizeof(char), &string[0]);
	checkResult(err);

	std::cout << string << std::endl;

	return 0;
}

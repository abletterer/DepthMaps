# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (DepthMaps)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(GLM REQUIRED)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
	message( FATAL_ERROR "Please select another Build Directory !" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()



# Compile external dependencies
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-1c73e35")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

include_directories(
	${OPENCL_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
	external/glfw-3.0.3/include/GLFW/
	external/glew-1.9.0/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	${OPENCL_LIBRARIES}
	GLFW_303
	GLEW_190
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# DepthMapToPointCloud
# file(
# 	GLOB
# 	SRC_FILES
# 	"${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
# 	"${CMAKE_CURRENT_SOURCE_DIR}/common/*.hpp"
# 	"${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
# 	"${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
# )

add_executable(DepthMapToPointCloud
	# DepthMapToPointCloud/DepthMapToPointCloud.cpp
	common/hello.c
	# ${SRC_FILES}
)
target_link_libraries(DepthMapToPointCloud
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(DepthMapToPointCloud PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/DepthMapToPointCloud/")
create_target_launcher(DepthMapToPointCloud WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/DepthMapToPointCloud/")


SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET DepthMapToPointCloud POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/DepthMapToPointCloud${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/DepthMapToPointCloud/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )


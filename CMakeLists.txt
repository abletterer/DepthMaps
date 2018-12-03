# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (DepthMaps)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(Qt4 REQUIRED)
find_package(QGLViewer REQUIRED)
find_package(Boost COMPONENTS filesystem regex system REQUIRED)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
	message( FATAL_ERROR "Please select another Build Directory !" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

include_directories(
	${OPENGL_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
	${QT_INCLUDE_DIR}
	${QGLVIEWER_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS}
	.
)

INCLUDE(${QT_USE_FILE})

set(ALL_LIBS
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${QT_LIBRARIES}
	${QGLVIEWER_LIBRARY}
	${Boost_LIBRARIES}
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
	${QT_DEFINITIONS}
)

#DepthMapToPointCloud
file(
	GLOB
	SRC_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
)

file(
	GLOB
	INCLUDE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/common/*.hpp"
)
QT4_WRAP_CPP(INCLUDE_FILES_MOC ${INCLUDE_FILES})

file(
	GLOB
	SHADER_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
	"${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
	"${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.cs"
)


add_executable(DepthMapToPointCloud
	"${CMAKE_CURRENT_SOURCE_DIR}/DepthMapToPointCloud/DepthMapToPointCloud.cpp"
	${SRC_FILES}
	${INCLUDE_FILES_MOC}
	${INCLUDE_FILES}
	${SHADER_FILES}
)
#target_compile_features(DepthMapToPointCloud
#	PRIVATE
#	cxx_range_for
#)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

target_link_libraries(DepthMapToPointCloud
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(DepthMapToPointCloud PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/DepthMapToPointCloud/")

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
	TARGET DepthMapToPointCloud POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	"${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/DepthMapToPointCloud${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/DepthMapToPointCloud/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/local/bin/cmake

# The command to remove a file.
RM = /opt/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/bletterer/Projets/DepthMaps

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/bletterer/Projets/DepthMaps/build

# Utility rule file for docs.

# Include the progress variables for this target.
include external/glfw-3.0.3/docs/CMakeFiles/docs.dir/progress.make

external/glfw-3.0.3/docs/CMakeFiles/docs:
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/bletterer/Projets/DepthMaps/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating HTML documentation"
	cd /Users/bletterer/Projets/DepthMaps/build/external/glfw-3.0.3/docs && /usr/local/bin/doxygen

docs: external/glfw-3.0.3/docs/CMakeFiles/docs
docs: external/glfw-3.0.3/docs/CMakeFiles/docs.dir/build.make
.PHONY : docs

# Rule to build all files generated by this target.
external/glfw-3.0.3/docs/CMakeFiles/docs.dir/build: docs
.PHONY : external/glfw-3.0.3/docs/CMakeFiles/docs.dir/build

external/glfw-3.0.3/docs/CMakeFiles/docs.dir/clean:
	cd /Users/bletterer/Projets/DepthMaps/build/external/glfw-3.0.3/docs && $(CMAKE_COMMAND) -P CMakeFiles/docs.dir/cmake_clean.cmake
.PHONY : external/glfw-3.0.3/docs/CMakeFiles/docs.dir/clean

external/glfw-3.0.3/docs/CMakeFiles/docs.dir/depend:
	cd /Users/bletterer/Projets/DepthMaps/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/bletterer/Projets/DepthMaps /Users/bletterer/Projets/DepthMaps/external/glfw-3.0.3/docs /Users/bletterer/Projets/DepthMaps/build /Users/bletterer/Projets/DepthMaps/build/external/glfw-3.0.3/docs /Users/bletterer/Projets/DepthMaps/build/external/glfw-3.0.3/docs/CMakeFiles/docs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/glfw-3.0.3/docs/CMakeFiles/docs.dir/depend


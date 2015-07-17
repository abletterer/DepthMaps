#ifndef UTILS_HPP
#define UTILS_HPP

#include <GL/glew.h>

#include <string>

#include <vector>

#include <fstream>
#include <sstream>
#include <iterator>

#include <iostream>

void loadDepthMap(const std::string& filename, std::vector<GLfloat>& depth_map, int& width, int& height)
{
	std::ifstream file(filename);
	std::string line;

	getline(file, line);
	std::istringstream is_before( line );
	std::vector<GLfloat> tmp_vec((std::istream_iterator<GLfloat>(is_before)),
						  std::istream_iterator<GLfloat>());
	depth_map.insert(depth_map.end(), tmp_vec.begin(), tmp_vec.end());
	width = depth_map.size();

	while (getline( file, line )) {
		std::istringstream is( line );
		std::vector<GLfloat> tmp_vec{std::istream_iterator<GLfloat>(is),
							  std::istream_iterator<GLfloat>()};
		depth_map.insert(depth_map.end(), tmp_vec.begin(), tmp_vec.end());
	}

	height = depth_map.size()/width;
}

#endif // UTILS_HPP

#ifndef UTILS_HPP
#define UTILS_HPP

#include <GL/glew.h>

#include <string>

#include <vector>

#include <fstream>
#include <sstream>
#include <iterator>

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <glm/glm.hpp>

namespace fs = boost::filesystem;

enum file_type
{
	ORIGINAL = 0,
	VISIBILITY = 1,
	DENSITY = 2
};

void loadDepthMaps(const std::string& directory_name, std::vector<std::vector<GLfloat>>& depth_maps, std::vector<glm::mat4>& matrices, int& width, int& height, const file_type type)
{
	fs::path directory(directory_name);

	if ( fs::exists(directory) && fs::is_directory(directory))
	{
		std::vector<fs::path> files;
		std::copy(fs::directory_iterator(directory), fs::directory_iterator(), std::back_inserter(files));
		std::sort(files.begin(), files.end());
		boost::regex filter_type;
		switch(type)
		{
			case ORIGINAL:
			filter_type.set_expression(".*originalDepthMap.dat");
			break;
			case VISIBILITY:
			filter_type.set_expression(".*modifiedDepthMap-Visibility.dat");
			break;
			case DENSITY:
			filter_type.set_expression(".*modifiedDepthMap-Density.dat");
			break;
			default:
			filter_type.set_expression("-");
			break;
		}
		boost::smatch what;
		int count;
		boost::regex filter_matrix(".*MVPMatrix.dat");
		for(std::vector<fs::path>::const_iterator dir_iter = files.begin() ; dir_iter != files.end(); ++dir_iter)
		{
			if(boost::regex_match(dir_iter->filename().string(), what, filter_type))
			{
				std::vector<GLfloat> depth_map;
				std::ifstream file(directory_name+dir_iter->filename().string());
				std::string line;

				if(file.is_open())
				{
					getline(file, line);
					std::istringstream is_before(line);
					std::vector<GLfloat> tmp_vec((std::istream_iterator<GLfloat>(is_before)),
						std::istream_iterator<GLfloat>());
					depth_map.insert(depth_map.end(), tmp_vec.begin(), tmp_vec.end());
					width = depth_map.size();

					while (getline(file, line))
					{
						std::istringstream is(line);
						std::vector<GLfloat> tmp_vec{std::istream_iterator<GLfloat>(is), std::istream_iterator<GLfloat>()};
						depth_map.insert(depth_map.end(), tmp_vec.begin(), tmp_vec.end());
					}

					height = depth_map.size()/width;

					depth_maps.push_back(depth_map);
					++count;
				}
				else
				{
					std::cerr << "Problème à l'ouverture du fichier" << std::endl;
				}
			}
			else if(boost::regex_match(dir_iter->filename().string(), what, filter_matrix))
			{
				std::ifstream file(directory_name+dir_iter->filename().string());
				std::string line;

				if(file.is_open())
				{
					glm::mat4 tmp_mat;
					int count = 0;
					while (getline( file, line ) && count < 4)
					{
						std::istringstream is(line);
						std::vector<GLfloat> tmp_vec{std::istream_iterator<GLfloat>(is), std::istream_iterator<GLfloat>()};

						for(int i = 0; i < tmp_vec.size(); ++i)
						{
							tmp_mat[count][i] = tmp_vec[i];
						}
						++count;
					}

					tmp_mat = glm::inverse(tmp_mat);

					matrices.push_back(tmp_mat);
				}
				else
				{
					std::cerr << "Problème à l'ouverture du fichier" << std::endl;
				}
			}
		}
	}
}

#endif // UTILS_HPP

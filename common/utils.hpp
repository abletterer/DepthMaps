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

void loadSimulatedDepthMaps(const std::string& directory_name, std::vector<std::vector<GLfloat>>& depth_maps, std::vector<glm::mat4>& matrices, int& width, int& height, const file_type type)
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

void loadRealDepthMaps(const std::string& directory_name, std::vector<std::vector<GLfloat>>& depth_maps, std::vector<glm::mat4>& matrices, int& width, int& height)
{
	fs::path directory(directory_name);

	width = height = 0;

	if ( fs::exists(directory) && fs::is_directory(directory))
	{
		std::vector<fs::path> files;
		std::copy(fs::directory_iterator(directory), fs::directory_iterator(), std::back_inserter(files));
		std::sort(files.begin(), files.end());
		boost::regex filter_type;
		filter_type.set_expression("depth_.*.dat");
		boost::smatch what;
		int count;
		boost::regex filter_matrix("param_depth.*.dat");
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
						for(int i = 0; i < tmp_vec.size(); ++i)
						{
							tmp_vec[i] = (1-tmp_vec[i])*2-1;	//Convert depth-map from [0;1] to [-1;-1] where -1 is the near_plane and 1 the far_plane
						}
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
		}

		for(std::vector<fs::path>::const_iterator dir_iter = files.begin() ; dir_iter != files.end(); ++dir_iter)
		{
			if(boost::regex_match(dir_iter->filename().string(), what, filter_matrix))
			{
				std::ifstream file(directory_name+dir_iter->filename().string());
				std::string line;

				if(file.is_open())
				{
					glm::mat4 tmp_mat, ndc_mat, k_mat;
					int count = 0;

					std::vector<GLfloat> params;
					params.reserve(6);

					for(int i = 0; i < 4; ++i)
					{
						for(int j = 0; j < 4; ++j)
						{
							k_mat[i][j] = 0;
							ndc_mat[i][j] = 0;
						}
					}

					//Get every intrinsic parameter {fx, fy, cx, cy}

					while (getline( file, line ) && count < 4)
					{
						params.push_back(std::stof(line.substr(3)));
						++count;
					}

					//Add last two parameters {near, far}
					params.push_back(0.4); params.push_back(8.0);

					//Construction of the intrinsic matrix
					k_mat[0][0] = params[0];
					k_mat[1][1] = params[1];
					k_mat[2][0] = -params[2];
					k_mat[2][1] = -params[3];
					k_mat[2][2] = params[4]+params[5];
					k_mat[2][3] = -1;
					k_mat[3][2] = params[4]*params[5];

					//Construction of the ndc (normalized device coordinates matrix
					ndc_mat[0][0] = 2./width;
					ndc_mat[1][1] = 2./(-height);
					ndc_mat[2][2] = -2./(params[5]-params[4]);
					ndc_mat[3][0] = -1;
					ndc_mat[3][1] = 1;
					ndc_mat[3][2] = -(params[5]+params[4])/(params[5]-params[4]);
					ndc_mat[3][3] = 1;

					tmp_mat = ndc_mat*k_mat;

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

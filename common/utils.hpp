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

#include <Eigen/Core>

namespace fs = boost::filesystem;

enum file_type
{
	ORIGINAL = 0,
	VISIBILITY = 1,
	DENSITY = 2
};

void loadSimulatedDepthMaps(const std::string& directory_name, std::vector<Eigen::MatrixXf>& depth_maps,
							std::vector<glm::mat4>& matrices, int& width, int& height, const file_type type)
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
				std::vector<float> depth_vector;
				std::ifstream file(directory_name+dir_iter->filename().string());
				std::string line;

				if(file.is_open())
				{
					getline(file, line);
					std::istringstream is_before(line);
					std::vector<float> tmp_vec((std::istream_iterator<float>(is_before)),
						std::istream_iterator<float>());
					depth_vector.insert(depth_vector.end(), tmp_vec.begin(), tmp_vec.end());
					width = depth_vector.size();

					while (getline(file, line))
					{
						std::istringstream is(line);
						std::vector<float> tmp_vec{std::istream_iterator<float>(is), std::istream_iterator<float>()};
						depth_vector.insert(depth_vector.end(), tmp_vec.begin(), tmp_vec.end());
					}

					height = depth_vector.size()/width;

					Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> depth_image(depth_vector.data(), height, width);
					
					depth_image = depth_image.array()*2-1;

					depth_maps.push_back(depth_image);
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
						std::vector<float> tmp_vec{std::istream_iterator<float>(is), std::istream_iterator<float>()};

						for(int i = 0; i < tmp_vec.size(); ++i)
						{
							tmp_mat[count][i] = tmp_vec[i];
						}
						++count;
					}

					tmp_mat = glm::inverse(glm::transpose(tmp_mat));

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

void loadRealDepthMaps(const std::string& directory_name, std::vector<std::vector<float>>& depth_maps,
					   std::vector<glm::mat4>& matrices, int& width, int& height, std::vector<float>& params)
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
				std::vector<float> depth_vector;
				std::ifstream file(directory_name+dir_iter->filename().string());
				std::string line;

				if(file.is_open())
				{
					getline(file, line);
					std::istringstream is_before(line);
					std::vector<float> tmp_vec((std::istream_iterator<float>(is_before)),
						std::istream_iterator<float>());
					depth_vector.insert(depth_vector.end(), tmp_vec.begin(), tmp_vec.end());
					width = depth_vector.size();

					while (getline(file, line))
					{
						std::istringstream is(line);
						std::vector<float> tmp_vec{std::istream_iterator<float>(is), std::istream_iterator<float>()};
						for(int i = 0; i < tmp_vec.size(); ++i)
						{
							tmp_vec[i] = (tmp_vec[i])*2-1;	//Convert depth-map from [0;1] to [-1;1] where -1 is the near_plane and 1 the far_plane
						}
						depth_vector.insert(depth_vector.end(), tmp_vec.begin(), tmp_vec.end());
					}

					height = depth_vector.size()/width;

					depth_maps.push_back(depth_vector);
					++count;
				}
				else
				{
					std::cerr << "Problème à l'ouverture du fichier" << std::endl;
				}
			}
		}

		params.reserve(6);

		for(std::vector<fs::path>::const_iterator dir_iter = files.begin() ; dir_iter != files.end(); ++dir_iter)
		{
			if(boost::regex_match(dir_iter->filename().string(), what, filter_matrix))
			{
				std::ifstream file(directory_name+dir_iter->filename().string());
				std::string line;

				if(file.is_open())
				{
					glm::mat4 tmp_mat;
					int count = 0;

					//Get every intrinsic parameter {fx, fy, cx, cy}
					while (getline( file, line ) && count < 4)
					{
						params.push_back(std::stof(line.substr(3)));
						++count;
					}

					//Construction of the intrinsic matrix
					tmp_mat[0][0] = 1./params[0];
					tmp_mat[1][1] = 1./params[1];
					tmp_mat[2][0] = -params[2]/params[0];
					tmp_mat[2][1] = -params[3]/params[1];
					tmp_mat[2][2] = 1;
					tmp_mat[3][3] = 1;

//					tmp_mat = glm::inverse(tmp_mat);

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

void decompose(Eigen::MatrixXf& image, int width, int height, int& levels)
{
	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> image_tmp(image);

	levels = 0;

	while(width > 128 && height > 128)
	{
		const int width2 = width, height2 = height;
		width /= 2; height /= 2;

		//DECOMPOSITION VERTICALE
		for(int j = 0; j < height2; ++j)
		{
			//Traitement des pairs
			for(int i = 0; i < width2-1; i += 2)
			{
				image(i/2, j) = image_tmp(i, j);
			}

			//Traitement des impairs
			for(int i = 1; i < width2-1; i += 2)
			{
				float impair = image_tmp(i, j);
				float pair_1 = image_tmp(i-1, j);
				float pair_2 = image_tmp(i+1, j);

				impair -= (pair_1+pair_2)/2.f;
				image(width+i/2, j) = impair/2.f;
			}
		}

		//Traitement spécifique pour la dernière colonne (différence avec le pair situé à gauche)
		for(int j = 0; j < height2; ++j)
		{
			image(width2-1, j) = (image(width2-1, j)-image_tmp(width2-2, j))/2.f;
		}

		image_tmp = Eigen::Matrix<GLfloat, Eigen::Dynamic, Eigen::Dynamic>(image);

		//DECOMPOSITION HORIZONTALE

		for(int i = 0; i < width2; ++i)
		{
			for(int j = 0; j < height2-1; j += 2)
			{
				image(i, j/2) = image_tmp(i, j);
			}
			for(int j = 1; j < height2-1; j += 2)
			{
				float impair = image_tmp(i, j);
				float pair_1 = image_tmp(i, j-1);
				float pair_2 = image_tmp(i, j+1);

				impair -= (pair_1+pair_2)/2.f;
				image(i, height+j/2) = impair/2.f;
			}

			//Traitement spécifique pour la dernière ligne (différence avec le pair situé au dessus)
			image(i, height2-1) = (image(i, height2-1)-image_tmp(i, height2-2))/2.f;
		}

		++levels;
	}
}

#endif // UTILS_HPP

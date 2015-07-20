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

namespace fs = boost::filesystem;

enum file_type
{
	ORIGINAL = 0,
	VISIBILITY = 1,
	DENSITY = 2
};

void loadDepthMaps(const std::string& directory_name, std::vector<std::vector<GLfloat>>& depth_maps, int& width, int& height, const file_type type)
{
	fs::path directory(directory_name);
	fs::directory_iterator end_iter;

	if ( fs::exists(directory) && fs::is_directory(directory))
	{
		boost::regex filter;
		switch(type)
		{
			case ORIGINAL:
			filter.set_expression(".*originalDepthMap.dat");
			break;
			case VISIBILITY:
			filter.set_expression(".*modifiedDepthMap-Visibility.dat");
			break;
			case DENSITY:
			filter.set_expression(".*modifiedDepthMap-Density.dat");
			break;
			default:
			filter.set_expression("-");
			break;
		}
		boost::smatch what;
		int count;
		for(fs::directory_iterator dir_iter(directory) ; dir_iter != end_iter && count < 2; ++dir_iter)
		{
			if (fs::is_regular_file(dir_iter->status()))
			{
				if(boost::regex_match(dir_iter->path().filename().string(), what, filter))
				{
					std::vector<GLfloat> depth_map;
					std::ifstream file(directory_name+dir_iter->path().filename().string());
					std::string line;

					if(file.is_open())
					{
						getline(file, line);
						std::istringstream is_before( line );
						std::vector<GLfloat> tmp_vec((std::istream_iterator<GLfloat>(is_before)),
							std::istream_iterator<GLfloat>());
						depth_map.insert(depth_map.end(), tmp_vec.begin(), tmp_vec.end());
						width = depth_map.size();

						while (getline( file, line )) 
						{
							std::istringstream is( line );
							std::vector<GLfloat> tmp_vec{std::istream_iterator<GLfloat>(is), std::istream_iterator<GLfloat>()};
							depth_map.insert(depth_map.end(), tmp_vec.begin(), tmp_vec.end());
						}

						height = depth_map.size()/width;

						depth_maps.push_back(depth_map);
					}
					else
					{
						std::cerr << "Problème à l'ouverture du fichier" << std::endl;
					}
					++count;
				}
			}
		}
	}
}

#endif // UTILS_HPP

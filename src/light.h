#ifndef Light_H
#define Light_H

#include <vector>
#include <string>
#include "boundingbox.h"
#if 0
#include "glm/ext.hpp"
#endif

//TODO: Expand light to handle non-point light sources
class Light {
public:
	//CONSTRUCTOR & DESTRUCTOR
	//Light() {}
	Light(std::vector<glm::vec3> pos, std::vector<glm::vec3> clrs, std::vector<double> times) {
		positions.swap(pos);
		colors.swap(clrs);
		timesteps.swap(times);
	}
	~Light() {}

	// Accessors
	void SetPositions(std::vector<glm::vec3> p)
	{
		positions = p;
	}

	void SetColors (std::vector<glm::vec3> c)
	{
		colors = c;
	}

	void SetTimesteps (std::vector<double> t)
	{
		timesteps = t;
	}

	// print
	std::string to_string()
	{
		std::string result;
		result.append("Print Light\n");
		result.append("\tPositions:\n");
		for (int i = 0; i < positions.size(); i++)
		{
			result.append("\t\t");
			result.append(string_from_vec3(positions[i]));
			result.append("\n");
		}
		result.append("\tColors:\n");
		for (int i = 0; i < colors.size(); i++)
		{
			result.append("\t\t");
			result.append(string_from_vec3(colors[i]));
			result.append("\n");
		}
		result.append("\tTimesteps:\n");
		for (int i = 0; i < timesteps.size(); i++)
		{
			result.append("\t\t");
			result.append(std::to_string(timesteps[i]));
			result.append("\n");
		}
		return result;
	}

private:
	//REPRESENTATION
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> colors;
	std::vector<double> timesteps;
};

#endif

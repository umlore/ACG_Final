#ifndef Light_H
#define Light_H

#include <vector>
#include <string>
#include "boundingbox.h"

#if (_WIN32)
#define uint unsigned int
#endif

#if 0
#include "glm/ext.hpp"
#endif

struct light_interpolation {
	glm::vec3 pos;
	glm::vec3 clr;
	double intensity;
};

//TODO: Expand light to handle non-point light sources
class Light {
public:
	//CONSTRUCTOR & DESTRUCTOR
	//Light() {}
	Light(std::vector<glm::vec3> pos, std::vector<glm::vec3> clrs,
		std::vector<double> intens, std::vector<double> times) {

		positions.swap(pos);
		colors.swap(clrs);
		intensities.swap(intens);
		timesteps.swap(times);
		curidx = 0;
	}
	~Light() {}

	//ACCESSORS
	light_interpolation getInterpolation(double t) {
		//timesteps[0] is time between type[0] and type[1]
		//timesteps[max] is time between type[0] and type[0] (full cycle)
		t = fmod(t,timesteps[timesteps.size()-1]);
		light_interpolation li;

		while (!(t >= timesteps[curidx] && t < timesteps[curidx+1])) {
			curidx++;
			if (curidx == timesteps.size()-1) { curidx = 0; }
		}

		double diff = timesteps[curidx+1] - timesteps[curidx];
		diff = (t-timesteps[curidx])/diff;
		std::cout << "BLEND RATE: " << diff << '\n';

		li.pos = positions[(curidx+1)%positions.size()] - positions[curidx];
		li.clr = colors[(curidx+1)%colors.size()] - colors[curidx];
		li.intensity = intensities[(curidx+1)%intensities.size()] - intensities[curidx];

		li.pos *= diff;
		li.clr *= diff;
		li.intensity *= diff;

		li.pos += positions[curidx];
		li.clr += colors[curidx];
		li.intensity += intensities[curidx];

		return li;
		
	}

	//UTILITY
	std::string to_string()
	{
		std::string result;
		result.append("Print Light\n");
		result.append("\tPositions:\n");
		for (uint i = 0; i < positions.size(); i++)
		{
			result.append("\t\t");
			result.append(string_from_vec3(positions[i]));
			result.append("\n");
		}
		result.append("\tColors:\n");
		for (uint i = 0; i < colors.size(); i++)
		{
			result.append("\t\t");
			result.append(string_from_vec3(colors[i]));
			result.append("\n");
		}
		result.append("\tIntensities:\n");
		for (uint i = 0; i < intensities.size(); i++)
		{
			result.append("\t\t");
			result.append(std::to_string(intensities[i]));
			result.append("\n");
		}
		result.append("\tTimesteps:\n");
		for (uint i = 0; i < timesteps.size(); i++)
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
	std::vector<double> intensities; //stored as radius of light volume
	std::vector<double> timesteps;
	uint curidx; //current index
};

#endif

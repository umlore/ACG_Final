#ifndef Light_H
#define Light_H

#include <vector>
#include <string>
#include "boundingbox.h"

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

	//ACCESSORS
	glm::vec3 getPos(int i) {return positions[i];}
	glm::vec3 getColor(int i) {return colors[i];}
	double getTimestep(int i) {return timesteps[i];}

private:
	//REPRESENTATION
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> colors;
	std::vector<double> timesteps;
};

#endif
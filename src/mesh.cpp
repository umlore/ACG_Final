#include "mesh.h"
#include <iostream>

Mesh::Mesh() {
	
}

Mesh::Mesh(std::vector<int> i, std::vector<glm::vec3> c, std::vector<glm::vec3> p,
	std::vector<double> t, BoundingBox b) {
	indeces.swap(i);
	colors.swap(c);
	positions.swap(p);
	timesteps.swap(t);
	bbox = b;
}

Mesh::~Mesh() {
	
}

std::string Mesh::to_string() {
	std::string output = "";
	
	output+="<Mesh\nVerts: ";
	output.append(std::to_string(indeces[0]));
	output.append("-");
	output.append(std::to_string(indeces[1]));
	output.append("\nEdges: ");
	output.append(std::to_string(indeces[2]));
	output.append("-");
	output.append(std::to_string(indeces[3]));
	output.append("\nTris: ");
	output.append(std::to_string(indeces[4]));
	output.append("-");
	output.append(std::to_string(indeces[5]));
	output.append("\nPositions: \n"); 
	for (uint i = 0; i < positions.size(); i++) {
		output.append(glm::to_string(positions[i]));
		output.append("\n");
	}
	output.append("Colors:\n");
	for (uint i = 0; i < colors.size(); i++) {
		output.append(glm::to_string(colors[i]));
		output.append("\n");
	}
	output.append("Timesteps:\n");
	for (uint i = 0; i < timesteps.size(); i++) {
		output.append(std::to_string(timesteps[i]));
		output.append("\n");
	}
	return output;
	
}
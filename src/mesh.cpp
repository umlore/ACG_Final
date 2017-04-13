#include "mesh.h"

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
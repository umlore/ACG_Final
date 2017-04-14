#ifndef Mesh_H
#define Mesh_H

#include <vector>
#include <string>
#include "boundingbox.h"
#include "glm/ext.hpp"

class Mesh {

public:
	//CONSTRUCTOR & DESTRUCTOR
	Mesh();
	Mesh(std::vector<int>, std::vector<glm::vec3>, std::vector<glm::vec3>, std::vector<double>, BoundingBox);
	~Mesh();

	//MODIFIERS
	void setIndeces(std::vector<int> i) { indeces.swap(i); }
	void setColors(std::vector<glm::vec3> c) { colors.swap(c); }
	void setPositions(std::vector<glm::vec3> p) { positions.swap(p); }
	void setTimesteps(std::vector<double> t) { timesteps.swap(t); }

	//ACCESSORS
	std::string to_string();

	//VERTICES
	int numVertices() const { return indeces[1] - indeces[0]; }
	
	//EDGES
	int numEdges() const { return indeces[3] - indeces[2]; }

	//TRIANGLES
	int numTriangles() const { return indeces[5] - indeces[4]; }

private:

	//REPRESENTATION
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> positions;
	std::vector<double> timesteps;

	std::vector<int> indeces;
	//first vertex 	 0
	//last vertex 	 1
	//first edge 	 2
	//last edge 	 3
	//first tri 	 4
	//last tri 		 5

	//bounding box of this individual object
	BoundingBox bbox;
};

#endif
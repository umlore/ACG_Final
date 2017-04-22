#ifndef Mesh_H
#define Mesh_H

#include <vector>
#include <string>
#include "boundingbox.h"
#if 0
#include "glm/ext.hpp"
#endif

#if (_WIN32)
	#define uint unsigned int
#endif

struct mesh_interpolation {
	glm::vec3 clr;
	glm::vec3 pos;
};

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
	void setFilename(std::string fn) { filename = fn; }

	//ACCESSORS
	mesh_interpolation getInterpolation(double t);
	std::string to_string();

	std::string getFilename() { return filename; }

	//COMPLETELY DEFUNCT ACCESSORS - INDECES INFORMATION CAN BE FOUND
	//IN GEOMETRY FILEMAP INDEXED BY FILENAME
	int getMinVertexIndex() { return indeces[0]; }
	int getMaxVertexIndex() { return indeces[1]; }
	int getMinEdgeIndex() { return indeces[2]; }
	int getMaxEdgeIndex() { return indeces[3]; }
	int getMinTriIndex() { return indeces[4]; }
	int getMaxTriIndex() { return indeces[5]; }

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
	std::string filename;
	uint curidx;

	std::vector<int> indeces;
	//first vertex 	 0
	//num vertex 	 1
	//first edge 	 2
	//num edge 	     3
	//first tri 	 4
	//num tri 		 5

	//bounding box of this individual object
	BoundingBox bbox;
};

#endif

//TODO (EUGENE): THIS IS NOT DONE, DONT WORRY ABOUT IT.  PROBABLY WONT USE

#ifndef Mesh_H
#define Mesh_H

class Mesh {

public:
	//CONSTRUCTOR & DESTRUCTOR
	Mesh();
	~Mesh();

	//VERTICES
	int numVertices() const { return lastVert - firstVert; }
	
	//EDGES
	int numEdges() const { return lastEdge - firstEdge; }

	//TRIANGLES
	int numTriangles() const { return lastTri - firstTri; }

private:

	//REPRESENTATION

	//bounding indeces of the vertices in the geometry vertex structure
	int firstVert;
	int lastVert;

	//bounding indeces of the vertices in the geometry edge structure
	int firstEdge;
	int lastEdge;

	//bounding indeces of the triangles in the geometry edge structure
	int firstTri;
	int lastTri;

	//bounding box of this individual object
	BoundingBox bbox;

	//path variables
	//must be same length, timestep[i] is the length of time to interpolate
	//to meshPos[i]
	std::vector<glm::vec3> meshPos;
	std::vector<double> timestep;
}
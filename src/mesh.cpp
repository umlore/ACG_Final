#include "mesh.h"
#include <iostream>

#if (_WIN32)
#define uint unsigned int
#endif

Mesh::Mesh() {
	curidx = 0;
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

mesh_interpolation Mesh::getInterpolation(double t) {
	t = fmod(t,timesteps[timesteps.size()-1]);
	mesh_interpolation mi;

	while (!(t >= timesteps[curidx] && t < timesteps[curidx+1])) {
		curidx++;
		if (curidx == timesteps.size()-1) {curidx = 0;}
	}

	double diff = timesteps[curidx+1] - timesteps[curidx];
	diff = (t-timesteps[curidx])/diff;

	mi.pos = positions[(curidx+1)%positions.size()] - positions[curidx];
	mi.clr = colors[(curidx+1)%colors.size()] - colors[curidx];

	mi.pos *= diff;
	mi.clr *= diff;

	mi.pos += positions[curidx];
	mi.clr += colors[curidx];

	return mi;
}


std::string Mesh::to_string() {
	std::string output = "";
	/*
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
	*/
	output+="Filename: ";
	output+=filename;
	output+= "\n";
	for (uint i = 0; i < positions.size(); i++) {
		output.append(string_from_vec3(positions[i]));
		output.append("\n");
	}
	output.append("Colors:\n");
	for (uint i = 0; i < colors.size(); i++) {
		output.append(string_from_vec3(colors[i]));
		output.append("\n");
	}
	output.append("Timesteps:\n");
	for (uint i = 0; i < timesteps.size(); i++) {
		output.append(std::to_string(timesteps[i]));
		output.append("\n");
	}
	return output;
	
}

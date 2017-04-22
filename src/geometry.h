#ifndef Geometry_H
#define Geometry_H

#include "glCanvas.h"
#include <vector>
#include <string>
#include <map>
#include "hash.h"
#include "boundingbox.h"
#include "vbo_structs.h"
#include "mesh.h"
#include "light.h"

class ArgParser;
class Vertex;
class Edge;
class Triangle;
class Mesh;

// ======================================================================
// ======================================================================

class Geometry {

public:

  // ========================
  // CONSTRUCTOR & DESTRUCTOR
  Geometry(ArgParser *_args) { args = _args; }
  ~Geometry();
  std::vector<int> ReadObjFile(char* filename);
  void Load(); 
  void ComputeGouraudNormals();

  void initializeVBOs(); 
  void setupVBOs(); 
  void drawVBOs();
  void cleanupVBOs();
    
  // ========
  // VERTICES
  int numVertices() const { return vertices.size(); }
  Vertex* addVertex(const glm::vec3 &pos);
  // look up vertex by index from original .obj file
  Vertex* getVertex(int i) const {
    assert (i >= 0 && i < numVertices());
    Vertex *v = vertices[i];
    assert (v != NULL);
    return v; }

  // =====
  // EDGES
  int numEdges() const { return edges.size(); }
  // this efficiently looks for an edge with the given vertices, using a hash table
  Edge* getGeometryEdge(Vertex *a, Vertex *b) const;

  // =========
  // TRIANGLES
  int numTriangles() const { return triangles.size(); }
  void addTriangle(Vertex *a, Vertex *b, Vertex *c);
  void removeTriangle(Triangle *t);

  // ===============
  // OTHER ACCESSORS
  const BoundingBox& getBoundingBox() const { return bbox; }
  //glm::vec3 LightPosition() const;
  void PrintFileMap();

private:

  // HELPER FUNCTIONS FOR PAINT
  //void SetupLights();
  void SetupFloor();
  void SetupGeometry();

  //void DrawLights();
  void DrawFloor();
  void DrawGeometry();

  // ==============
  // REPRESENTATION
  ArgParser *args;
  std::vector<Mesh> meshes;
  std::vector<Vertex*> vertices;
  edgeshashtype edges;
  triangleshashtype triangles;
  std::vector<Light> lights;
  BoundingBox bbox;

  std::map<std::string, std::vector<int>> filemap;

  // VBOs
  GLuint geometry_tri_verts_VBO;
  GLuint geometry_tri_indices_VBO;
  GLuint floor_tri_verts_VBO;
  GLuint floor_tri_indices_VBO;
  //GLuint light_verts_VBO;

  std::vector<VBOPosNormalColor> geometry_tri_verts; 
  std::vector<VBOIndexedTri> geometry_tri_indices;
  std::vector<VBOPosNormalColor> floor_tri_verts; 
  std::vector<VBOIndexedTri> floor_tri_indices;
  //std::vector<VBOPosNormalColor> light_verts;

};

// ======================================================================
// ======================================================================

#endif






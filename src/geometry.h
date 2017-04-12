#ifndef Geometry_H
#define Geometry_H

#include "glCanvas.h"
#include <vector>
#include <string>
#include "hash.h"
#include "boundingbox.h"
#include "vbo_structs.h"

class ArgParser;
class Vertex;
class Edge;
class Triangle;

// ======================================================================
// ======================================================================

class Geometry {

public:

  // ========================
  // CONSTRUCTOR & DESTRUCTOR
  Geometry(ArgParser *_args) { args = _args; }
  ~Geometry();
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
  glm::vec3 LightPosition() const;

private:

  // HELPER FUNCTIONS FOR PAINT
  void SetupLight(const glm::vec3 &light_position);
  void SetupFloor();
  void SetupGeometry();

  void DrawLight();
  void DrawFloor();
  void DrawGeometry();

  // ==============
  // REPRESENTATION
  ArgParser *args;
  std::vector<Vertex*> vertices;
  edgeshashtype edges;
  triangleshashtype triangles;
  BoundingBox bbox;

  // VBOs
  GLuint geometry_tri_verts_VBO;
  GLuint geometry_tri_indices_VBO;
  GLuint floor_tri_verts_VBO;
  GLuint floor_tri_indices_VBO;
  GLuint light_vert_VBO;

  std::vector<VBOPosNormalColor> geometry_tri_verts; 
  std::vector<VBOIndexedTri> geometry_tri_indices;
  std::vector<VBOPosNormalColor> floor_tri_verts; 
  std::vector<VBOIndexedTri> floor_tri_indices;
  std::vector<VBOPosNormalColor> light_vert;

};

// ======================================================================
// ======================================================================

#endif





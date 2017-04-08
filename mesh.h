#ifndef MESH_H
#define MESH_H

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

class Mesh {

public:

  // ========================
  // CONSTRUCTOR & DESTRUCTOR
  Mesh(ArgParser *_args) { args = _args; }
  ~Mesh();
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
  Edge* getMeshEdge(Vertex *a, Vertex *b) const;

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
  void SetupMirror();
  void SetupFloor();
  void SetupMesh();
  void SetupReflectedMesh();
  void SetupReflectedFloor();
  void SetupSilhouetteEdges(const glm::vec3 &light_position);
  void SetupShadowPolygons(const glm::vec3 &light_position);

  void DrawLight();
  void DrawMirror();
  void DrawFloor();
  void DrawMesh();
  void DrawReflectedMesh();
  void DrawReflectedFloor();
  void DrawSilhouetteEdges();
  void DrawShadowPolygons();

  // ==============
  // REPRESENTATION
  ArgParser *args;
  std::vector<Vertex*> vertices;
  edgeshashtype edges;
  triangleshashtype triangles;
  BoundingBox bbox;

  // VBOs
  GLuint mesh_tri_verts_VBO;
  GLuint mesh_tri_indices_VBO;
  GLuint reflected_mesh_tri_verts_VBO;
  GLuint reflected_mesh_tri_indices_VBO;
  GLuint shadow_polygon_tri_verts_VBO;
  GLuint shadow_polygon_tri_indices_VBO;
  GLuint mirror_tri_verts_VBO;
  GLuint mirror_tri_indices_VBO;
  GLuint floor_tri_verts_VBO;
  GLuint floor_tri_indices_VBO;
  GLuint reflected_floor_tri_verts_VBO;
  GLuint reflected_floor_tri_indices_VBO;
  GLuint silhouette_edge_tri_verts_VBO;
  GLuint silhouette_edge_tri_indices_VBO;
  GLuint light_vert_VBO;

  std::vector<VBOPosNormalColor> mesh_tri_verts; 
  std::vector<VBOIndexedTri> mesh_tri_indices;
  std::vector<VBOPosNormalColor> reflected_mesh_tri_verts; 
  std::vector<VBOIndexedTri> reflected_mesh_tri_indices;
  std::vector<VBOPosNormalColor> shadow_polygon_tri_verts; 
  std::vector<VBOIndexedTri> shadow_polygon_tri_indices;
  std::vector<VBOPosNormalColor> mirror_tri_verts; 
  std::vector<VBOIndexedTri> mirror_tri_indices;
  std::vector<VBOPosNormalColor> floor_tri_verts; 
  std::vector<VBOIndexedTri> floor_tri_indices;
  std::vector<VBOPosNormalColor> reflected_floor_tri_verts; 
  std::vector<VBOIndexedTri> reflected_floor_tri_indices;
  std::vector<VBOPosNormalColor> silhouette_edge_tri_verts; 
  std::vector<VBOIndexedTri> silhouette_edge_tri_indices;
  std::vector<VBOPosNormalColor> light_vert;

};

// ======================================================================
// ======================================================================

#endif





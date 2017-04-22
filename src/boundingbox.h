#ifndef _BOUNDING_BOX_H_
#define _BOUNDING_BOX_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

#include <cassert>
#include <algorithm>
#include <vector>

#include <string>

#include "vbo_structs.h"

// Because getting std::max & std::min to work on all platforms is annoying
inline double mymax(double x, double y) { if (x > y) return x; return y; }
inline double mymin(double x, double y) { if (x < y) return x; return y; }

// ====================================================================
// ====================================================================

static std::string string_from_vec3(glm::vec3 v)
{
	std::string result;
	result.append("( ");
	result.append(std::to_string(v.x));
	result.append(", ");
	result.append(std::to_string(v.y));
	result.append(", ");
	result.append(std::to_string(v.z));
	result.append(")");
	return result;
}

static void print_from_mat4(glm::mat4 translateMatrix)
{
  printf( 
    "[%f, %f, %f, %f]\n"
    "[%f, %f, %f, %f]\n"
    "[%f, %f, %f, %f]\n"
    "[%f, %f, %f, %f]\n",
    translateMatrix[0][0], translateMatrix[0][1], translateMatrix[0][2], translateMatrix[0][3],
    translateMatrix[1][0], translateMatrix[1][1], translateMatrix[1][2], translateMatrix[1][3],
    translateMatrix[2][0], translateMatrix[2][1], translateMatrix[2][2], translateMatrix[2][3],
    translateMatrix[3][0], translateMatrix[3][1], translateMatrix[3][2], translateMatrix[3][3]);
}

class BoundingBox {

public:

  // ========================
  // CONSTRUCTOR & DESTRUCTOR
  BoundingBox() { 
    Set(glm::vec3(0,0,0),glm::vec3(0,0,0)); }
  BoundingBox(const glm::vec3 &pos) {
    Set(pos,pos); }
  BoundingBox(const glm::vec3 &_minimum, const glm::vec3 &_maximum) { 
    Set(_minimum,_maximum); }

  // =========
  // ACCESSORS
  void Get(glm::vec3 &_minimum, glm::vec3 &_maximum) const {
    _minimum = minimum;
    _maximum = maximum; }
  const glm::vec3& getMin() const { return minimum; }
  const glm::vec3& getMax() const { return maximum; }
  void getCenter(glm::vec3 &c) const {
    c = maximum; 
    c -= minimum;
    c *= 0.5f;
    c += minimum;
  }
  double maxDim() const {
    double x = maximum.x - minimum.x;
    double y = maximum.y - minimum.y;
    double z = maximum.z - minimum.z;
    return mymax(x,mymax(y,z));
  }

  // =========
  // MODIFIERS
  void Set(const BoundingBox &bb) {
    minimum = bb.minimum;
    maximum = bb.maximum; }
  void Set(const glm::vec3 &_minimum, const glm::vec3 &_maximum) {
    assert (minimum.x <= maximum.x &&
	    minimum.y <= maximum.y &&
	    minimum.z <= maximum.z);
    minimum = _minimum;
    maximum = _maximum; }
  void Extend(const glm::vec3 v) {
    minimum = glm::vec3(mymin(minimum.x,v.x),
                        mymin(minimum.y,v.y),
                        mymin(minimum.z,v.z));
    maximum = glm::vec3(mymax(maximum.x,v.x),
                        mymax(maximum.y,v.y),
                        mymax(maximum.z,v.z)); 
  }
  void Extend(const BoundingBox &bb) {
    Extend(bb.minimum);
    Extend(bb.maximum); 
  }

  void initializeVBOs();
  void setupVBOs();
  void drawVBOs();
  void cleanupVBOs();

private:


  // ==============
  // REPRESENTATION
  glm::vec3 minimum;
  glm::vec3 maximum;

  GLuint bb_verts_VBO;
  GLuint bb_tri_indices_VBO;
  std::vector<VBOPosNormalColor> bb_verts;
  std::vector<VBOIndexedTri> bb_tri_indices; // actually triangles

};

// ====================================================================
// ====================================================================

#endif

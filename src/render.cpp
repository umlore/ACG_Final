#include <iostream>

#include "glCanvas.h"
#include "geometry.h"
#include "edge.h"
#include "vertex.h"
#include "triangle.h"
#include "argparser.h"
#include "utils.h"
#include "glm/ext.hpp"

glm::vec4 floor_color(0.9,0.8,0.7,1);
glm::vec4 geometry_color(0.8,0.8,0.8,1);
glm::vec4 mirror_color(0.1,0.1,0.2,1);
glm::vec4 mirror_tint(0.85,0.9,0.95,1);

glm::vec4 red(1.0,0,0,1);
glm::vec4 green(0,1,0,0.5);

float floor_factor = 0.75;

// =======================================================================
// =======================================================================


// the light position can be animated
/*
glm::vec3 Geometry::LightPosition() const {
  glm::vec3 min = bbox.getMin();
  glm::vec3 max = bbox.getMax();
  glm::vec3 tmp;
  bbox.getCenter(tmp);
  tmp += glm::vec3(0,1.5*(max.y-min.y),0);
  tmp += glm::vec3(cos(args->timer) * (max.x-min.x), 0, 0);
  tmp += glm::vec3(0,0,sin(args->timer) * (max.z-min.z));
  return tmp;
}
*/

void Geometry::initializeVBOs() {
  glGenBuffers(1,&geometry_tri_verts_VBO);
  glGenBuffers(1,&geometry_tri_indices_VBO);
  glGenBuffers(1,&floor_tri_verts_VBO);
  glGenBuffers(1,&floor_tri_indices_VBO);
  //glGenBuffers(1,&light_verts_VBO);
  bbox.initializeVBOs();
}

void Geometry::cleanupVBOs() {
	/* TODO (Andrew): We will eventually have an arbitrary number of geometryes to render
	   when that happens, we will need a loop. */
  glDeleteBuffers(1,&geometry_tri_verts_VBO);
  glDeleteBuffers(1,&geometry_tri_indices_VBO);
  glDeleteBuffers(1,&floor_tri_verts_VBO);
  glDeleteBuffers(1,&floor_tri_indices_VBO);
  //glDeleteBuffers(1,&light_verts_VBO);
  bbox.cleanupVBOs();
}

// ================================================================================
// ================================================================================
//TODO (Eugene): Setup lights currently does nothing.  do something
//void Geometry::SetupLights() {
  /*
  for (uint i = 0; i < lights.size(); i++) {
    glm::vec4 light_color(lights[i].getColor(0).x,lights[i].getColor(0).y,lights[i].getColor(0).z,1);
    light_verts.push_back(VBOPosNormalColor(lights[i].getPos(0),glm::vec3(1,0,0),light_color));
  }
  
  
  glBindBuffer(GL_ARRAY_BUFFER,light_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormalColor)*lights.size(),&light_verts[0],GL_STATIC_DRAW); 
  */
//}

void Geometry::SetupFloor() {
  glm::vec3 diff = bbox.getMax()-bbox.getMin();
  // create vertices just a bit bigger than the bounding box
  glm::vec3 a = bbox.getMin() + glm::vec3(-floor_factor*diff.x,0,-floor_factor*diff.z);
  glm::vec3 b = bbox.getMin() + glm::vec3(-floor_factor*diff.x,0, (1+floor_factor)*diff.z);
  glm::vec3 c = bbox.getMin() + glm::vec3( (1+floor_factor)*diff.x,0, (1+floor_factor)*diff.z);
  glm::vec3 d = bbox.getMin() + glm::vec3( (1+floor_factor)*diff.x,0,-floor_factor*diff.z);
  glm::vec3 normal = ComputeNormal(a,c,d);
  floor_tri_verts.push_back(VBOPosNormalColor(a,normal,floor_color));
  floor_tri_verts.push_back(VBOPosNormalColor(b,normal,floor_color));
  floor_tri_verts.push_back(VBOPosNormalColor(c,normal,floor_color));
  floor_tri_verts.push_back(VBOPosNormalColor(d,normal,floor_color));
  floor_tri_indices.push_back(VBOIndexedTri(0,1,2));
  floor_tri_indices.push_back(VBOIndexedTri(0,2,3));
  glBindBuffer(GL_ARRAY_BUFFER,floor_tri_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(VBOPosNormalColor) * floor_tri_verts.size(), 
	       &floor_tri_verts[0],
	       GL_STATIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,floor_tri_indices_VBO); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	       sizeof(VBOIndexedTri) * floor_tri_indices.size(),
	       &floor_tri_indices[0], GL_STATIC_DRAW);
}


void Geometry::SetupGeometry() {
  for (triangleshashtype::iterator iter = triangles.begin();
       iter != triangles.end(); iter++) {
    Triangle *t = iter->second;
    glm::vec3 a = (*t)[0]->getPos();
    glm::vec3 b = (*t)[1]->getPos();
    glm::vec3 c = (*t)[2]->getPos();    
    glm::vec3 na = ComputeNormal(a,b,c);
    glm::vec3 nb = na;
    glm::vec3 nc = na;
    if (args->gouraud_normals) {
      na = (*t)[0]->getGouraudNormal();
      nb = (*t)[1]->getGouraudNormal();
      nc = (*t)[2]->getGouraudNormal();
    }
    int start = geometry_tri_verts.size();
    geometry_tri_verts.push_back(VBOPosNormalColor(a,na,geometry_color));
    geometry_tri_verts.push_back(VBOPosNormalColor(b,nb,geometry_color));
    geometry_tri_verts.push_back(VBOPosNormalColor(c,nc,geometry_color));
    geometry_tri_indices.push_back(VBOIndexedTri(start,start+1,start+2));
  }
  glBindBuffer(GL_ARRAY_BUFFER,geometry_tri_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(VBOPosNormalColor) * geometry_tri_verts.size(), 
	       &geometry_tri_verts[0],
	       GL_STATIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,geometry_tri_indices_VBO); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	       sizeof(VBOIndexedTri) * geometry_tri_indices.size(),
	       &geometry_tri_indices[0], GL_STATIC_DRAW);
}

// ================================================================================
// ================================================================================

//TODO (Eugene): FIX DRAWLIGHT TO DRAW ALL THE LIGHTS
//void Geometry::DrawLights() {
  /*
  HandleGLError("enter draw mirror");
  glPointSize(10);
  glBindBuffer(GL_ARRAY_BUFFER, light_vert_VBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
  glDrawArrays(GL_POINTS, 0, light_vert.size());
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  HandleGLError("enter draw mirror");
  */
//}

void Geometry::DrawFloor() {
  HandleGLError("enter draw floor");
  glBindBuffer(GL_ARRAY_BUFFER,floor_tri_verts_VBO); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,floor_tri_indices_VBO); 
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
  glDrawElements(GL_TRIANGLES, floor_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  HandleGLError("leaving draw floor");
}

void Geometry::DrawGeometry() {
  HandleGLError("enter draw geometry");
  glBindBuffer(GL_ARRAY_BUFFER,geometry_tri_verts_VBO); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,geometry_tri_indices_VBO); 
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
  glDrawElements(GL_TRIANGLES, geometry_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  HandleGLError("leaving draw geometry");
}

// ======================================================================================
// ======================================================================================

void Geometry::setupVBOs() {
  // delete all the old geometry
  geometry_tri_verts.clear(); 
  geometry_tri_indices.clear();
  floor_tri_verts.clear(); 
  floor_tri_indices.clear();
  //light_verts.clear();

  // setup the new geometry
  //glm::vec3 light_position = LightPosition();
  //SetupLights();
  SetupFloor();
  SetupGeometry();
  bbox.setupVBOs();
}

void Geometry::drawVBOs() {

  /* Render all the geometry to a texture. */
  {
    //DrawMirror();
    //DrawFloor();
    if (args->geometry) {
      // shader 1: CHECKERBOARD
      // shader 2: ORANGE
      // shader 3: other
      glUniform1i(GLCanvas::whichshaderID, args->whichshader);
      DrawFloor();
      DrawGeometry();
      glUniform1i(GLCanvas::whichshaderID, 0);
    }
  } 

  /* Render the texture to the screen. */
  {}

  if (args->bounding_box) {
    bbox.drawVBOs();
  }

  HandleGLError(); 
}

// =================================================================

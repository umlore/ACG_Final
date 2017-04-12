#include <iostream>

#include "glCanvas.h"
#include "mesh.h"
#include "edge.h"
#include "vertex.h"
#include "triangle.h"
#include "argparser.h"
#include "utils.h"
#include "glm/ext.hpp"

glm::vec4 floor_color(0.9,0.8,0.7,1);
glm::vec4 mesh_color(0.8,0.8,0.8,1);
glm::vec4 mirror_color(0.1,0.1,0.2,1);
glm::vec4 mirror_tint(0.85,0.9,0.95,1);

glm::vec4 red(1.0,0,0,1);
glm::vec4 green(0,1,0,0.5);

float floor_factor = 0.75;

// =======================================================================
// =======================================================================


// the light position can be animated
glm::vec3 Mesh::LightPosition() const {
  glm::vec3 min = bbox.getMin();
  glm::vec3 max = bbox.getMax();
  glm::vec3 tmp;
  bbox.getCenter(tmp);
  tmp += glm::vec3(0,1.5*(max.y-min.y),0);
  tmp += glm::vec3(cos(args->timer) * (max.x-min.x), 0, 0);
  tmp += glm::vec3(0,0,sin(args->timer) * (max.z-min.z));
  return tmp;
}


void Mesh::initializeVBOs() {
  glGenBuffers(1,&mesh_tri_verts_VBO);
  glGenBuffers(1,&mesh_tri_indices_VBO);
  glGenBuffers(1,&floor_tri_verts_VBO);
  glGenBuffers(1,&floor_tri_indices_VBO);
  glGenBuffers(1,&light_vert_VBO);
  bbox.initializeVBOs();
}

void Mesh::cleanupVBOs() {
	/* TODO (Andrew): We will eventually have an arbitrary number of meshes to render
	   when that happens, we will need a loop. */
  glDeleteBuffers(1,&mesh_tri_verts_VBO);
  glDeleteBuffers(1,&mesh_tri_indices_VBO);
  glDeleteBuffers(1,&floor_tri_verts_VBO);
  glDeleteBuffers(1,&floor_tri_indices_VBO);
  glDeleteBuffers(1,&light_vert_VBO);
  bbox.cleanupVBOs();
}

// ================================================================================
// ================================================================================

void Mesh::SetupLight(const glm::vec3 &light_position) {
  light_vert.push_back(VBOPosNormalColor(light_position,glm::vec3(1,0,0),glm::vec4(1,1,0,0)));
  glBindBuffer(GL_ARRAY_BUFFER,light_vert_VBO); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormalColor)*1,&light_vert[0],GL_STATIC_DRAW); 
}

void Mesh::SetupFloor() {
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


void Mesh::SetupMesh() {
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
    int start = mesh_tri_verts.size();
    mesh_tri_verts.push_back(VBOPosNormalColor(a,na,mesh_color));
    mesh_tri_verts.push_back(VBOPosNormalColor(b,nb,mesh_color));
    mesh_tri_verts.push_back(VBOPosNormalColor(c,nc,mesh_color));
    mesh_tri_indices.push_back(VBOIndexedTri(start,start+1,start+2));
  }
  glBindBuffer(GL_ARRAY_BUFFER,mesh_tri_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(VBOPosNormalColor) * mesh_tri_verts.size(), 
	       &mesh_tri_verts[0],
	       GL_STATIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh_tri_indices_VBO); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	       sizeof(VBOIndexedTri) * mesh_tri_indices.size(),
	       &mesh_tri_indices[0], GL_STATIC_DRAW);
}

// ================================================================================
// ================================================================================

void Mesh::DrawLight() {
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
}

void Mesh::DrawFloor() {
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

void Mesh::DrawMesh() {
  HandleGLError("enter draw mesh");
  glBindBuffer(GL_ARRAY_BUFFER,mesh_tri_verts_VBO); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh_tri_indices_VBO); 
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
  glDrawElements(GL_TRIANGLES, mesh_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  HandleGLError("leaving draw mesh");
}

// ======================================================================================
// ======================================================================================

void Mesh::setupVBOs() {
  // delete all the old geometry
  mesh_tri_verts.clear(); 
  mesh_tri_indices.clear();
  floor_tri_verts.clear(); 
  floor_tri_indices.clear();
  light_vert.clear();

  // setup the new geometry
  glm::vec3 light_position = LightPosition();
  SetupLight(light_position);
  SetupFloor();
  SetupMesh();
  bbox.setupVBOs();
}

void Mesh::drawVBOs() {

  // mode 1: STANDARD PHONG LIGHTING (LIGHT ON)
  glUniform1i(GLCanvas::colormodeID, 1);

  // shader 0: NO SHADER
  //glUniform1i(GLCanvas::whichshaderID, 0);


  HandleGLError("enter draw vbos");
  // --------------------------
  // NEITHER SHADOWS NOR MIRROR
  /* TODO (Andrew): Used to be if statement for not shadows, not mirror. This will now 
     be our main rendering loop. */
  {
    //DrawMirror();
    //DrawFloor();
    if (args->geometry) {
      // shader 1: CHECKERBOARD
      // shader 2: ORANGE
      // shader 3: other
      glUniform1i(GLCanvas::whichshaderID, args->whichshader);
      DrawFloor();
      DrawMesh();
      glUniform1i(GLCanvas::whichshaderID, 0);
    }
  } 

  /*TODO (Andrew): Do we want shadows? Shadows are pretty, but they expensive.
   They are also kind of antithetical to differend rendering. How do commercial grade things 	do this?
   */

  // mode 0: NO LIGHTING
  //glUniform1i(GLCanvas::colormodeID, 0);

  /* TODO (Andrew): Draw light will need to be overhauled, because we are going to have a lot 	of lights. ACTUALLY, this probably just draws the square. Still need to change it, but	
   that would be useful.*/
  DrawLight();
  if (args->bounding_box) {
    bbox.drawVBOs();
  }

  HandleGLError(); 
}

// =================================================================

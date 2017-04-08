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
  glGenBuffers(1,&reflected_mesh_tri_verts_VBO);
  glGenBuffers(1,&reflected_mesh_tri_indices_VBO);
  glGenBuffers(1,&shadow_polygon_tri_verts_VBO);
  glGenBuffers(1,&shadow_polygon_tri_indices_VBO);
  glGenBuffers(1,&mirror_tri_verts_VBO);
  glGenBuffers(1,&mirror_tri_indices_VBO);
  glGenBuffers(1,&floor_tri_verts_VBO);
  glGenBuffers(1,&floor_tri_indices_VBO);
  glGenBuffers(1,&reflected_floor_tri_verts_VBO);
  glGenBuffers(1,&reflected_floor_tri_indices_VBO);
  glGenBuffers(1,&silhouette_edge_tri_verts_VBO);
  glGenBuffers(1,&silhouette_edge_tri_indices_VBO);
  glGenBuffers(1,&light_vert_VBO);
  bbox.initializeVBOs();
}

void Mesh::cleanupVBOs() {
  glDeleteBuffers(1,&mesh_tri_verts_VBO);
  glDeleteBuffers(1,&mesh_tri_indices_VBO);
  glDeleteBuffers(1,&reflected_mesh_tri_verts_VBO);
  glDeleteBuffers(1,&reflected_mesh_tri_indices_VBO);
  glDeleteBuffers(1,&shadow_polygon_tri_verts_VBO);
  glDeleteBuffers(1,&shadow_polygon_tri_indices_VBO);
  glDeleteBuffers(1,&mirror_tri_verts_VBO);
  glDeleteBuffers(1,&mirror_tri_indices_VBO);
  glDeleteBuffers(1,&floor_tri_verts_VBO);
  glDeleteBuffers(1,&floor_tri_indices_VBO);
  glDeleteBuffers(1,&reflected_floor_tri_verts_VBO);
  glDeleteBuffers(1,&reflected_floor_tri_indices_VBO);
  glDeleteBuffers(1,&silhouette_edge_tri_verts_VBO);
  glDeleteBuffers(1,&silhouette_edge_tri_indices_VBO);
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


void Mesh::SetupMirror() {
  glm::vec3 diff = bbox.getMax()-bbox.getMin();
  // create frame vertices just a bit bigger than the bounding box
  glm::vec3 a = bbox.getMin() + glm::vec3(-0.25*diff.x, 0.1*diff.y,-0.25*diff.z);
  glm::vec3 b = bbox.getMin() + glm::vec3(-0.25*diff.x, 1.25*diff.y,-0.25*diff.z);
  glm::vec3 c = bbox.getMin() + glm::vec3(-0.25*diff.x, 1.25*diff.y, 1.25*diff.z);
  glm::vec3 d = bbox.getMin() + glm::vec3(-0.25*diff.x, 0.1*diff.y, 1.25*diff.z);
  glm::vec3 normal = ComputeNormal(a,c,b);
  glm::vec4 color(0.1,0.1,0.1,1);

  mirror_tri_verts.push_back(VBOPosNormalColor(a,normal,mirror_color));
  mirror_tri_verts.push_back(VBOPosNormalColor(b,normal,mirror_color));
  mirror_tri_verts.push_back(VBOPosNormalColor(c,normal,mirror_color));
  mirror_tri_verts.push_back(VBOPosNormalColor(d,normal,mirror_color));
  mirror_tri_indices.push_back(VBOIndexedTri(0,1,2));
  mirror_tri_indices.push_back(VBOIndexedTri(0,2,3));
  glBindBuffer(GL_ARRAY_BUFFER,mirror_tri_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(VBOPosNormalColor) * mirror_tri_verts.size(), 
	       &mirror_tri_verts[0],
	       GL_STATIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mirror_tri_indices_VBO); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	       sizeof(VBOIndexedTri) * mirror_tri_indices.size(),
	       &mirror_tri_indices[0], GL_STATIC_DRAW);
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


// draw a second copy of the object where it appears to be on the other side of the mirror
void Mesh::SetupReflectedMesh() {

	//TODO: FIGURE OUT IF THERE'S AN ACTUAL MIRROR REPRESENTATION OR 
	//IF IT'S JUST HARDCODED

  // ASSIGNMENT: WRITE THIS FUNCTION

}


// draw a second copy of the floor where it appears to be on the other side of the mirror
void Mesh::SetupReflectedFloor() {

  // ASSIGNMENT: WRITE THIS FUNCTION


}


// figure out which edges are the silhouette of the object 
void Mesh::SetupSilhouetteEdges(const glm::vec3 &light_position) {



  // ASSIGNMENT: FIND THE SILOUETTE EDGES

  float thickness = 0.003*getBoundingBox().maxDim();
  
  // & use this helper function to create geometry for each edge
  // addEdgeGeometry(silhouette_edge_tri_verts,silhouette_edge_tri_indices,
  //                 /*VERTEX POS A*/,/*VERTEX POS B*/,red,red,thickness,thickness);



  if (silhouette_edge_tri_verts.size() > 0) {
    glBindBuffer(GL_ARRAY_BUFFER,silhouette_edge_tri_verts_VBO); 
    glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormalColor)*silhouette_edge_tri_verts.size(),&silhouette_edge_tri_verts[0],GL_STATIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,silhouette_edge_tri_indices_VBO); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedTri)*silhouette_edge_tri_indices.size(),&silhouette_edge_tri_indices[0],GL_STATIC_DRAW);
  }
}


// project the silhouette edges away from the light source
void Mesh::SetupShadowPolygons(const glm::vec3 &light_position) {



  // ASSIGNMENT: WRITE THIS FUNCTION
  



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

void Mesh::DrawMirror() {
  HandleGLError("enter draw mirror");
  glBindBuffer(GL_ARRAY_BUFFER,mirror_tri_verts_VBO); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mirror_tri_indices_VBO); 
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
  glDrawElements(GL_TRIANGLES, mirror_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  HandleGLError("leaving draw mirror");
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

void Mesh::DrawReflectedFloor() {
  if (reflected_floor_tri_verts.size() > 0) {
    HandleGLError("enter draw reflected_floor");
    glBindBuffer(GL_ARRAY_BUFFER,reflected_floor_tri_verts_VBO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,reflected_floor_tri_indices_VBO); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
    glDrawElements(GL_TRIANGLES, reflected_floor_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    HandleGLError("leaving draw reflected_floor");
  }
}

void Mesh::DrawReflectedMesh() {
  if (reflected_mesh_tri_verts.size() > 0) {
    HandleGLError("enter draw reflected_mesh");
    glBindBuffer(GL_ARRAY_BUFFER,reflected_mesh_tri_verts_VBO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,reflected_mesh_tri_indices_VBO); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
    glDrawElements(GL_TRIANGLES, reflected_mesh_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    HandleGLError("leaving draw reflected_mesh");
  }
}

void Mesh::DrawSilhouetteEdges() {
  if (silhouette_edge_tri_verts.size() > 0) {
    HandleGLError("enter draw silhouette_edge");
    glBindBuffer(GL_ARRAY_BUFFER,silhouette_edge_tri_verts_VBO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,silhouette_edge_tri_indices_VBO); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
    glDrawElements(GL_TRIANGLES, silhouette_edge_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    HandleGLError("leaving draw silhouette_edge");
  }
}

void Mesh::DrawShadowPolygons() {
  if (shadow_polygon_tri_verts.size() > 0) {
    HandleGLError("enter draw silhouette_edge");
    glBindBuffer(GL_ARRAY_BUFFER,shadow_polygon_tri_verts_VBO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,shadow_polygon_tri_indices_VBO); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
    glDrawElements(GL_TRIANGLES, shadow_polygon_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    HandleGLError("leaving draw silhouette_edge");
  }
}

// ======================================================================================
// ======================================================================================

void Mesh::setupVBOs() {
  // delete all the old geometry
  mesh_tri_verts.clear(); 
  mesh_tri_indices.clear();
  reflected_mesh_tri_verts.clear(); 
  reflected_mesh_tri_indices.clear();
  shadow_polygon_tri_verts.clear(); 
  shadow_polygon_tri_indices.clear();
  mirror_tri_verts.clear(); 
  mirror_tri_indices.clear();
  floor_tri_verts.clear(); 
  floor_tri_indices.clear();
  reflected_floor_tri_verts.clear(); 
  reflected_floor_tri_indices.clear();
  silhouette_edge_tri_verts.clear(); 
  silhouette_edge_tri_indices.clear();
  light_vert.clear();

  // setup the new geometry
  glm::vec3 light_position = LightPosition();
  SetupLight(light_position);
  SetupMirror();
  SetupFloor();
  SetupMesh();
  SetupReflectedMesh();
  SetupReflectedFloor();
  SetupSilhouetteEdges(light_position);
  SetupShadowPolygons(light_position);
  bbox.setupVBOs();
}

void Mesh::drawVBOs() {


  // mode 1: STANDARD PHONG LIGHTING (LIGHT ON)
  glUniform1i(GLCanvas::colormodeID, 1);

  // shader 0: NO SHADER
  glUniform1i(GLCanvas::whichshaderID, 0);


  HandleGLError("enter draw vbos");
  // --------------------------
  // NEITHER SHADOWS NOR MIRROR
  if (!args->mirror && !args->shadow) {
    DrawMirror();
    DrawFloor();
    if (args->geometry) {
      // shader 1: CHECKERBOARD
      // shader 2: ORANGE
      // shader 3: other
      glUniform1i(GLCanvas::whichshaderID, args->whichshader);
      DrawMesh();
      glUniform1i(GLCanvas::whichshaderID, 0);
    }
  } 

  // ---------------------
  // MIRROR ONLY RENDERING
  else if (args->mirror && !args->shadow) {
    // Clear frame, depth & stencil buffers
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Draw all non-mirror geometry to frame & depth buffers
    DrawMesh();
    DrawFloor();
    // draw back of mirror (if we can see the back)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    DrawMirror();     
    glDisable(GL_CULL_FACE);

    // Draw mirror to stencil buffer, where depth buffer passes
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
    glStencilFunc(GL_ALWAYS,1,~0); 
    // (only draw the mirror if we can see the front)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    DrawMirror();     
    glDisable(GL_CULL_FACE);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE); // disable frame buffer writes      
    glDepthRange(1,1);
    glDepthFunc(GL_ALWAYS);
    glStencilFunc(GL_EQUAL,1,~0); 
    glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
    DrawMirror();     

    // Set depth to infinity, where stencil buffer passes
    glDepthFunc(GL_LESS);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); // enable frame buffer writes
    glDepthRange(0,1);

    // Draw reflected geometry to frame & depth buffer, where stencil buffer passes
    DrawReflectedMesh();
    DrawReflectedFloor();

    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE); // disable frame buffer writes      
    glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
    glDepthFunc(GL_ALWAYS);
    DrawMirror();
    glDepthFunc(GL_LESS);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); // enable frame buffer writes    

    glDisable(GL_STENCIL_TEST);
  } 


  // ---------------------
  // SHADOW ONLY RENDERING
  else if (!args->mirror && args->shadow) {
    


  // ASSIGNMENT: WRITE THIS RENDERING MODE



  // use the following code to turn the lights on & off
  // ... instead of glEnable(GL_LIGHTS), etc.
  // mode 2: AMBIENT ONLY (LIGHT OFF) 
  //glUniform1i(GLCanvas::colormodeID, 2);
  // mode 1: STANDARD PHONG LIGHTING (LIGHT ON)
  //glUniform1i(GLCanvas::colormodeID, 1);



  }

  // --------------------------
  // MIRROR & SHADOW!
  else {
    assert (args->mirror && args->shadow);




    // EXTRA CREDIT: IMPLEMENT THIS INTERACTION




  }


  // -------------------------
  // ADDITIONAL VISUALIZATIONS (for debugging)
  if (args->reflected_geometry) {
    DrawReflectedMesh();
    DrawReflectedFloor();
  }

  // mode 0: NO LIGHTING
  glUniform1i(GLCanvas::colormodeID, 0);

  DrawLight();
  if (args->bounding_box) {
    bbox.drawVBOs();
  }
  if (args->silhouette_edges) {
    DrawSilhouetteEdges();
  }

  if (args->shadow_polygons) {

    // FIXME (not part of assignment): shadow polygons are currently
    //   visualized opaque.  in glut version they are transparent (and
    //   probably more helpful).  will eventually port/debug this.
    
    //glDisable(GL_LIGHTING);
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    //glEnable(GL_BLEND);
    //glDepthMask(GL_FALSE);
    //glColor4f(0,1,0.5,0.2);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    DrawShadowPolygons();
    //glDepthMask(GL_TRUE);
    //glDisable(GL_BLEND);
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    //glEnable(GL_LIGHTING);
  }

  HandleGLError(); 
}

// =================================================================

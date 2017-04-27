#include <iostream>

#include "glCanvas.h"
#include "geometry.h"
#include "edge.h"
#include "vertex.h"
#include "triangle.h"
#include "argparser.h"
#include "utils.h"
#if 0
#include "glm/ext.hpp"
#endif
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

glm::vec4 floor_color(0.9,0.8,0.7,1);
glm::vec4 geometry_color(0.8,0.8,0.8,1);
glm::vec4 mirror_color(0.1,0.1,0.2,1);
glm::vec4 mirror_tint(0.85,0.9,0.95,1);

glm::vec4 red(1.0,0,0,1);
glm::vec4 green(0,1,0,0.5);

float floor_factor = 0.75;



GLuint GLCanvas::lights_VAO;

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
  glGenBuffers(1,&lightbox_tri_verts_VBO);
  glGenBuffers(1,&lightbox_tri_indices_VBO);
  bbox.initializeVBOs();
}

void Geometry::cleanupVBOs() {
	/* TODO (Andrew): We will eventually have an arbitrary number of geometryes to render
	   when that happens, we will need a loop. */
  glDeleteBuffers(1,&geometry_tri_verts_VBO);
  glDeleteBuffers(1,&geometry_tri_indices_VBO);
  glDeleteBuffers(1,&floor_tri_verts_VBO);
  glDeleteBuffers(1,&floor_tri_indices_VBO);
  glDeleteBuffers(1,&lightbox_tri_verts_VBO);
  glDeleteBuffers(1,&lightbox_tri_indices_VBO);
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

void Geometry::SetupLightBox() {
  float thickness = 0.1;
  glm::vec4 black(0,0,0,.2);

  glGenVertexArrays(1, &GLCanvas::lights_VAO);
  glBindVertexArray(GLCanvas::lights_VAO);
  
  glm::vec3 p0(-1,-1,-1);
  glm::vec3 p1(1,-1,-1);
  glm::vec3 p2(-1,-1,1);
  glm::vec3 p3(1,-1,1);
  glm::vec3 p4(-1,1,-1);
  glm::vec3 p5(1,1,-1);
  glm::vec3 p6(-1,1,1);
  glm::vec3 p7(1,1,1);

  glm::vec3 zero(0,0,0);//for use w/ normals, they don't matter for light boxes

  lightbox_tri_verts.push_back(VBOPosNormalColor(p0,zero,black));
  lightbox_tri_verts.push_back(VBOPosNormalColor(p1,zero,black));
  lightbox_tri_verts.push_back(VBOPosNormalColor(p2,zero,black));
  lightbox_tri_verts.push_back(VBOPosNormalColor(p3,zero,black));
  lightbox_tri_verts.push_back(VBOPosNormalColor(p4,zero,black));
  lightbox_tri_verts.push_back(VBOPosNormalColor(p5,zero,black));
  lightbox_tri_verts.push_back(VBOPosNormalColor(p6,zero,black));
  lightbox_tri_verts.push_back(VBOPosNormalColor(p7,zero,black));

  lightbox_tri_indices.push_back(VBOIndexedTri(0,1,5));
  lightbox_tri_indices.push_back(VBOIndexedTri(0,5,4));

  lightbox_tri_indices.push_back(VBOIndexedTri(3,2,6));
  lightbox_tri_indices.push_back(VBOIndexedTri(3,6,7));

  lightbox_tri_indices.push_back(VBOIndexedTri(2,0,4));
  lightbox_tri_indices.push_back(VBOIndexedTri(2,4,6));

  lightbox_tri_indices.push_back(VBOIndexedTri(1,3,7));
  lightbox_tri_indices.push_back(VBOIndexedTri(1,7,5));

  lightbox_tri_indices.push_back(VBOIndexedTri(4,5,7));
  lightbox_tri_indices.push_back(VBOIndexedTri(4,7,6));

  lightbox_tri_indices.push_back(VBOIndexedTri(3,1,2));
  lightbox_tri_indices.push_back(VBOIndexedTri(0,2,1));


	HandleGLError("Inside setuplightbox 0");
  glBindBuffer(GL_ARRAY_BUFFER,lightbox_tri_verts_VBO);
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormalColor)*lightbox_tri_verts.size(),
      &lightbox_tri_verts[0],GL_STATIC_DRAW);
	HandleGLError("Inside setuplightbox 1");
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,lightbox_tri_indices_VBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedTri) * lightbox_tri_indices.size(),
      &lightbox_tri_indices[0],GL_STATIC_DRAW);
}

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
  /*
  int tri_i = 0;

  for (int i = 0; i < meshes.size(); i++) {
    while (tri_i <= meshes[i].getMaxTriIndex()) {

    }
  }
  */
  printf ("Setup Geometry\n");
  
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

void Geometry::DrawLightBox() {
  HandleGLError("enter draw light box");
  glBindBuffer(GL_ARRAY_BUFFER,lightbox_tri_verts_VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,lightbox_tri_indices_VBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
  glDrawElements(GL_TRIANGLES, lightbox_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  HandleGLError("leaving draw light box");
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
  lightbox_tri_verts.clear();
  lightbox_tri_indices.clear();

  // setup the new geometry
  //glm::vec3 light_position = LightPosition();
  //SetupLights();
	HandleGLError("setupVBOs A");
  SetupFloor();
	HandleGLError("setupVBOs B");
  SetupGeometry();
	HandleGLError("setupVBOs C");


  SetupLightBox();
	HandleGLError("setupVBOs D");
  //bbox.setupVBOs();
}



void Geometry::drawVBOs(const glm::mat4 &ProjectionMatrix,const glm::mat4 &ViewMatrix) {

  // prepare data to send to the shaders
  //glm::vec3 lightPos = geometry->LightPosition();
  //glm::vec4 lightPos2 = glm::vec4(lightPos.x,lightPos.y,lightPos.z,1);
  //lightPos2 = ModelMatrix * lightPos2;
  //glUniform3f(GLCanvas::LightID, 0, 0, 0);

  //glUniformMatrix4fv(GLCanvas::MatrixID, 1, GL_FALSE, &MVP[0][0]);
  //glUniformMatrix4fv(GLCanvas::ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

 	glBindFramebuffer(GL_FRAMEBUFFER, GLCanvas::targetBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Render all the geometry to a texture. */
  if (args->geometry) {
    //glUniformMatrix4fv(GLCanvas::MatrixID, 1, GL_FALSE, &MVP[0][0]);
    //glUniformMatrix4fv(GLCanvas::ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		glEnable(GL_DEPTH_TEST);
    //std::cout << "DRAWGEOMETRY START\n";
    for (int i = 0; i < meshes.size(); i++) 
		{
      
      //std::cout << "LOOP START\n";
      mesh_interpolation mi = meshes[i].getInterpolation(args->timer);
      //std::cout << "Pos: " << string_from_vec3(mi.pos) << '\n';

      glm::mat4 ModelMatrix = glm::translate(mi.pos);

      //ModelMatrix = glm::mat4(1.0f);

      //MVP = MVP * translateMatrix;
      glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

      //print_from_mat4(ModelMatrix);
      //print_from_mat4(MVP);

      //glm::mat4 modelModelMatrix = (translateMatrix);

      glUniformMatrix4fv(GLCanvas::MatrixID, 1, GL_FALSE, &MVP[0][0]);
      glUniformMatrix4fv(GLCanvas::ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

      DrawGeometry();
    }
    //DrawFloor();
		glDisable(GL_DEPTH_TEST);
  }

	/* if (lights) */
  if (args->lightbox) {
		glEnable(GL_DEPTH_TEST);
		for (int i = 0; i < numLights(); i++)
		{
      /* Interpolation */
      Light* l = getLight(i);
      light_interpolation li = l->getInterpolation(args->timer);
			/* Transforms */
      glm::mat4 ModelMatrix = glm::translate(li.pos);
      ModelMatrix = ModelMatrix * glm::scale(glm::vec3(li.intensity,li.intensity,li.intensity));
      glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			/* Uniforms */
      glUniformMatrix4fv(GLCanvas::MatrixID, 1, GL_FALSE, &MVP[0][0]);
      glUniformMatrix4fv(GLCanvas::ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
      //std::cout << "AFTER UNIFORMS\n";
			/* Draw Call */
      DrawLightBox();
      //std::cout<<"AFTER DRAW CALL\n";
		}
		glDisable(GL_DEPTH_TEST);
	}

  /* Render the texture to the screen. */
  {
			GLCanvas::drawPost();
	}

  if (args->bounding_box) {
    bbox.drawVBOs();
  }

  HandleGLError(); 
}

// =================================================================

#include <iostream>
#include <string.h>

#include "geometry.h"
#include "edge.h"
#include "utils.h"
#include "vertex.h"
#include "triangle.h"
#include "argparser.h"

int Triangle::next_triangle_id = 0;

// =======================================================================
// Geometry DESTRUCTOR 
// =======================================================================

Geometry::~Geometry() {
  // delete all the triangles
  std::vector<Triangle*> todo;
  for (triangleshashtype::iterator iter = triangles.begin();
       iter != triangles.end(); iter++) {
    Triangle *t = iter->second;
    todo.push_back(t);
  }
  int num_triangles = todo.size();
  for (int i = 0; i < num_triangles; i++) {
    removeTriangle(todo[i]);
  }
  // delete all the vertices
  int num_vertices = numVertices();
  for (int i = 0; i < num_vertices; i++) {
    delete vertices[i];
  }
  cleanupVBOs();
}

// =======================================================================
// MODIFIERS:   ADD & REMOVE
// =======================================================================

Vertex* Geometry::addVertex(const glm::vec3 &position) {
  int index = numVertices();
  Vertex *v = new Vertex(index, position);
  vertices.push_back(v);
  if (numVertices() == 1)
    bbox = BoundingBox(position,position);
  else 
    bbox.Extend(position);
  return v;
}


void Geometry::addTriangle(Vertex *a, Vertex *b, Vertex *c) {
  // create the triangle
  Triangle *t = new Triangle();
  // create the edges
  Edge *ea = new Edge(a,b,t);
  Edge *eb = new Edge(b,c,t);
  Edge *ec = new Edge(c,a,t);
  // point the triangle to one of its edges
  t->setEdge(ea);
  // connect the edges to each other
  ea->setNext(eb);
  eb->setNext(ec);
  ec->setNext(ea);
  // verify these edges aren't already in the Geometry 
  // (which would be a bug, or a non-manifold Geometry)
  assert (edges.find(std::make_pair(a,b)) == edges.end());
  assert (edges.find(std::make_pair(b,c)) == edges.end());
  assert (edges.find(std::make_pair(c,a)) == edges.end());
  // add the edges to the master list
  edges[std::make_pair(a,b)] = ea;
  edges[std::make_pair(b,c)] = eb;
  edges[std::make_pair(c,a)] = ec;
  // connect up with opposite edges (if they exist)
  edgeshashtype::iterator ea_op = edges.find(std::make_pair(b,a)); 
  edgeshashtype::iterator eb_op = edges.find(std::make_pair(c,b)); 
  edgeshashtype::iterator ec_op = edges.find(std::make_pair(a,c)); 
  if (ea_op != edges.end()) { ea_op->second->setOpposite(ea); }
  if (eb_op != edges.end()) { eb_op->second->setOpposite(eb); }
  if (ec_op != edges.end()) { ec_op->second->setOpposite(ec); }
  // add the triangle to the master list
  assert (triangles.find(t->getID()) == triangles.end());
  triangles[t->getID()] = t;
}


void Geometry::removeTriangle(Triangle *t) {
  Edge *ea = t->getEdge();
  Edge *eb = ea->getNext();
  Edge *ec = eb->getNext();
  Vertex *a = ea->getStartVertex();
  Vertex *b = eb->getStartVertex();
  Vertex *c = ec->getStartVertex();
  // remove these elements from master lists
  edges.erase(std::make_pair(a,b)); 
  edges.erase(std::make_pair(b,c)); 
  edges.erase(std::make_pair(c,a)); 
  triangles.erase(t->getID());
  // clean up memory
  delete ea;
  delete eb;
  delete ec;
  delete t;
}


// Helper function for accessing data in the hash table
Edge* Geometry::getGeometryEdge(Vertex *a, Vertex *b) const {
  edgeshashtype::const_iterator iter = edges.find(std::make_pair(a,b));
  if (iter == edges.end()) return NULL;
  return iter->second;
}

// =======================================================================
// the load function parses very simple .obj files
// =======================================================================
void Geometry::PrintFileMap() {
  std::cout << "FILE MAP: \n";
  for(std::map<std::string, std::vector<int> >::const_iterator it = filemap.begin();
    it != filemap.end(); ++it)
  {
    std::cout << it->first << " ";
    for (int i = 0; i < 6; i++) {
      std::cout << filemap[it->first][i] << " ";
    }
    std::cout << '\n';
  }
  std::cout << '\n';
}

std::vector<int> Geometry::ReadObjFile(char* filename) {
  std::vector<int> indeces(6);
  //[0] vert count
  //[1] vert start index
  //[2] edge count
  //[3] edge start index
  //[4] tri count
  //[5] tri start index
  indeces[0] = 0;
  indeces[2] = 0;
  indeces[4] = 0;
  indeces[1] = numVertices();
  indeces[3] = numEdges();
  indeces[5] = numTriangles();

  if (filemap.count(filename)) { return indeces; }
  std::string fn = "../src/";
  fn.append(filename);
  FILE *objfile = fopen(fn.c_str(), "r");
  if (objfile == NULL) {
    std::cout << "ERROR! CANNOT OPEN '" << filename << "'\n";
    return indeces;
  }

  char line[200] = "";
  char token[100] = "";
  char atoken[100] = "";
  char btoken[100] = "";
  char ctoken[100] = "";
  float x,y,z;
  int a,b,c,d,e;

  while(fgets(line, 200, objfile)) {
    int token_count = sscanf (line, "%s\n",token);
    if (token_count == -1) continue;
    a = b = c = d = e = -1;

    if (!strcmp(token, "v")) {
      indeces[0]++;
      sscanf (line, "%s %f %f %f\n",token,&x,&y,&z);
      addVertex(glm::vec3(x,y,z));
    } else if (!strcmp(token, "f")) {
      indeces[4]++;
      indeces[2] += 3;
      int num = sscanf (line, "%s %s %s %s\n",token,
      atoken,btoken,ctoken);
      sscanf (atoken,"%d",&a);
      sscanf (btoken,"%d",&b);
      sscanf (ctoken,"%d",&c);
      assert (num == 4);
      /* TODO (Eugene): This assumes that vert indexing starts at 1 for some reason
      //not sure if it needs to be fixed, our implementation starts at 0
      a -= vert_index;
      b -= vert_index;
      c -= vert_index;
      */
      assert (a >= 0 && a < numVertices());
      assert (b >= 0 && b < numVertices());
      assert (c >= 0 && c < numVertices());
      addTriangle(getVertex(a+indeces[1]),getVertex(b+indeces[1]),getVertex(c+indeces[1])); 
    }
  }
  return indeces;
}

void Geometry::Load() {

  //TODO: ADD ALL THE FILES TO FILEMAP, AND FIX MESH IF IT'S BROKEN, ALSO THE TOSTRING
  std::string input_file = args->path + "/" + args->input_file;
  
  FILE *objfile = fopen(input_file.c_str(),"r");
  if (objfile == NULL) {
    std::cout << "ERROR! CANNOT OPEN '" << input_file << "'\n";
    return;
  }

  char line[200] = "";
  char token[100] = "";
  char atoken[100] = "";
  char btoken[100] = "";
  char ctoken[100] = "";
  float x,y,z;
  int a,b,c,d,e;

  Mesh mesh = Mesh();
  
  /*
  int vert_count = 0;
  int vert_index = 0; //possibly change this back to 1
  int edge_count = 0;
  int edge_index = 0;
  int tri_count = 0;
  int tri_index = 0;
  */

  std::vector<int> indeces;

  std::string positionFile;

  std::vector<glm::vec3> colors;
  std::vector<glm::vec3> positions;
  std::vector<double> timesteps;

  std::vector<glm::vec3> lpos;
  std::vector<glm::vec3> lclrs;
  std::vector<double> ltimes;
  std::vector<double> lintensities;

  bool firstmesh = true;
  bool firstlight = true;
  /*
  //DATA MUST BE AN OBJECT FIRST - LIGHTS MUST BE AT END
  while(fgets(line,200,objfile)) {
    int token_count = sscanf(line,"%s\n",token);
    if (token_count == -1) continue;
    if (!strcmp(token,"o")){
      break;
    }
  }
  */
  while (fgets(line, 200, objfile)) { 
    //std::cout << "line: " << line;
    int token_count = sscanf (line, "%s\n",token);
    if (token_count == -1) continue;
    a = b = c = d = e = -1;
    if (!strcmp(token,"o")){
      

      if (firstmesh) {
        firstmesh = false;

        sscanf(line,"%s %s\n",token, atoken);
        //std::cout << "SUBFILENAME: " << atoken << "\n";
        std::vector<int> indeces;
        if (!filemap.count(atoken)) { //will never happen, cannot be in map yet
          indeces = ReadObjFile(atoken);
        }
        positionFile = atoken;
        filemap[positionFile] = indeces;

        continue;
      }
      /*
      std::vector<int> indeces;
      indeces.push_back(vert_index);
      indeces.push_back(vert_index+vert_count-1);
      indeces.push_back(edge_index);
      indeces.push_back(edge_index+edge_count-1);
      indeces.push_back(tri_index);
      indeces.push_back(tri_index+tri_count-1);
      */
      /*
      vert_count = 0;
      vert_index = numVertices();
      edge_count = 0;
      edge_index = numEdges();
      tri_count = 0;
      tri_index = numTriangles();
      */
      //add finalized lists to the mesh object
      //std::cout << "length: " << indeces.size() << '\n';
      //mesh.setIndeces(indeces);
      mesh.setColors(colors);
      mesh.setPositions(positions);
      mesh.setTimesteps(timesteps); 
      mesh.setFilename(positionFile);
      //add the mesh object to the master vector, and start a new one.
      meshes.push_back(mesh);
      mesh = Mesh();
      //std::cout << "length after: " << indeces.size() << '\n';

      //CLEAR THE ARRAYS
      indeces.clear();
      colors.clear();
      positions.clear();
      timesteps.clear();

      //READ THE FILE
      sscanf(line, "%s %s\n",token, atoken);
      //std::cout << "SUBFILENAME: " << atoken << "\n";
      std::vector<int> indeces;
      if (!filemap.count(atoken)) { //will never happen, cannot be in map yet
        indeces = ReadObjFile(atoken);
      }
      positionFile = atoken;
      filemap[positionFile] = indeces;

      //add a light
    } else if (!strcmp(token,"l")) {
      if (firstlight) {
        firstlight = false;
        continue;
      }
      Light temp_light = Light(lpos,lclrs,lintensities,ltimes);
      lights.push_back(temp_light);

      lpos.clear();
      lclrs.clear();
      ltimes.clear();
      lintensities.clear();

      //add a position
    } else if (!strcmp(token,"lp")) {
      sscanf(line, "%s %f %f %f\n",token,&x,&y,&z);
      //std::cout << "Succesful reads: "<<record<<" Line : " << line;
      //std::cout << "Parse Light Position: " << string_from_vec3( glm::vec3(x,y,z)) << std::endl;
      glm::vec3 light_pos(x,y,z);

      lpos.push_back(light_pos);

    } else if (!strcmp(token,"lc")) {
      sscanf(line, "%s %f %f %f\n",token,&x,&y,&z);
      //std::cout << "Parse Light Color: " << string_from_vec3( glm::vec3(x,y,z)) << std::endl;
      glm::vec3 light_clr(x,y,z);

      lclrs.push_back(light_clr);
      //add light timestep
    } else if (!strcmp(token,"lt")) {
      sscanf(line, "%s %f", token, &x);
      //std::cout << "Parse Light timestep: " << x << std::endl;
      ltimes.push_back(x);
      //add light intensity
    } else if (!strcmp(token,"li")) {
      sscanf(line, "%s %f", token, &x);
      lintensities.push_back(x);
    } else if (!strcmp(token,"p")) {
      
      sscanf(line, "%s %s %s %s\n",token,atoken,btoken,ctoken);
      double p1, p2, p3;
      sscanf(atoken,"%lf",&p1);
      sscanf(btoken,"%lf",&p2);
      sscanf(ctoken,"%lf",&p3);
      glm::vec3 temp_pos(p1,p2,p3);
      positions.push_back(temp_pos);
      
      //add a color
    } else if (!strcmp(token,"c")) {
      sscanf(line, "%s %s %s %s\n",token,atoken,btoken,ctoken);
      double p1, p2, p3;
      sscanf(atoken,"%lf",&p1);
      sscanf(btoken,"%lf",&p2);
      sscanf(ctoken,"%lf",&p3);
      glm::vec3 temp_clr(p1,p2,p3);
      colors.push_back(temp_clr);

      /*
      sscanf(line,"%s %s %s %s\n",token,atoken,btoken,ctoken);
      glm::vec3 temp_clr(atoken,btoken,ctoken);
      colors.push_back(temp_clr);
      */
      //add a timestep
    } else if (!strcmp(token,"t")) {
      sscanf(line,"%s %s",token,atoken);
      int p1;
      sscanf(atoken,"%d",&p1);
      timesteps.push_back(p1);
    } else if (!strcmp(token,"vt")) {
    } else if (!strcmp(token,"vn")) {
      //comment
    } else if (token[0] == '#') {
    } else {
      printf ("LINE: '%s'",line);
    }
  }

  /*
  indeces.push_back(vert_index);
  indeces.push_back(vert_index+vert_count-1);
  indeces.push_back(edge_index);
  indeces.push_back(edge_index+edge_count-1);
  indeces.push_back(tri_index);
  indeces.push_back(tri_index+tri_count-1);
  */

  /*
  vert_count = 0;
  vert_index = numVertices();
  edge_count = 0;
  edge_index = numEdges();
  tri_count = 0;
  tri_index = numTriangles();
  */
  //add finalized lists to the mesh object
  //mesh.setIndeces(indeces);
  mesh.setColors(colors);
  mesh.setPositions(positions);
  mesh.setTimesteps(timesteps);
  mesh.setFilename(positionFile); 
  //add the mesh object to the master vector, and start a new one.
  meshes.push_back(mesh);
  mesh = Mesh();

  Light temp_light = Light(lpos,lclrs,lintensities,ltimes);
  lights.push_back(temp_light);

  ComputeGouraudNormals();
  /*
  std::cout << "loaded " << numTriangles() << " triangles " << std::endl;
  
  for (uint i = 0; i < meshes.size(); i++) {
    std::cout << meshes[i].to_string();
  } 

  
  std::cout << "Print " << lights.size() << " lights" << std::endl;
  for (uint i = 0; i < lights.size(); i++)
  {
	  std::cout << lights[i].to_string();
  }
  */
  /*
  std::cout << "Light Interpolations: \n";
  for (double i = 0; i < 10; i+= .5) {
    light_interpolation li = lights[0].getInterpolation(i);
    std::cout << "Time: " << i << "\n\tPosition: " << string_from_vec3(li.pos);
    std::cout << "\n\tColor: " << string_from_vec3(li.clr);
    std::cout << "\n\tIntensity: " << li.intensity << "\n\n";
  }
  */
  /*
  std::cout << "Mesh Interpolations: \n";
  for (double i = 0; i < 10; i+=.5) {
    mesh_interpolation mi = meshes[0].getInterpolation(i);
    std::cout << "Time: " << i << "\n\tPosition: " << string_from_vec3(mi.pos);
    std::cout << "\n\tColor: " << string_from_vec3(mi.clr) << "\n\n";
  }
  */
  //PrintFileMap();
  //std::cout << "\nNumVertices: " << vertices.size() << "\nNumEdges: " << edges.size() << "\nnumTriangles: " << triangles.size() << '\n';
}

// =======================================================================

// compute the gouraud normals of all vertices of the Geometry and store at each vertex
void Geometry::ComputeGouraudNormals() {
  int i;
  // clear the normals
  for (i = 0; i < numVertices(); i++) {
    getVertex(i)->clearGouraudNormal();
  }
  // loop through all the triangles incrementing the normal at each vertex
  for (triangleshashtype::iterator iter = triangles.begin();
       iter != triangles.end(); iter++) {
    Triangle *t = iter->second;
    glm::vec3 n = ComputeNormal((*t)[0]->getPos(),
                                (*t)[1]->getPos(),
                                (*t)[2]->getPos());
    (*t)[0]->incrGouraudNormal(n);
    (*t)[1]->incrGouraudNormal(n);
    (*t)[2]->incrGouraudNormal(n);
  }
  // finally, normalize the sum at each vertex
  for (i = 0; i < numVertices(); i++) {
    getVertex(i)->normalizeGouraudNormal();
  }
}

// =================================================================

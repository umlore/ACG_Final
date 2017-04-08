#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "boundingbox.h"
#include "edge.h"

// ===========================================================
// Simple half-edge data structure representation for a triangle mesh

class Triangle {

public:

  // ========================
  // CONSTRUCTOR & DESTRUCTOR
  Triangle() {
    edge = NULL; 
    id = next_triangle_id;
    next_triangle_id++;
  }
  ~Triangle() {}

  // =========
  // ACCESSORS
  Vertex* operator[](int i) const { 
    assert (edge != NULL);
    if (i==0) return edge->getStartVertex();
    if (i==1) return edge->getNext()->getStartVertex();
    if (i==2) return edge->getNext()->getNext()->getStartVertex();
    assert(0);
  }
  Edge* getEdge() { 
    assert (edge != NULL);
    return edge; 
  }
  int getID() { return id; }

  // =========
  // MODIFIERS
  void setEdge(Edge *e) {
    assert (edge == NULL);
    edge = e;
  }

protected:

  // ==============
  // REPRESENTATION
  Edge *edge;
  int id;

  static int next_triangle_id;
};

// ===========================================================

#endif

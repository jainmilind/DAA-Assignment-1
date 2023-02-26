#include <vector>
#include "DCEL.h"

Face::Face() {}
HalfEdge::HalfEdge() {}
Vertex::Vertex() {}
DCEL::DCEL() {}

std::vector<Vertex*> Face::enumerate_all_vertices() {
    std::vector<Vertex*> vertices;
    HalfEdge* curr = this->edge;
   
    do {
        vertices.push_back(curr->org);
        curr = curr->nxt;
    } while (curr != this->edge);
    
    return vertices;
}


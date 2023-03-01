#include <vector>

// Edge is any edge of face in clockwise order
class Face {
public:
    HalfEdge* edge = NULL;    
    Face();
    Face(HalfEdge*);
    std::vector<Vertex*> enumerate_all_vertices();
};

/*
   org = Source vertex
   face = face pointing in clockwise direction
   twin = halfedge in oppostie direction
   nxt = nxt halfedge in same order
   prev = previous halfedge in same order
*/
class HalfEdge {
public:
    Vertex* org = NULL;
    Face* face = NULL;
    HalfEdge *twin = NULL, *nxt = NULL, *prev = NULL;
    HalfEdge();
    HalfEdge(Vertex*, Vertex*);
    HalfEdge(HalfEdge*, Vertex*);
    HalfEdge(HalfEdge*, HalfEdge*);
};

/*
    x, y are coordinates
    leave = an edge sourced at this vertex
*/
class Vertex {
public:
    HalfEdge* leave = NULL;
    double x = 0, y = 0;
    Vertex();
    Vertex(double, double);
};

class DCEL {
public:
    std::vector<HalfEdge*> edges;
    DCEL();
    DCEL(std::vector<std::pair<double, double>>&);
};


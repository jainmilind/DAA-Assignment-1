#include <vector>

class Face {
public:
    HalfEdge* edge = NULL;    
    Face();
    std::vector<Vertex*> enumerate_all_vertices();
};

class HalfEdge {
public:
    Vertex* org = NULL;
    Face* face = NULL;
    HalfEdge *twin = NULL, *nxt = NULL, *prev = NULL;
    HalfEdge();
};

class Vertex {
public:
    HalfEdge* leave = NULL;
    double x = 0, y = 0;
    Vertex();
};

class DCEL {
public:
    std::vector<HalfEdge*> edges;
    DCEL();
};


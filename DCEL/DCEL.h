#include <vector>

class Face;
class HalfEdge;
class Vertex;
class DCEL;
class Rectangle;

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
    HalfEdge* twin = NULL, * nxt = NULL, * prev = NULL;
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
    std::vector<Vertex*> corr;
    std::vector<Face*> faces;
    DCEL();
    DCEL(std::vector<std::pair<double, double>>&);
    void split(Vertex*, Vertex*);
    Face* unite(HalfEdge*);
    void print();
};

class Rectangle {
public:
    double lx, ly, ux, uy;
    Rectangle();
    Rectangle(double, double, double, double);
};


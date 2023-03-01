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

Face::Face(HalfEdge* edge) {
    this->edge = edge;
}

// takes two isolated vertices and joins them with a halfedge
HalfEdge::HalfEdge(Vertex* src, Vertex* des) {
    this->org = src;

    this->twin = new HalfEdge();
    this->twin->org = des;
    this->twin->twin = this;
}

// takes a previous halfedge and adds a new vertex
HalfEdge::HalfEdge(HalfEdge* prev, Vertex* v) {
    this->org = prev->twin->org;
    this->prev = prev;
    prev->nxt = this;

    this->twin = new HalfEdge();
    this->twin->org = v;
    this->twin->twin = this;
    this->twin->nxt = prev->twin;
    prev->twin->prev = this->twin;
}

HalfEdge::HalfEdge(HalfEdge* prev, HalfEdge* nxt) {
    this->org = prev->twin->org;
    this->nxt = nxt;
    this->prev = prev;

    this->twin = new HalfEdge();
    this->twin->org = nxt->org;
    this->twin->prev = nxt->twin;
    this->twin->nxt = prev->twin;

    prev->nxt = this;
    nxt->prev = this;

    prev->twin->prev = this->twin;
    nxt->twin->nxt = this->twin;

    Face* new_face = new Face(this);

    HalfEdge* temp = this;
    do {
        temp->face = new_face;
        temp = temp->nxt;
    } while (temp != this);
}

Vertex::Vertex(double x, double y) {
    this->x = x;
    this->y = y;
}

// points in clockwise order 
DCEL::DCEL(std::vector<std::pair<double, double>>& a) {
    int n = a.size();
    assert(n > 2 && "Please Enter Valid Polygon ;-;");

    Vertex* v1 = new Vertex(a[0].first, a[0].second);
    Vertex* v2 = new Vertex(a[1].first, a[1].second);
    this->edges.push_back(new HalfEdge(v1, v2));

    for (int i = 2; i < n; ++i) {
        Vertex* v = new Vertex(a[i].first, a[i].second);
        this->edges.push_back(new HalfEdge(this->edges.back(), v));
    }

    this->edges.push_back(new HalfEdge(this->edges.back(), this->edges[0]));
}

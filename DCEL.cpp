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
    src->leave = this;
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

    prev->twin->org->leave = this;
}

HalfEdge::HalfEdge(HalfEdge* prev, HalfEdge* nxt) {
    this->org = prev->twin->org;
    this->nxt = nxt;
    this->prev = prev;

    this->twin = new HalfEdge();
    this->twin->org = nxt->org;
    this->twin->prev = nxt->twin;
    this->twin->nxt = prev->twin;
    this->twin->twin = this;

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

    prev->twin->org->leave = this;
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

void DCEL::split(Vertex* v1, Vertex* v2) {
    HalfEdge* e1 = v1->leave;
    HalfEdge* e2 = v2->leave->twin->nxt->twin;
    HalfEdge* e3 = v1->leave;
    HalfEdge* e4 = e1->twin->next->twin;

    HalfEdge* e = new HalfEdge();
    e->org = v2;
    e->twin = new HalfEdge();
    e->twin->org = v1;

    e2->nxt = e;
    e->prev = e2;
    e1->prev = e;
    e->nxt = e1;

    e->twin->nxt = e3;
    e->twin->prev = e4;
    e3->prev = e->twin;
    e4->nxt = e->twin;

    e->face = e1->face;

    Face* new_face = new Face(e->twin);
    HalfEdge* temp = e->twin;
    do {
        temp->face = new_face;
        temp = temp->nxt;
    } while (temp != e->twin);
}

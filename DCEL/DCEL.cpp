#include <vector>
#include <iostream>
#include <cassert>
#include "DCEL.h"

Face::Face() {}
HalfEdge::HalfEdge() {}
Vertex::Vertex() {}
DCEL::DCEL() {}
Rectangle::Rectangle() {}

Rectangle::Rectangle(double lx, double ly, double ux, double uy) {
    this->lx = lx;
    this->ly = ly;
    this->ux = ux;
    this->uy = uy;
}

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

    this->corr.resize(n);
    Vertex* v1 = new Vertex(a[0].first, a[0].second);
    Vertex* v2 = new Vertex(a[1].first, a[1].second);
    this->corr[0] = v1;
    this->corr[1] = v2;
    this->edges.push_back(new HalfEdge(v1, v2));

    for (int i = 2; i < n; ++i) {
        Vertex* v = new Vertex(a[i].first, a[i].second);
        this->corr[i] = v;
        this->edges.push_back(new HalfEdge(this->edges.back(), v));
    }

    this->edges.push_back(new HalfEdge(this->edges.back(), this->edges[0]));
    this->faces.push_back(this->edges.back()->face);
}


void DCEL::split(Vertex* v1, Vertex* v2) {
    Face* cur = this->faces[0];
    HalfEdge* e1, * e2;
    HalfEdge* temp = cur->edge;
    do {
        if (temp->org == v1)
            e1 = temp;
        else if (temp->org == v2)
            e2 = temp;
        temp = temp->nxt;
    } while (temp != cur->edge);

    e1 = e1->prev;

    assert(e1 and e2);
    HalfEdge* e = new HalfEdge();
    e->twin = new HalfEdge();
    e->twin->twin = e;
    e->twin->face = e1->face;
    e1->face->edge = e->twin;

    e->org = v2;
    e->twin->org = v1;

    e2->prev->nxt = e; e->prev = e2->prev;
    e1->nxt->prev = e; e->nxt = e1->nxt;

    e2->prev = e->twin; e->twin->nxt = e2;
    e1->nxt = e->twin; e->twin->prev = e1;

    Face* new_face = new Face(e);
    temp = e;
    do {
        temp->face = new_face;
        temp = temp->nxt;
    } while (temp != e);

    this->faces.push_back(new_face);
    v1->leave = e->twin;
    return;
}

void DCEL::print() {
    // std::cout << "STARTING FACE PRINTING" << std::endl;
    std::cout << this->faces.size() << std::endl;
    for (Face* f : this->faces) {
        auto ans = f->enumerate_all_vertices();
        std::cout << ans.size() << std::endl;
        for (Vertex* v : ans) {
            std::cout << v->x << ' ' << v->y << std::endl;
        }
    }
}

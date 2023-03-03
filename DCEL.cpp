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
    if (v1->leave->twin->org == v2 or v2->leave->twin->org == v1) 
        return;

    HalfEdge* e1 = v1->leave;
    HalfEdge* e2 = v2->leave->twin->nxt->twin;
    HalfEdge* e3 = v2->leave;
    HalfEdge* e4 = e1->twin->nxt->twin;
    using namespace std;
    cout << "HI1" << endl;
    HalfEdge* e = new HalfEdge();
    e->org = v2;
    e->twin = new HalfEdge();
    e->twin->org = v1;
    e->twin->twin = e;
    v1->leave = e->twin;
    
    cout << "HI2" << endl;
    cout<<e<< e1<< e2<<e3<< e4; cout << endl;

    e2->nxt = e; e->prev = e2; 
    e2->twin->prev = e->twin; //
    cout << "HI21" << endl;
    e1->prev = e; e->nxt = e1; 
    e1->twin->nxt = e->twin; //
    cout << "HI22" << endl;

    e->twin->nxt = e3; e3->prev = e->twin;
    e3->twin->nxt = e; //
    cout << "HI23" << endl;

    e->twin->prev = e4; e4->nxt = e->twin;
    cout << "HI231" << endl;
    cout << e4 << endl;
    cout << e4->twin << endl;
    cout << e4->twin->prev << endl;
    e4->twin->prev = e; //
    cout << "HI3" << endl;

    e->face = e1->face;
    // ! added now :skull:
    e->face->edge = e1;
    cout << "HI4" << endl;

    Face* new_face = new Face(e->twin);
    HalfEdge* temp = e->twin;
    do {
        // cout << temp->org->x << ' ' << temp->org->y << endl;
        temp->face = new_face;
        temp = temp->nxt;
    } while (temp != e->twin);
    cout << "HI5" << endl;

    this->faces.push_back(new_face);
}

void DCEL::print() {
    for (Face* f : this->faces) {
        std::cout << "\nNew face starting\n";
        for (Vertex* v : f->enumerate_all_vertices()) {
            std::cout << v->x << ' ' << v->y << std::endl;
        }
    }
}

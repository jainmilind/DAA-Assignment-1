#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#include "DCEL.h"

Face::Face() {}
HalfEdge::HalfEdge() {}
Vertex::Vertex() {}
DCEL::DCEL() {}
Rectangle::Rectangle() {}

/**
 * @brief This is a constructor of Rectangle class
 * It takes two (x-y) coordinates as input and makes a rectangle using the same
 * @param lx lower left x-coordinate
 * @param ly lower left y-coordinate
 * @param ux upper right x-coordinate
 * @param uy upper right y-coordinate
 */
Rectangle::Rectangle(double lx, double ly, double ux, double uy)
{
    this->lx = lx;
    this->ly = ly;
    this->ux = ux;
    this->uy = uy;
}

/**
 * @brief This is a method in Face class
 * It returns the list of all the vertices present in a face
 * @return list of vertices
 */
std::vector<Vertex *> Face::enumerateAllVertices()
{
    std::vector<Vertex *> vertices;
    HalfEdge *curr = this->edge;
    do
    {
        vertices.push_back(curr->org);
        curr = curr->nxt;
    } while (curr != this->edge);

    return vertices;
}
/**
 * @brief This is constructor of Face class
 * It initializes edge
 * @param edge edge representing a face
 */
Face::Face(HalfEdge *edge)
{
    this->edge = edge;
}

/**
 * @brief This is a constructor of the HalfEdge Class
 * It makes the half edge between the source and destination vertex and initializes all the attributes
 * @param src source vertex
 * @param des destination vertex
 */
HalfEdge::HalfEdge(Vertex *src, Vertex *des)
{
    this->org = src;

    this->twin = new HalfEdge();
    this->twin->org = des;
    this->twin->twin = this;
    des->leave = this->twin;
}

/**
 * @brief This is a constructor of the HalfEdge Class
 * It makes the half edge between the previous HalfEdge and a vertex by initializing all the attributes
 * @param prev Previous HalgEdge
 * @param v vertex
 */
HalfEdge::HalfEdge(HalfEdge *prev, Vertex *v)
{
    this->org = prev->twin->org;
    this->prev = prev;
    prev->nxt = this;

    this->twin = new HalfEdge();
    this->twin->org = v;
    this->twin->twin = this;
    this->twin->nxt = prev->twin;
    prev->twin->prev = this->twin;

    v->leave = this->twin;
}

/**
 * @brief This is a constructor of the HalfEdge Class
 * It makes the half edge between the previous HalfEdge and the next HalfEdge by initializing all the attributes
 * @param prev Previous HalgEdge
 * @param nxt Next HalfEdge
 */
HalfEdge::HalfEdge(HalfEdge *prev, HalfEdge *nxt)
{
    this->org = prev->twin->org;
    this->nxt = nxt;
    this->prev = prev;

    this->twin = new HalfEdge();
    this->twin->twin = this;
    this->twin->org = nxt->org;
    this->twin->prev = nxt->twin;
    this->twin->nxt = prev->twin;

    prev->nxt = this;
    nxt->prev = this;

    prev->twin->prev = this->twin;
    nxt->twin->nxt = this->twin;

    Face *newFace = new Face(this);
    HalfEdge *temp = this;
    do
    {
        temp->face = newFace;
        temp = temp->nxt;
    } while (temp != this);

    nxt->org->leave = this->twin;
}

/**
 * @brief This is a constructor of the Vertex Class
 * It initializes the the x and y coordinates the vertex
 * @param x x-coordinate
 * @param y y-coordinate
 */
Vertex::Vertex(double x, double y)
{
    this->x = x;
    this->y = y;
}

/**
 * @brief This is a constructor of the DCEL class
 * It creates a DCEL data structure with all the points given in clockwise order
 * @param a list of pair of (x-coordinate,y-coordinate)
 */
DCEL::DCEL(std::vector<std::pair<double, double>> &a)
{
    int n = a.size();
    assert(n > 2 && "Please Enter Valid Polygon ;-;");

    this->corr.resize(n);
    Vertex *v1 = new Vertex(a[0].first, a[0].second);
    Vertex *v2 = new Vertex(a[1].first, a[1].second);
    this->corr[0] = v1;
    this->corr[1] = v2;
    this->edges.push_back(new HalfEdge(v1, v2));

    for (int i = 2; i < n; ++i)
    {
        Vertex *v = new Vertex(a[i].first, a[i].second);
        this->corr[i] = v;
        this->edges.push_back(new HalfEdge(this->edges.back(), v));
    }

    this->edges.push_back(new HalfEdge(this->edges.back(), this->edges[0]));
    this->faces.push_back(this->edges.back()->face);
}

/**
 * @brief This is a method of DCEL class
 * It takes two vertices as input and splits a single face into two faces
 * @param v1 First vertex
 * @param v2 Second vertex
 */
void DCEL::split(Vertex *v1, Vertex *v2)
{

    Face *cur = this->faces[0];
    HalfEdge *e1 = NULL, *e2 = NULL;
    HalfEdge *temp = cur->edge;
    do
    {
        if (temp->twin->org == v1)
            e1 = temp;
        else if (temp->org == v2)
            e2 = temp;
        temp = temp->nxt;
    } while (temp != cur->edge);

    assert(e1);
    assert(e2);

    HalfEdge *e3 = e1->nxt, *e4 = e2->prev;

    HalfEdge *e = new HalfEdge();
    e->twin = new HalfEdge();
    e->twin->twin = e;

    e->face = e->twin->face = e1->face;

    e->org = v2;
    e->twin->org = v1;

    e1->nxt->prev = e;
    e->nxt = e1->nxt;
    e2->prev->nxt = e;
    e->prev = e2->prev;

    e1->nxt = e->twin;
    e->twin->prev = e1;
    e2->prev = e->twin;
    e->twin->nxt = e2;

    e1->face->edge = e1;
    Face *newFace = new Face(e);
    temp = e;
    do
    {
        temp->face = newFace;
        temp = temp->nxt;
    } while (temp != e);

    this->edges.push_back(e);
    this->faces.push_back(newFace);

    return;
}

/**
 * @brief This is a method of DCEL class
 * It takes two faces as input and merges two faces into one
 * @param f1 First face
 * @param f2 Second face
 */
Face *DCEL::unite(Face *f1, Face *f2)
{
    HalfEdge *temp = f1->edge;
    HalfEdge *remove;
    do
    {
        if (temp->twin->face == f2)
        {
            remove = temp;
            break;
        }
        temp = temp->nxt;
    } while (temp != f1->edge);

    HalfEdge *e1 = remove->twin->prev;
    HalfEdge *e2 = remove->twin->nxt;

    remove->nxt->prev = e1;
    e1->nxt = remove->nxt;
    remove->prev->nxt = e2;
    e2->prev = remove->prev;

    Face *new_Face = new Face(e1);
    temp = e1;
    do
    {
        temp->face = new_Face;
        temp = temp->nxt;
    } while (temp != e1);

    this->faces.push_back(new_Face);

    return new_Face;
}

/**
 * @brief This is a method of DCEL class
 * It prints the vertices of the DCEL
 */
void DCEL::print()
{
    int cnt = 0;
    for (Face *f : this->faces)
        cnt += f->edge != NULL;
    std::cout << cnt << std::endl;
    for (Face *f : this->faces)
        if (f->edge != NULL)
        {
            auto ans = f->enumerateAllVertices();
            std::cout << ans.size() << std::endl;
            for (Vertex *v : ans)
            {
                std::cout << v->x << ' ' << v->y << std::endl;
            }
        }
}
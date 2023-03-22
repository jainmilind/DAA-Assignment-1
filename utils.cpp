#include <cassert>
#include <cstddef>
#include <vector>
#include <set>
#include <queue>
#include <limits.h>
#include <array>
#include <math.h>   


// measures angle in anticlockwise direction with pivot at b (degrees)
double angle(Vertex* a, Vertex* b, Vertex* c) {
    double angle1 = atan2(b->y - a->y, b->x - a->x);
    double angle2 = atan2(c->y - b->y, c->x - b->x);
    double angle = angle2 - angle1;
    if (angle > M_PI)
        angle -= 2 * M_PI;
    else if (angle <= -M_PI)
        angle += 2 * M_PI;
    return angle * 180 / M_PI + 180;
}

bool is_convex_angle(Vertex* v) {
    bool ans = true;
    Vertex* prev = v->leave->nxt->org;
    Vertex* nxt = v->leave->prev->org;

    return angle(prev, v, nxt) <= 180;
}

// Returns rectangle enclosing a list of vertices
const double inf = (double)INT64_MAX;
Rectangle get_rectangle(std::vector<Vertex*>& vertices) {
    Rectangle ans = Rectangle(inf, inf, -inf, -inf);
    for (Vertex* v : vertices) {
        ans.ux = std::max(ans.ux, v->x);
        ans.uy = std::max(ans.uy, v->y);

        ans.lx = std::min(ans.lx, v->x);
        ans.ly = std::min(ans.ly, v->y);
    }

    return ans;
}

// Check if is a point is inside a rectangle.
bool is_inside_rectangle(Rectangle& rectangle, Vertex* v) {
    return v->x > rectangle.lx
        and v->x < rectangle.ux
        and v->y > rectangle.ly
        and v->y < rectangle.uy;
}

// Check if is a point is inside a polygon formed by the given vertices.
bool is_inside_polygon(std::vector<Vertex*>& polygon, Vertex* v) {
    int n = polygon.size();
    int i = 0, j = 0;
    bool answer = false;
    for (i = 0, j = n - 1; i < n; j = i++) {
        double xi = polygon[i]->x, yi = polygon[i]->y, xj = polygon[j]->x, yj = polygon[j]->y;
        if (((yi > v->y) != (yj > v->y)) and (v->x < (xj - xi) * (v->y - yi) / (yj - yi) + xi))
            answer ^= 1;
    }
    return answer;
}

// Returns line joining v1 and v2, gives the coefficients a,b,c if the line is of the form ax + by = c
std::array<double, 3> findLine(Vertex* v1, Vertex* v2) {
    double a = v2->y - v1->y;
    double b = v1->x - v2->x;
    double c = a * (v1->x) + b * (v1->y);
    return { a, b, c };
}

/* Checks if two points a and b lie on the same side of the line joining
v1 and v2 */
bool onSameSide(Vertex* v1, Vertex* v2, Vertex* a, Vertex* b) {
    // Find the coefficient of the line joining v1 and v2.
    std::array<double, 3> coeff = findLine(v1, v2);
    double fx1 = coeff[0] * a->x + coeff[1] * a->y - coeff[2];
    double fx2 = coeff[0] * b->x + coeff[1] * b->y - coeff[2];
    if ((fx1 * fx2) > 0)
        return true;
    return false;
}

/*

*/
std::set<Vertex*> get_notches(Face* face) {
    std::vector<Vertex*> vertices = face->enumerate_all_vertices();
    std::set<Vertex*> notches;

    int n = vertices.size();
    for (int i = 0; i < n; i++) {
        int pre = (i - 1 + n) % n;
        int nxt = (i + 1) % n;
        if (angle(vertices[pre], vertices[i], vertices[nxt]) > 180)
            notches.insert(vertices[i]);
    }

    return notches;
}

Vertex* getNextVertex(Vertex* v, Face* f) {
    Vertex* u = NULL;
    HalfEdge* temp = f->edge;
    do {
        if (temp->org == v)
            u = temp->twin->org;
        temp = temp->nxt;
    } while (temp != f->edge);

    assert(u != NULL);
    return u;
}

Vertex* getPrevVertex(Vertex* v, Face* f) {
    Vertex* u = NULL;
    HalfEdge* temp = f->edge;
    do {
        if (temp->nxt->org == v)
            u = temp->org;
        temp = temp->nxt;
    } while (temp != f->edge);

    assert(temp != NULL);
    return u;
}


bool isLinear(Vertex* v1, Vertex* v2, Vertex* v3) {
    return abs((v2->y - v1->y) / (v2->x - v1->x) - (v3->y - v2->y) / (v3->x - v2->x)) < 1e-8;
}

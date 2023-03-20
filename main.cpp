#include <cassert>
#include <cstddef>
#include <vector>
#include <set>
#include <queue>
#include <limits.h>
#include <iostream>
#include <math.h>   
#include <array>
#include <map>
#include "DCEL/DCEL.cpp"


// !remove in end (for debugging only)
// using namespace std;
// template <typename A, typename B> ostream& operator<<(ostream& os, const pair<A, B>& p) { return os << "(" << p.first << ", " << p.second << ")"; }
// template <typename T_container, typename T = typename enable_if < !is_same<T_container, string>::value, typename T_container::value_type >::type > ostream& operator<<(ostream& os, const T_container& v) { os << '{'; string sep; for (const T& x : v) os << sep << x, sep = ", "; return os << '}'; }
// template <typename T> void debug_out(string s, T t) { cout << "[" << s << ": " << t << "]\n"; }
// template <typename T, typename... U> void debug_out(string s, T t, U... u) { int w = 0, c = 0; while (w < (int)s.size()) { if (s[w] == '(') c++; if (s[w] == ')') c--; if (!c and s[w] == ',') break; w++; } cout << "[" << s.substr(0, w) << ": " << t << "] "; debug_out(s.substr(w + 2, (int)s.size() - w - 1), u...); }
// #define dbg(x...) debug_out(#x, x)


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

bool is_inside_rectangle(Rectangle& rectangle, Vertex* v) {
    return v->x > rectangle.lx
        and v->x < rectangle.ux
        and v->y > rectangle.ly
        and v->y < rectangle.uy;
}

//
bool is_inside_polygon(std::vector<Vertex*>& polygon, Vertex* v) {
    int n = polygon.size();
    // if (n <= 2) return false;
    // assert(n > 2);
    int i = 0, j = 0;
    bool answer = false;
    for (i = 0, j = n - 1; i < n; j = i++) {
        double xi = polygon[i]->x, yi = polygon[i]->y, xj = polygon[j]->x, yj = polygon[j]->y;
        if (((yi > v->y) != (yj > v->y)) and (v->x < (xj - xi) * (v->y - yi) / (yj - yi) + xi))
            answer ^= 1;
    }
    return answer;
}

std::array<double, 3> findLine(Vertex* v1, Vertex* v2) {
    double a = v2->y - v1->y;
    double b = v1->x - v2->x;
    double c = a * (v1->x) + b * (v1->y);
    return { a, b, c };
}

bool onSameSide(Vertex* v1, Vertex* v, Vertex* vr, Vertex* vi) {
    std::array<double, 3> coeff = findLine(v1, v);
    double fx1 = coeff[0] * vr->x + coeff[1] * vr->y - coeff[2];
    double fx2 = coeff[0] * vi->x + coeff[1] * vi->y - coeff[2];
    if ((fx1 * fx2) > 0)
        return true;
    return false;
}

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

Vertex* getNextVertex(Vertex *v, Face *f){
    Vertex *u = NULL;
    HalfEdge *temp = f -> edge;
    do {
        if(temp -> org == v)
            u = temp -> twin -> org;
        temp = temp -> nxt;
    }while (temp != f -> edge);

    assert(u != NULL);
    return u;
}

Vertex* getPrevVertex(Vertex *v, Face *f){
    Vertex *u = NULL;
    HalfEdge *temp = f -> edge;
    do {
        if(temp -> nxt -> org == v)
            u = temp -> org;
        temp = temp -> nxt;
    }while (temp != f -> edge);

    assert(temp != NULL);
    return u;
}

bool isLinear(Vertex *v1, Vertex *v2, Vertex *v3){
    return abs((v2->y - v1->y)/(v2->x - v1->x) - (v3->y - v2->y)/(v3->x - v2->x)) < 1e-8;
}

void decomopse_mp1(DCEL& polygon) {
    std::vector<std::vector<Vertex*>> l;
    l.push_back({ polygon.corr[0] });
    int m = 1;
    int n = polygon.corr.size();

    while (n > 3) {
        std::vector<Vertex*> v(n + 2);
        v[1] = l[m - 1].back();
        v[2] = getNextVertex(v[1], polygon.faces[0]);
        l.push_back({});
        l.back() = { v[1], v[2] };
        int i = 2;
        v[3] = getNextVertex(v[2], polygon.faces[0]);


        while (((angle(v[i - 1], v[i], v[i + 1]) <= 180
            and angle(v[i], v[i + 1], v[1]) <= 180
            and angle(v[i + 1], v[1], v[2]) <= 180) 
            or isLinear(v[i - 1], v[i], v[i+1]))
            and l[m].size() < n) {

            l[m].push_back(v[i + 1]);
            i++;
            v[i + 1] = getNextVertex(v[i], polygon.faces[0]);
        }

        if (l[m].size() != n) {
            std::set<Vertex*> cur_notches = get_notches(polygon.faces[0]);
            for (Vertex* now : l[m])
                // if (cur_notches.count(now))
                    cur_notches.erase(now);

            std::deque<Vertex*> lpvs(cur_notches.begin(), cur_notches.end());
            
            while (lpvs.size() > 0) {
                auto rectangle = get_rectangle(l[m]);
                bool backward = false;

                while (!backward and lpvs.size() > 0) {
                    while (lpvs.size() > 0 and !is_inside_rectangle(rectangle, lpvs.front())) {
                        lpvs.pop_front();
                    }
                    if (lpvs.size() > 0) {
                        Vertex* V = lpvs[0];
                        if (is_inside_polygon(l[m], V)) {
                            std::set<Vertex*> vtr;
                            for (int x = 2; x < l[m].size(); ++x) {
                                if (onSameSide(l[m][0], V, l[m].back(), l[m][x]))
                                    vtr.insert(l[m][x]);
                            }
                            // vtr.insert(v[1]);
                            std::vector<Vertex*> new_l;
                            for (Vertex* i : l[m]) {
                                if (vtr.count(i)) continue;
                                new_l.push_back(i);
                            }
                            l[m].swap(new_l);
                            backward = true;
                        }
                        lpvs.pop_front();
                    }
                }
            }
        }


        if (l[m].back() != v[2]) {
            
            if (l[m].size() != n)
                polygon.split(l[m][0], l[m].back());
            else break;

            n = n - l[m].size() + 2;
        }


        m++;
    }
}

bool is_convex_angle(Vertex* v) {
    bool ans = true;
    Vertex* prev = v -> leave -> nxt -> org;
    Vertex* nxt = v -> leave -> prev -> org;

    return angle(prev, v, nxt) <= 180;
}


void merge_polygon(DCEL &polygon){

    std::vector<HalfEdge*> lle = {NULL};
    for(auto e : polygon.edges){
        if(e -> org -> leave != e && e -> twin -> org -> leave != e -> twin)
            lle.push_back(e);
    }

    std::map<Vertex*, std::set<std::pair<Vertex*, Face*>>> lp;

    for(int i = 1; i < (int)lle.size(); i++){
        HalfEdge *e = lle[i];

        Vertex *v1 = e -> org;
        Vertex *v2 = e -> twin -> org;
        
        lp[v1].insert({v2, e -> face}); lp[v2].insert({v1, e -> face});
        lp[v1].insert({v2, e -> twin -> face}); lp[v2].insert({v1, e -> twin -> face});
    }

    std::map<Face*, int> ftoi;
    std::map<int, Face*> itof;

    int temp = 1;
    for(auto face : polygon.faces){
        ftoi[face] = temp;
        itof[temp] = face;
        temp++;
    }

    int np = polygon.faces.size();
    int m = np - 1;

    std::vector<bool> ldp(np+1, true);
    std::vector<int> lup(np+1);

    for(int i = 1; i <= np; i++) lup[i] = i;

    for(int j = 1; j <= m; j++){

        Vertex *vs = lle[j] -> org;
        Vertex *vt = lle[j] -> twin -> org;

        Face *pj = lle[j] -> face;
        Face *pu = lle[j] -> twin -> face;

        if((lp[vs].size() > 2 and lp[vt].size() > 2) or
            (lp[vs].size() > 2 and is_convex_angle(vt)) or
            (lp[vt].size() > 2 and is_convex_angle(vs)) or
            (is_convex_angle(vs) and is_convex_angle(vt)))
        {
            Vertex *j2 = vt, *i2 = vs, *j3 = getNextVertex(vt, pj), *i1 = getPrevVertex(vs, pj);
            Vertex *j1 = getPrevVertex(vt, pu), *i3 = getNextVertex(vs, pu);

            if(angle(i1, i2, i3) <= 180 && angle(j1, j2, j3) <= 180){
                np = np + 1;
                
                Face *new_face = polygon.unite(pu, pj);
                ftoi[new_face] = np;
                itof[np] = new_face;

                ldp[ftoi[pj]] = false;
                ldp[ftoi[pu]] = false;
                ldp.push_back(true);

                lup[ftoi[pj]] = np;
                lup[ftoi[pu]] = np;
                lup.push_back(np);

                for(int h = 1; h < np; h++){
                    if(lup[h] == ftoi[pj] or lup[j] == ftoi[pu])
                        lup[h] = np;
                }
            }
        }
    }

    std::vector<Face*> merged_faces;
    for(int i = 1; i < ldp.size(); i++){
        if(ldp[i]){
            merged_faces.push_back(itof[i]);
        }
    }

    polygon.faces.swap(merged_faces);

}

int main() {
    int n; std::cin >> n;
    std::vector<std::pair<double, double>> a(n);
    for (auto& x : a)
        std::cin >> x.first >> x.second;

    DCEL polygon = DCEL(a);

    decomopse_mp1(polygon);

    merge_polygon(polygon);
    polygon.print();
}

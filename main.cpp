#include <vector>
#include <set>
#include <queue>
#include <limits.h>
#include <iostream>
#include <math.h>   
#include <array>
#include <map>
#include "DCEL/DCEL.cpp"

/*
// !remove in end (for debugging only)
using namespace std;
template <typename A, typename B> ostream& operator<<(ostream& os, const pair<A, B>& p) { return os << "(" << p.first << ", " << p.second << ")"; }
template <typename T_container, typename T = typename enable_if < !is_same<T_container, string>::value, typename T_container::value_type >::type > ostream& operator<<(ostream& os, const T_container& v) { os << '{'; string sep; for (const T& x : v) os << sep << x, sep = ", "; return os << '}'; }
template <typename T> void debug_out(string s, T t) { cout << "[" << s << ": " << t << "]\n"; }
template <typename T, typename... U> void debug_out(string s, T t, U... u) { int w = 0, c = 0; while (w < (int)s.size()) { if (s[w] == '(') c++; if (s[w] == ')') c--; if (!c and s[w] == ',') break; w++; } cout << "[" << s.substr(0, w) << ": " << t << "] "; debug_out(s.substr(w + 2, (int)s.size() - w - 1), u...); }
#define dbg(x...) debug_out(#x, x)
*/

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

const double inf = INT64_MAX;
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
    assert(n > 2);
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
    for (int i = 0; i < n; ++i) {
        int pre = (i - 1 + n) % n;
        int nxt = (i + 1) % n;
        if (angle(vertices[pre], vertices[i], vertices[nxt]) >= 180)
            notches.insert(vertices[i]);
    }

    return notches;
}

void decomopse_mp1(DCEL& polygon) {
    std::vector<std::vector<Vertex*>> l;
    l.push_back({ polygon.corr[0] });
    int m = 1;
    int n = polygon.corr.size();

    while (n > 3) {
        std::vector<Vertex*> v(n + 2);
        v[1] = l[m - 1].back();
        v[2] = v[1]->leave->nxt->org;
        l.push_back({});
        l.back() = { v[1], v[2] };
        int i = 2;
        v[3] = v[2]->leave->nxt->org;

        while (angle(v[i - 1], v[i], v[i + 1]) <= 180
            and angle(v[i], v[i + 1], v[1]) <= 180
            and angle(v[i + 1], v[1], v[2]) <= 180
            and l[m].size() < n) {
            l[m].push_back(v[i + 1]);

            i++;
            v[i + 1] = v[i]->leave->nxt->org;
        }

        if (l[m].size() != n) {
            std::set<Vertex*> cur_notches = get_notches(l[m][0]->leave->face);
            for (Vertex* now : l[m])
                if (cur_notches.count(now))
                    cur_notches.erase(now);

            std::deque<Vertex*> lpvs(cur_notches.begin(), cur_notches.end());
            while (lpvs.size() > 0) {
                auto rectangle = get_rectangle(l[m]);
                bool backward = false;
                while (!backward and lpvs.size() > 0) {
                    while (lpvs.size() > 0 and !is_inside_rectangle(rectangle, lpvs[0])) {
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
                            vtr.insert(v[1]);
                            std::vector<Vertex*> new_l;
                            for (Vertex* i : l[m]) {
                                if (vtr.count(i)) continue;
                                new_l.push_back(i);
                            }
                            l[m] = new_l;
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

bool is_convex_angle(Vertex* v, HalfEdge* ed) {
    bool ans = true;
    Vertex* prev = ed->twin->nxt->twin->org;
    Vertex* nxt = ed->prev->org;

    return angle(prev, v, nxt) <= 180;
}

void merging(DCEL& polygon) {
    int np = polygon.faces.size();
    int m = np - 1;

    std::vector<bool> ldp(np + 1);
    std::vector<int> lup(np + 1);

    for (int i = 1; i <= np; ++i) {
        ldp[i] = true;
        lup[i] = i;
    }

    std::vector<HalfEdge*> lle = { NULL };

    for (HalfEdge* e : polygon.edges) {
        if (e->face != NULL and e->twin->face != NULL) {
            lle.push_back(e);
        }
    }

    assert(lle.size() == np);
    std::map<Vertex*, std::vector<std::pair<Face*, Vertex*>>> lp;

    for (Face* cur_face : polygon.faces) {
        auto cur_edge = cur_face->edge;
        do {
            lp[cur_edge->org].push_back({ cur_face, cur_edge->nxt->org });
            cur_edge = cur_edge->nxt;
        } while (cur_edge != cur_face->edge);
    }

    for (int j = 1; j <= m; ++j) {
        Vertex* vs = lle[j]->org;
        Vertex* vt = lle[j]->twin->org;

        // // ! Altered 3.2
        if ((lp[vs].size() > 2 and lp[vt].size() > 2) or
            (lp[vs].size() > 2 and is_convex_angle(vt, lle[j]->twin)) or
            (lp[vt].size() > 2 and is_convex_angle(vs, lle[j])) or
            (is_convex_angle(vs, lle[j]) and is_convex_angle(vt, lle[j]->twin)))
        {
            auto j2 = vt;
            auto i2 = vs;
            auto j3 = lle[j]->nxt->twin->org; // next(pj, vt)
            auto i1 = lle[j]->prev->org; // prev(pj, vt)

            Face* u = NULL;
            int cnt = 0;
            for (auto cur : lp[vt]) {
                if (cur.second == vs and cur.first != lle[j]->face) {
                    u = cur.first;
                    cnt++;
                }
            }

            assert(u != NULL);
            assert(cnt == 1);

            int idu = std::find(polygon.faces.begin(), polygon.faces.end(), u) - polygon.faces.begin();

            auto j1 = lle[j]->twin->prev->org; // prev(pu, vt);
            auto i3 = lle[j]->twin->nxt->twin->org; // next(pu, vs);

            // // ! isn't this useless as h loop at bottom has same time complexity as number of faces
            // // TODO: use map to map face to integer 

            if (angle(i1, i2, i3) <= 180 and angle(j1, j2, j3) <= 180) {
                np++;

                // // TODO: Delete old stuff from lp
                // // ? isn't this hard / time consuming
                // // * anyways it will have same time complexity due to h loop

                // Stores all stuff i have to delete
                std::map<Vertex*, std::set<std::pair<Face*, Vertex*>>> del; 
                for (Face* old_face : { u, lle[j]->face }) {
                    auto cur_edge = old_face->edge;
                    do {
                        del[cur_edge->org].insert({ old_face, cur_edge->nxt->org });
                        cur_edge = cur_edge->nxt;
                    } while (cur_edge != old_face->edge);
                }

                for (auto &x : del) {
                    Vertex* v = x.first;
                    auto &st = x.second;

                    std::vector<std::pair<Face*, Vertex*>> take;
                    for (auto &val : lp[v]) {
                        if (!st.count(val))
                            take.push_back(val);
                    }

                    lp[v].swap(take);
                } 


                polygon.unite(lle[j]);
                // // TODO: Add new stuff in lp
                // // ? this is straight forward

                auto new_face = polygon.faces.back();
                auto cur_edge = new_face->edge;
                do {
                    lp[cur_edge->org].push_back({ new_face, cur_edge->nxt->org });
                    cur_edge = cur_edge->nxt;
                } while (cur_edge != new_face->edge);

                ldp[j] = false;
                ldp[idu] = false;
                ldp.push_back(true); // ldp[np] = true;
                lup[j] = np;
                lup[idu] = np;
            }

            for (int h = 1; h < np; ++h) {
                if (lup[h] == j or lup[h] == idu)
                    lup[h] = np;
            }
        }
    }

    // for (int i = 0; i <= m; ++i) {
    //     if (lup[i] != i) {
    //         polygon.faces.erase(std::find(polygon.faces.begin(), polygon.faces.end(), lle[i]->face));
    //     }
    // }
}


int main() {
    int n; std::cin >> n;
    std::vector<std::pair<double, double>> a(n);
    for (auto& x : a)
        std::cin >> x.first >> x.second;

    DCEL polygon = DCEL(a);

    decomopse_mp1(polygon);
    // polygon.print();
    
    merging(polygon);
    polygon.print();
}

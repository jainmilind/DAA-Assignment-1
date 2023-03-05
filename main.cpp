#include <vector>
#include <set>
#include <queue>
#include <limits.h>
#include <iostream>
#include <math.h>   
#include <array>
#include "DCEL/DCEL.cpp"

// !remove in end (for debugging only)
using namespace std;
template <typename A, typename B> ostream& operator<<(ostream& os, const pair<A, B>& p) { return os << "(" << p.first << ", " << p.second << ")"; }
template <typename T_container, typename T = typename enable_if < !is_same<T_container, string>::value, typename T_container::value_type >::type > ostream& operator<<(ostream& os, const T_container& v) { os << '{'; string sep; for (const T& x : v) os << sep << x, sep = ", "; return os << '}'; }
template <typename T> void debug_out(string s, T t) { cout << "[" << s << ": " << t << "]\n"; }
template <typename T, typename... U> void debug_out(string s, T t, U... u) { int w = 0, c = 0; while (w < (int)s.size()) { if (s[w] == '(') c++; if (s[w] == ')') c--; if (!c and s[w] == ',') break; w++; } cout << "[" << s.substr(0, w) << ": " << t << "] "; debug_out(s.substr(w + 2, (int)s.size() - w - 1), u...); }
#define dbg(x...) debug_out(#x, x)

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

// https://wrfranklin.org/Research/Short_Notes/pnpoly.html
bool is_inside_polygon(std::vector<Vertex*>& polygon, Vertex* v) {
    int n = polygon.size();
    int i = 0, j = 0;
    bool answer = false;
    for (i = 0, j = n - 1; i < n; j = i++) {
        if (((polygon[i]->y > v->y) != (polygon[j]->y > v->y)) and
            (v->x < (polygon[j]->x - polygon[i]->x) * (v->y - polygon[i]->y) / (polygon[j]->y - polygon[i]->y) + polygon[i]->x))
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
        cout << "HOI" << m << endl;
        dbg(v); cout << endl;
        dbg(l[m]); cout << endl;
        if (l[m].size() != n) {
            std::set<Vertex*> temp = get_notches(l[m][0]->leave->face);
            dbg(temp); cout << endl;
            // std::cerr << temp.size() << std::endl;
            for (Vertex* now : l[m])
                if (temp.count(now))
                    temp.erase(now);

            // for (auto now : temp)
            //     std::cerr << now << ' ';
            // std::cerr << std::endl;
            std::deque<Vertex*> lpvs(temp.begin(), temp.end());
            // dbg(lpvs); cout << endl;
            while (lpvs.size() > 0) {
                auto rectangle = get_rectangle(l[m]);
                bool backward = false;
                while (!backward and lpvs.size() > 0) {
                    dbg(lpvs, backward); cout << endl;
                    while (lpvs.size() > 0 and !is_inside_rectangle(rectangle, lpvs[0])) {
                        lpvs.pop_front();
                    }
                    dbg(lpvs); cout << endl;
                    if (lpvs.size() > 0) {
                        Vertex* V = lpvs[0];
                        // dbg(V); cout << endl;
                        if (is_inside_polygon(l[m], V)) {
                            std::set<Vertex*> vtr;
                            for (int x = 2; x < l[m].size(); ++x) {
                                if (onSameSide(l[m][0], V, l[m].back(), l[m][x]))
                                    vtr.insert(l[m][x]);
                            }
                            dbg(vtr); cout << endl;
                            std::vector<Vertex*> new_l;
                            for (Vertex* i : l[m]) {
                                if (vtr.count(i)) continue;
                                new_l.push_back(i);
                            }
                            dbg(new_l); cout << endl;
                            l[m] = new_l;
                            backward = true;
                        }
                        lpvs.pop_front();
                    }
                }
            }
        }

        if (l[m].back() != v[2]) {
            // ! l[m] is convex polygon congrats
            dbg(l[m]); cout << endl;
            // for (Vertex* x : l[m]) p.erase(x);
            // p.insert(l[m][0]);
            // p.insert(l[m].back());
            dbg(l[m][0]->x, l[m][0]->y);
            dbg(l[m].back()->x, l[m].back()->y);
            // dbg(p); cout << endl;
            if (l[m].size() != n)
                polygon.split(l[m][0], l[m].back());
            else break;
            // dbg(p); cout << endl;
            n = n - l[m].size() + 2;
        }

        dbg(m);
        dbg(polygon.faces.size());
        polygon.print();
        cout << endl;
        m++;
    }
    // std::cout<<"HI\n"<<std::endl;;;;
    // for (auto &v : l)
    //     v.clear();
    // l.clear();
}

int main() {
    int n; std::cin >> n;
    std::vector<std::pair<double, double>> a(n);
    for (auto& x : a)
        std::cin >> x.first >> x.second;

    DCEL polygon = DCEL(a);

    decomopse_mp1(polygon);
    polygon.print();
}

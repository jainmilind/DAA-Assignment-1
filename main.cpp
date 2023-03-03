#include <vector>
#include <set>
#include <queue>
#include <limits.h>
#include <iostream>
#include <math.h>
#include "DCEL.h"

//https://stackoverflow.com/questions/40286650/how-to-get-the-anti-clockwise-angle-between-two-2d-vectors

// measures angle in anticlockwise direction with pivot at b (degrees)
double angle(Vertex* a, Vertex* b, Vertex* c) {
    // TODO: implement
    return 0;
}

const double inf = INT64_MAX;
std::pair<std::pair<double, double>, std::pair<double, double>> get_rectangle(std::vector<Vertex*>& vertices) {
    std::pair<double, double> top_right = { -inf, -inf }, bot_left = { inf, inf };
    for (Vertex* v : vertices) {
        top_right.first = std::max(top_right.first, v->x);
        top_right.second = std::max(top_right.second, v->y);

        bot_left.first = std::min(bot_left.first, v->x);
        bot_left.second = std::min(bot_left.second, v->y);
    }

    return { bot_left, top_right };
}

bool is_inside_rectangle(std::pair<std::pair<double, double>, std::pair<double, double>> rectangle, Vertex* v) {
    return v->x > rectangle.first.first
        and v->x < rectangle.second.first
        and v->y > rectangle.first.second
        and v->y < rectangle.second.second;
}

bool is_inside_polygon(std::vector<Vertex*>& polygon, Vertex* v) {
    // TODO: Implement
    return true;
}

void decomopse(DCEL& polygon, std::set<Vertex*>& p, std::set<Vertex*> notches) {
    std::vector<std::vector<Vertex*>> l(1);
    l[0] = { polygon.corr[0] };
    int m = 1;
    int n = polygon.corr.size();

    while (p.size() > 3) {
        std::vector<Vertex*> v(n + 1);
        v[1] = l[m - 1].back();
        v[2] = v[1]->leave->nxt->org;
        l.push_back(std::vector<Vertex*>());
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

        if (l[m].size() != p.size()) {
            std::set<Vertex*> temp = notches;
            for (Vertex* now : l[m])
                if (temp.count(now))
                    temp.erase(now);

            std::deque<Vertex*> lpvs(temp.begin(), temp.end());
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
                                if (angle(v[1], V, v[x]) > 180) 
                                    vtr.insert(v[x]);
                            }
                            std::vector<Vertex*> new_l;
                            for (Vertex* i : l[m]) {
                                if (vtr.count(i)) continue;
                                new_l.push_back(i);
                            }
                            l[m].swap(new_l);
                            backward = true;
                            lpvs.pop_front();
                        }
                    }
                }
            }
        }

        if (l[m].back() != v[2]) {
            // ! l[m] is convex polygon congrats
            // TODO: 3.5.2
            n = n - l[m].size() + 2;
        }

        m++;
    }
}

int main() {
    int n; std::cin >> n;
    std::vector<std::pair<double, double>> a(n);
    for (auto& x : a)
        std::cin >> x.first >> x.second;

    DCEL polygon = DCEL(a);

}

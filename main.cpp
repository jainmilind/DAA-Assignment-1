#include <iostream>
#include <map>
#include "DCEL/DCEL.cpp"
#include "utils.cpp"

void decompose_mp1(DCEL& polygon) {
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
            or isLinear(v[i - 1], v[i], v[i + 1]))
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




void merge_polygon(DCEL& polygon) {

    std::vector<HalfEdge*> lle = { NULL };
    for (auto e : polygon.edges) {
        if (e->org->leave != e && e->twin->org->leave != e->twin)
            lle.push_back(e);
    }

    std::map<Vertex*, std::set<std::pair<Vertex*, Face*>>> lp;

    for (int i = 1; i < (int)lle.size(); i++) {
        HalfEdge* e = lle[i];

        Vertex* v1 = e->org;
        Vertex* v2 = e->twin->org;

        lp[v1].insert({ v2, e->face }); lp[v2].insert({ v1, e->face });
        lp[v1].insert({ v2, e->twin->face }); lp[v2].insert({ v1, e->twin->face });
    }

    std::map<Face*, int> ftoi;
    std::map<int, Face*> itof;

    int temp = 1;
    for (auto face : polygon.faces) {
        ftoi[face] = temp;
        itof[temp] = face;
        temp++;
    }

    int np = polygon.faces.size();
    int m = np - 1;

    std::vector<bool> ldp(np + 1, true);
    std::vector<int> lup(np + 1);

    for (int i = 1; i <= np; i++) lup[i] = i;

    for (int j = 1; j <= m; j++) {

        Vertex* vs = lle[j]->org;
        Vertex* vt = lle[j]->twin->org;

        Face* pj = lle[j]->face;
        Face* pu = lle[j]->twin->face;

        if ((lp[vs].size() > 2 and lp[vt].size() > 2) or
            (lp[vs].size() > 2 and is_convex_angle(vt)) or
            (lp[vt].size() > 2 and is_convex_angle(vs)) or
            (is_convex_angle(vs) and is_convex_angle(vt)))
        {
            Vertex* j2 = vt, * i2 = vs, * j3 = getNextVertex(vt, pj), * i1 = getPrevVertex(vs, pj);
            Vertex* j1 = getPrevVertex(vt, pu), * i3 = getNextVertex(vs, pu);

            if (angle(i1, i2, i3) <= 180 && angle(j1, j2, j3) <= 180) {
                np = np + 1;

                Face* new_face = polygon.unite(pu, pj);
                ftoi[new_face] = np;
                itof[np] = new_face;

                ldp[ftoi[pj]] = false;
                ldp[ftoi[pu]] = false;
                ldp.push_back(true);

                lup[ftoi[pj]] = np;
                lup[ftoi[pu]] = np;
                lup.push_back(np);

                for (int h = 1; h < np; h++) {
                    if (lup[h] == ftoi[pj] or lup[j] == ftoi[pu])
                        lup[h] = np;
                }
            }
        }
    }

    std::vector<Face*> merged_faces;
    for (int i = 1; i < ldp.size(); i++) {
        if (ldp[i]) {
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

    decompose_mp1(polygon);

    merge_polygon(polygon);
    polygon.print();
}

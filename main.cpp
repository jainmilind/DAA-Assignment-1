#include <iostream>
#include <map>
#include "DCEL/DCEL.cpp"
#include "utils.cpp"
#include <chrono>
#include <iomanip>

/**
 * This function is used to decompose the polygon using the MP1 algorithm into polygons which are convex.
 * @param polygon This is the the polygon stored as the DCEL data structure.
 */
void decomposeMp1(DCEL &polygon)
{ // l is a list of vertices to be removed from the polygon to form a new polygon at every iteration
    std::vector<std::vector<Vertex *>> l;
    // Append the list with the first vertex of the polygon
    l.push_back({polygon.corr[0]});
    // m is the number of iterations
    int m = 1;
    // n is the number of remaining vertices in a polygon
    int n = polygon.corr.size();

    std::cerr << "Number of notches: " << getNotches(polygon.faces[0]).size() << std::endl;

    while (n > 3)
    {
        // v is list of vertices
        std::vector<Vertex *> v(n + 2);
        // v[1] is the last vertex of the polygon in the previous iteration
        v[1] = l[m - 1].back();
        // v[2] is the next vertex to v[1] in the polygon to be decomposed
        v[2] = getNextVertex(v[1], polygon.faces[0]);
        l.push_back({});
        // We intialize the new polygon with v[1] and v[2]
        l.back() = {v[1], v[2]};
        int i = 2;
        // v[3] is the next vertex to v[2] in the polygon to be decomposed
        v[3] = getNextVertex(v[2], polygon.faces[0]);
        // While the next vertex, the last vertex and the first vertex of the current polygon are convex in the polygon formed after adding the next vertex, we add the next vertex into the current polygon
        while (((angle(v[i - 1], v[i], v[i + 1]) <= 180 and angle(v[i], v[i + 1], v[1]) <= 180 and angle(v[i + 1], v[1], v[2]) <= 180) or isLinear(v[i - 1], v[i], v[i + 1])) and l[m].size() < n)
        {
            l[m].push_back(v[i + 1]);
            i++;
            v[i + 1] = getNextVertex(v[i], polygon.faces[0]);
        }

        if (l[m].size() != n)
        {
            // Find all the notches of the polygon to be decomposed
            std::set<Vertex *> curNotches = getNotches(polygon.faces[0]);
            // Remove all notches that are part of the current polygon
            for (Vertex *now : l[m])
                curNotches.erase(now);
            // lpvs contains all the notches not part of the current polygon
            std::deque<Vertex *> lpvs(curNotches.begin(), curNotches.end());

            while (lpvs.size() > 0)
            {
                // Find the rectangle that encloses all the points of the current polygon
                auto rectangle = getRectangle(l[m]);
                bool backward = false;

                while (!backward and lpvs.size() > 0)
                {
                    // Keep removing all notches till we find a notch inside the rectangle
                    while (lpvs.size() > 0 and !isInsideRectangle(rectangle, lpvs.front()))
                    {
                        lpvs.pop_front();
                    }
                    if (lpvs.size() > 0)
                    {
                        // V is the notch that lies inside the rectangle
                        Vertex *V = lpvs[0];
                        // Check if the V is inside the polygon
                        if (isInsidePolygon(l[m], V))
                        {
                            // vtr is a set of vertices that are to be removed from the current polygon
                            std::set<Vertex *> vtr;

                            for (int x = 2; x < l[m].size(); ++x)
                            {
                                if (onSameSide(l[m][0], V, l[m].back(), l[m][x]))
                                    vtr.insert(l[m][x]);
                            }
                            // newLis a list of vertices that are in the updated polygon
                            std::vector<Vertex *> newL;
                            // Initialize newL
                            for (Vertex *i : l[m])
                            {
                                if (vtr.count(i))
                                    continue;
                                newL.push_back(i);
                            }
                            // Set the current polygon to be removed as the updated polygon
                            l[m].swap(newL);
                            backward = true;
                        }
                        // Remove the current notch from the list
                        lpvs.pop_front();
                    }
                }
            }
        }
        // Check if the polygon only contains only 2 vertices
        if (l[m].back() != v[2])
        {
            // Check if polygon to be removed is the entire polygon
            if (l[m].size() != n)
                // Split the polygon
                polygon.split(l[m][0], l[m].back());
            else
                break;
            // Update the number of vertices in the remaining polygon
            n = n - l[m].size() + 2;
        }
        // Update the number of iterations
        m++;
    }
}

/**
 * This function is used merge all the polygons using the merging algorithm in the DCEL which on merging still remain convex.
 * @param polygon This is the the polygon stored as the DCEL data structure.
 */
void mergePolygon(DCEL &polygon)
{

    // lle stores all the diagonals of the decomposed polygon
    std::vector<HalfEdge *> lle = {NULL};

    // Initializing lle
    for (auto e : polygon.edges)
    {
        if (e->org->leave != e && e->twin->org->leave != e->twin)
            lle.push_back(e);
    }

    // For every vertex v that is a vetex of a diagonal, lp[v] stores a pair - a vertex (the other end of the diagonal) and a face (that the diagonal is a part of)
    std::map<Vertex *, std::set<std::pair<Vertex *, Face *>>> lp;

    // Initializing lp
    for (int i = 1; i < (int)lle.size(); i++)
    {
        HalfEdge *e = lle[i];

        Vertex *v1 = e->org;
        Vertex *v2 = e->twin->org;

        lp[v1].insert({v2, e->face});
        lp[v2].insert({v1, e->face});
        lp[v1].insert({v2, e->twin->face});
        lp[v2].insert({v1, e->twin->face});
    }

    // ftoi maps every face to an integer
    std::map<Face *, int> ftoi;
    // itof unmaps an integer to its face.
    std::map<int, Face *> itof;

    int temp = 1;
    // Initialzing itof and froi
    for (auto face : polygon.faces)
    {
        ftoi[face] = temp;
        itof[temp] = face;
        temp++;
    }

    // np is the number of polygons
    int np = polygon.faces.size();
    // m is the number of diagonals
    int m = np - 1;

    // ldp is the list of faces that are part of the final merged polygon. If ldp[i] is true then the face corresponding to i is part of the final polygon
    std::vector<bool> ldp(np + 1, true);
    // lup tells if a face is a part of another face. If lup[i] = j, then the face corresponding to i is a part of the face corresponding to j.
    std::vector<int> lup(np + 1);

    // Initialze lup[i] = i, i.e every face is only part of itself.
    for (int i = 1; i <= np; i++)
        lup[i] = i;

    for (int j = 1; j <= m; j++)
    {

        // Source vertex of the diagonal.
        Vertex *vs = lle[j]->org;
        // The other end of the diagonal
        Vertex *vt = lle[j]->twin->org;

        // Face that is on the right of the line segment from vs to vt
        Face *pj = lle[j]->face;
        // Face that is on the left of the line segment from vs to vt
        Face *pu = lle[j]->twin->face;

        if ((lp[vs].size() > 2 and lp[vt].size() > 2) or
            (lp[vs].size() > 2 and isConvexAngle(vt)) or
            (lp[vt].size() > 2 and isConvexAngle(vs)) or
            (isConvexAngle(vs) and isConvexAngle(vt)))
        {
            Vertex *j2 = vt, *i2 = vs, *j3 = getNextVertex(vt, pj), *i1 = getPrevVertex(vs, pj);
            Vertex *j1 = getPrevVertex(vt, pu), *i3 = getNextVertex(vs, pu);

            if (angle(i1, i2, i3) <= 180 && angle(j1, j2, j3) <= 180)
            {
                np = np + 1;

                // We merge the two faces and get the resultant face.
                Face *newFace = polygon.unite(pu, pj);
                // Set the mapping of the new face
                ftoi[newFace] = np;
                // Set the unmapping of the new face
                itof[np] = newFace;

                // The previous faces will now not be a part of the final polygon
                ldp[ftoi[pj]] = false;
                ldp[ftoi[pu]] = false;
                // The new face is part of the final polygon.
                ldp.push_back(true);

                // The previous faces are now a part of the new polygon.
                lup[ftoi[pj]] = np;
                lup[ftoi[pu]] = np;
                lup.push_back(np);

                for (int h = 1; h < np; h++)
                {
                    if (lup[h] == ftoi[pj] or lup[j] == ftoi[pu])
                        lup[h] = np;
                }
            }
        }
    }

    // Faces that are a part of the final polygon.
    std::vector<Face *> mergedFaces;
    for (int i = 1; i < ldp.size(); i++)
    {
        if (ldp[i])
        {
            mergedFaces.push_back(itof[i]);
        }
    }

    // Set the faces of the polygon to the merged faces.
    polygon.faces.swap(mergedFaces);
}

/**
 * This function is the main function which takes input as all the vertices of the polygon and then decomposes the polygon to a set of convex polygons and merges all the unnecessary polygons and finally returns the decomposed set of convex polygons.
 */
int main()
{
    int n;
    std::cin >> n;
    std::vector<std::pair<double, double>> a(n);
    for (auto &x : a)
        std::cin >> x.first >> x.second;

    DCEL polygon = DCEL(a);

    auto startDecompose = std::chrono::high_resolution_clock::now();
    decomposeMp1(polygon);
    auto stopDecompose = std::chrono::high_resolution_clock::now();
    auto decomposeTime = std::chrono::duration_cast<std::chrono::microseconds>(stopDecompose - startDecompose);
    std::cerr << "Number of vertices: " << n << std::endl;
    int cntFaces = 0;
    for (Face *f : polygon.faces)
        cntFaces += f->edge != NULL;
    std::cerr << "Number of faces after decomposition: " << cntFaces << std::endl;

    auto startMerge = std::chrono::high_resolution_clock::now();
    mergePolygon(polygon);
    auto stopMerge = std::chrono::high_resolution_clock::now();
    auto mergeTime = std::chrono::duration_cast<std::chrono::microseconds>(stopMerge - startMerge);

    cntFaces = 0;
    for (Face *f : polygon.faces)
        cntFaces += f->edge != NULL;
    std::cerr << "Number of faces after merging: " << cntFaces << std::endl;

    std::cerr << std::setprecision(10) << "Time for decomposing is: " << (double)decomposeTime.count() / 1000 << " ms" << std::endl;
    std::cerr << std::setprecision(10) << "Time for merging is: " << (double)mergeTime.count() / 1000 << " ms" << std::endl;
    polygon.print();
}
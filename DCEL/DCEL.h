#include <vector>

class Face;
class HalfEdge;
class Vertex;
class DCEL;
class Rectangle;

/**
 * @brief This is a class representing a Face
 * It initializes edge as the halfedge representing the face
 */
class Face
{
public:
    /**
     * @brief This is any edge of face in clockwise order
     */
    HalfEdge *edge = NULL;
    /**
     * @brief This is a default constructor of Face class
     */
    Face();
    /**
     * @brief This is a parameterized constructor of Face class and takes HalfEdge as a paramter
     * @param HalfEdge HalfEdge of the face
     */
    Face(HalfEdge *);
    /**
     * @brief This is a method in Face class
     * It returns the list of all the vertices present in a face
     * @return list of vertices
     */
    std::vector<Vertex *> enumerateAllVertices();
};

/**
 * @brief This is a class representing a HalfEdge
 */
/*
   org = Source vertex
   face = face pointing in clockwise direction
   twin = halfedge in oppostie direction
   nxt = nxt halfedge in same order
   prev = previous halfedge in same order
*/
class HalfEdge
{
public:
    /**
     * @brief This is the source vertex of the HalfEdge
     */
    Vertex *org = NULL;
    /**
     * @brief This is the face pointing the clockwise direction
     */
    Face *face = NULL;
    /**
     * @brief This is the halfedge in opposite direction
     */
    HalfEdge *twin = NULL;
    /**
     * @brief This is the next halfedge in the same order
     */
    HalfEdge *nxt = NULL;
    /**
     * @brief This is the previous halfedge in the same order
     */
    HalfEdge *prev = NULL;
    /**
     * @brief This is a default constructor of HalfEdge class
     */
    HalfEdge();
    /**
     * @brief This is a constructor of HalfEdge class that creates a HalfEdge between two vertices
     */
    HalfEdge(Vertex *, Vertex *);
    /**
     * @brief This is a constructor of HalfEdge class that creates a HalfEdge between a halfedge and a vertex
     */
    HalfEdge(HalfEdge *, Vertex *);
    /**
     * @brief This is a constructor of HalfEdge class that creates a HalfEdge between two halfedges
     */
    HalfEdge(HalfEdge *, HalfEdge *);
};

/**
 * @brief This is a class representing a Vertex
 */
/*
    x, y are coordinates
    leave = an edge sourced at this vertex
*/
class Vertex
{
public:
    /**
     * @brief This is the HalfEdge leaving from the vertex
     */
    HalfEdge *leave = NULL;
    /**
     * @brief This is the x-coordinate of the vertex
     */
    double x = 0;
    /**
     * @brief This is the y-coordinate of the vertex
     */
    double y = 0;
    /**
     * @brief This is a default constructor of Vertex class
     */
    Vertex();
    /**
     * @brief This is a constructor of Vertex class that initializes the x and y coordinates of that vertex
     */
    Vertex(double, double);
};

/**
 * @brief This is a class representing a DCEL
 */
class DCEL
{
public:
    /**
     * @brief This is the list of all halfedges present in the DCEL
     */
    std::vector<HalfEdge *> edges;
    /**
     * @brief This is the list of coordinates of the DCEL
     */
    std::vector<Vertex *> corr;
    /**
     * @brief This is the list of all faces present in DCEL
     */
    std::vector<Face *> faces;
    /**
     * @brief This is a default constructor of DCEL class
     */
    DCEL();
    /**
     * @brief This is a constructor of DCEL class which constructs DCEL with all the vertices
     */
    DCEL(std::vector<std::pair<double, double>> &);
    /**
     * @brief This is a method of DCEL class which splits the face into two faces
     */
    void split(Vertex *, Vertex *);
    /**
     * @brief This is a method of DCEL class which unites two faces into a single face by taking 2 faces as input
     */
    Face *unite(Face *, Face *);
    /**
     * @brief This is a method of DCEL class which prints the vertices of the DCEL
     */
    void print();
};

/**
 * @brief This is a class representing a Rectangle
 */
class Rectangle
{
public:
    /**
     * @brief This is the lower left x-coordinate
     */
    double lx;
    /**
     * @brief This is the lower left y-coordinate
     */
    double ly;
    /**
     * @brief This is the upper right x-coordinate
     */
    double ux;
    /**
     * @brief This is the upper right y-coordinate
     */
    double uy;
    /**
     * @brief This is a default constructor of Rectangle class
     */
    Rectangle();
    /**
     * @brief This is a constructor of Rectangle class which generates the rectangle
     */
    Rectangle(double, double, double, double);
};

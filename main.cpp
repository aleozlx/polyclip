#include <iostream>
#include <list>

template <typename T>
struct Vertex2D { T x, y; };
typedef Vertex2D<float> Vertex2Df32;
typedef std::list<Vertex2Df32> Polygon;

inline float Cross(const Vertex2Df32 &a, const Vertex2Df32 &b) {
    return a.x * b.y - a.y * b.x;
}

float CrossEdge(const Vertex2Df32 &p1, const Vertex2Df32 & q0, const Vertex2Df32 & q1) {
    Vertex2Df32 a{q0.x-q1.x, q0.y-q1.y}, b{p1.x-q1.x, p1.y-q1.y};
    return Cross(a, b);
}

Vertex2Df32 Intersect(const Vertex2Df32 &p0, const Vertex2Df32 & p1, const Vertex2Df32 & q0, const Vertex2Df32 & q1) {
    return {0,0};
}

Polygon SutherlandClipping(const Polygon &polygon, const Polygon &clipper) {
    Polygon pout = polygon;
    for(auto c1 = clipper.begin(); c1!=clipper.end(); ++c1) {
        auto c0 = std::prev(c1 == clipper.begin()?clipper.end():c1);
        Polygon pin = pout;
        pout.clear();
        for(auto p1 = pin.begin(); p1!=pin.end(); ++p1) {
            auto p0 = std::prev(p1 == clipper.begin()?clipper.end():p1);
            Vertex2Df32 i = Intersect(*p0, *p1, *c0, *c1);
            if (CrossEdge(*p1, *c0, *c1)<0) {
                if (!(CrossEdge(*p0, *c0, *c1)<0))
                    pout.push_back(i);
                pout.push_back(*p1);
            }
            else if (CrossEdge(*p0, *c0, *c1)<0)
                pout.push_back(i);
        }
    }
    return pout;
}

int main(int, char const **) {
    Polygon poly1 = {{1,2},{2,1},{4,1},{4,2},{3,3}};
    Polygon poly2 = {{0,0},{2.5,0},{2.5,2.5},{0,2.5}};
    Polygon output = SutherlandClipping(poly1, poly2);
    return 0;
}

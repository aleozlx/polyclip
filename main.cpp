#include <iostream>
#include <list>

template <typename T>
struct Vertex2D { T x, y; };
typedef Vertex2D<float> Vertex2Df32;
typedef std::list<Vertex2Df32> Polygon;

void PrintPolygon(const Polygon &polygon) {
    std::cout<<"Polygon ";
    for(auto const &p: polygon) {
        std::cout<<p.x<<","<<p.y<<"  ";
    }
    std::cout<<std::endl;
}

/// a x b = det(a b) = a1 * b2 - a2 * b1
float Cross2D(const Vertex2Df32 &a, const Vertex2Df32 &b) {
    return a.x * b.y - a.y * b.x;
}

float Cross2D(float ax, float ay, float bx, float by) {
    return ax * by - ay * bx;
}

float CrossEdge(const Vertex2Df32 &p1, const Vertex2Df32 & q0, const Vertex2Df32 & q1) {
    return Cross2D({q0.x-q1.x, q0.y-q1.y}, {p1.x-q1.x, p1.y-q1.y});
}

Vertex2Df32 Intersect(const Vertex2Df32 &p0, const Vertex2Df32 & p1, const Vertex2Df32 & q0, const Vertex2Df32 & q1) {
    float dp = Cross2D(p0, p1), dq = Cross2D(q0, q1),
        x = Cross2D(dp, dq, p0.x-p1.x, q0.x-q1.x),
        y = Cross2D(dp, dq, p0.y-p1.y, q0.y-q1.y),
        n = Cross2D({p0.x-p1.x, p0.y-p1.y}, {q0.x-q1.x, q0.y-q1.y});
    return {x/n, y/n};
}

Polygon SutherlandClipping(const Polygon &polygon, const Polygon &clipper) {
    Polygon pout = polygon;
    for(auto c1 = clipper.begin(); c1!=clipper.end(); ++c1) {
        auto c0 = std::prev(c1 == clipper.begin()?clipper.end():c1);
        Polygon pin = pout;
        PrintPolygon(pin);
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
    PrintPolygon(output);
    return 0;
}

#include <iostream>
#include <list>
#include <vector>

#define Cross2D(ax, ay, bx, by) ((ax) * (by) - (ay) * (bx))

template <typename T>
struct Edge;

template<typename T>
struct BoxEdge {
    enum { Left, Bottom, Right, Top } side;
    union { T x; T y; };
};

template<typename E>
using Clipper = std::list<E>;

template<typename T>
struct Box {
    T xmin, ymin, xmax, ymax;

    Clipper<BoxEdge<T>> asClipper() const {
        return {
            { BoxEdge<T>::Left, xmin },
            { BoxEdge<T>::Bottom, ymin },
            { BoxEdge<T>::Right, xmax },
            { BoxEdge<T>::Top, ymax },
        };
    }
};

template <typename T>
struct Vertex2D {
    T x, y;

    Vertex2D<T> operator-(Vertex2D<T> const &b) const {
        return { x-b.x, y-b.y };
    }

    /// a x b = det(a b) = a1 * b2 - a2 * b1
    T cross(Vertex2D<T> const &b) const {
        return Cross2D(x, y, b.x, b.y);
    }

    /// <0: left of the edge; >0: right of the edge; =0: on the edge
    T cross(Vertex2D<T> const &q0, Vertex2D<T> const &q1) const {
        return (q0-q1).cross(*this-q1);
    }

    T cross(Edge<T> const &edge) const {
        return (edge.p0-edge.p1).cross(*this-edge.p1);
    }

    bool left_of(Edge<T> const &edge) const {
        return cross(edge)<0;
    }

    bool left_of(BoxEdge<T> const &edge) const {
        switch(edge.side) {
            case BoxEdge<T>::Left: return x > edge.x;
            case BoxEdge<T>::Bottom: return y > edge.y;
            case BoxEdge<T>::Right: return x < edge.x;
            case BoxEdge<T>::Top: return y < edge.y;
        }
    }
};

template <typename T>
struct Edge {
    Vertex2D<T> p0, p1;

    static Vertex2D<T> intersect(Vertex2D<T> const &p0, Vertex2D<T> const &p1, Vertex2D<T> const &q0, Vertex2D<T> const &q1) {
        T dp = p0.cross(p1), dq = q0.cross(q1),
            x = Cross2D(dp, dq, p0.x-p1.x, q0.x-q1.x),
            y = Cross2D(dp, dq, p0.y-p1.y, q0.y-q1.y),
            n = (p0-p1).cross(q0-q1);
        return {x/n, y/n};
    }

    Vertex2D<T> intersect(Edge<T> const &edge) {
        return intersect(this->p0, this->p1, edge.p0, edge.p1);
    }

    Vertex2D<T> intersect(BoxEdge<T> const &edge) {
        Vertex2D<T> d = p1-p0;
        switch(edge.side) {
            case BoxEdge<T>::Top:
            case BoxEdge<T>::Bottom:
                return {p0.x + ((edge.y - p0.y) / d.y)*d.x, edge.y};
            case BoxEdge<T>::Left:
            case BoxEdge<T>::Right:
                return {edge.x, p0.y + ((edge.x - p0.x) / d.x)*d.y};
        }
    }
};

template<typename T>
using Polygon = std::list<Vertex2D<T>>;

template<typename T>
void printPolygon(const Polygon<T> &polygon) {
    std::cout<<"Polygon ";
    for(auto const &p: polygon)
        std::cout<<p.x<<","<<p.y<<"  ";
    std::cout<<std::endl;
}

template<typename T>
Clipper<Edge<T>> cvtPolygonToClipper(const Polygon<T> &clipper) {
    Clipper<Edge<T>> ret;
    for(auto c1 = clipper.begin(); c1!=clipper.end(); ++c1) {
        auto c0 = std::prev(c1 == clipper.begin()?clipper.end():c1);
        ret.push_back({*c0, *c1});
    }
    return ret;
}

template<typename T, typename C>
Polygon<T> SutherlandClipping(Polygon<T> const &polygon, C const &clipper) {
    Polygon<T> pout = polygon;
    for(auto clipper_edge: clipper) {
        Polygon<T> pin = pout;
        // printPolygon(pin);
        pout.clear();
        for(auto p1 = pin.begin(); p1!=pin.end(); ++p1) {
            auto p0 = std::prev(p1 == pin.begin()?pin.end():p1);
            Edge<T> edge = {*p0, *p1};
            if (p1->left_of(clipper_edge)) {
                if (!(p0->left_of(clipper_edge)))
                    pout.push_back(edge.intersect(clipper_edge));
                pout.push_back(*p1);
            }
            else if (p0->left_of(clipper_edge))
                pout.push_back(edge.intersect(clipper_edge));
        }
    }
    return pout;
}

int main(int, char const **) {
    Box<float> box = {0,0,2.5,2.5};
    Polygon<float> poly1 = {{1,2},{2,1},{4,1},{4,2},{3,3}},
        poly2 = {{0,0},{2.5,0},{2.5,2.5},{0,2.5}},
        output1 = SutherlandClipping(poly1, cvtPolygonToClipper(poly2)),
        output2 = SutherlandClipping(poly1, box.asClipper());
    printPolygon(output1);
    printPolygon(output2);
    return 0;
}

#include <iostream>
#include <list>
#include <vector>

template<typename E>
using Clipper = std::list<E>;

template<typename T>
struct Box {
    T xmin, ymin, xmax, ymax;

    struct Edge {
        enum { Left, Bottom, Right, Top } side;
        union { T x; T y; };
    };

    Clipper<Edge> asClipper() const {
        return {
            { Edge::Left, xmin },
            { Edge::Bottom, ymin },
            { Edge::Right, xmax },
            { Edge::Top, ymax },
        };
    }
};

template<typename T>
struct Polygon;

template <typename T>
struct Vertex2D {
    T x, y;

    Vertex2D<T> operator-(Vertex2D<T> const &b) const {
        return { x-b.x, y-b.y };
    }

    /// a x b = det(a b) = a1 * b2 - a2 * b1
    T operator*(Vertex2D<T> const &b) const {
        return x*b.y-y*b.x;
    }

    Vertex2D<T> operator/(T normalizer) const {
        return {x/normalizer, y/normalizer};
    }

    /// <0: left of the edge; >0: right of the edge; =0: on the edge
    // T cross(Vertex2D<T> const &q0, Vertex2D<T> const &q1) const {
    //     return (q0-q1)*(*this-q1);
    // }

    bool operator<(typename Polygon<T>::Edge const &edge) const {
        return (edge.p0-edge.p1)*(*this-edge.p1)<0;
    }

    bool operator<(typename Box<T>::Edge const &edge) const {
        switch(edge.side) {
            case Box<T>::Edge::Left: return x > edge.x;
            case Box<T>::Edge::Bottom: return y > edge.y;
            case Box<T>::Edge::Right: return x < edge.x;
            case Box<T>::Edge::Top: return y < edge.y;
        }
    }
};

template<typename V>
V intersect4p(V const &p0, V const &p1, V const &q0, V const &q1) {
    V p = p0-p1, q = q0-q1, v = {p.x, q.x}, w = {p.y, q.y}, d = {p0*p1, q0*q1}, z = {d*v, d*w};
    return z/(p*q);
}

template<typename T>
struct Polygon {
    std::list<Vertex2D<T>> data;

    Polygon(std::list<Vertex2D<T>> const &data): data(data) {

    }

    Polygon(Box<T> const &box): data({
        {box.xmin, box.ymin},
        {box.xmax, box.ymin},
        {box.xmax, box.ymax},
        {box.xmin, box.ymax}
    }) {

    }

    void debug() {
        std::cout<<"Polygon ";
        for(auto const &p: this->data)
            std::cout<<p.x<<","<<p.y<<"  ";
        std::cout<<std::endl;
    }

    struct Edge {
        Vertex2D<T> p0, p1;

        Vertex2D<T> operator*(Edge const &edge) {
            return intersect4p(this->p0, this->p1, edge.p0, edge.p1);
        }

        Vertex2D<T> operator*(typename Box<T>::Edge const &edge) {
            Vertex2D<T> d = p1-p0;
            switch(edge.side) {
                case Box<T>::Edge::Top:
                case Box<T>::Edge::Bottom:
                    return {p0.x + ((edge.y - p0.y) / d.y)*d.x, edge.y};
                case Box<T>::Edge::Left:
                case Box<T>::Edge::Right:
                    return {edge.x, p0.y + ((edge.x - p0.x) / d.x)*d.y};
            }
        }
    };

    std::list<Edge> GetEdges() const {
        Clipper<Edge> ret;
        for(auto c1 = data.begin(); c1!=data.end(); ++c1) {
            auto c0 = std::prev(c1 == data.begin()?data.end():c1);
            ret.push_back({*c0, *c1});
        }
        return ret;
    }

    Clipper<Edge> asClipper() const {
        return GetEdges();
    }
};

template<typename P, typename C>
P SutherlandClipping(P const &polygon, C const &clipper) {
    auto pout = polygon.data;
    for(auto clipper_edge: clipper) {
        auto pin = pout;
        pout.clear();
        for(auto edge: P(pin).GetEdges()) {
            if (edge.p1<clipper_edge) {
                if (!(edge.p0<clipper_edge))
                    pout.push_back(edge*clipper_edge);
                pout.push_back(edge.p1);
            }
            else if (edge.p0<clipper_edge)
                pout.push_back(edge*clipper_edge);
        }
    }
    return P(pout);
}

// 2.5,2.5  2,2.5  1,2  2,1  2.5,1

int main(int, char const **) {
    Box<float> box = {0,0,2.5,2.5};
    Polygon<float> poly1({{1,2},{2,1},{4,1},{4,2},{3,3}}), poly2(box);
    SutherlandClipping(poly1, poly2.asClipper()).debug();
    SutherlandClipping(poly1, box.asClipper()).debug();
    return 0;
}

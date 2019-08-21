#include <iostream>
#include <list>
#include <vector>

#define Cross2D(ax, ay, bx, by) ((ax) * (by) - (ay) * (bx))

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
    T cross(Vertex2D<T> const &b) const {
        return Cross2D(x, y, b.x, b.y);
    }

    /// <0: left of the edge; >0: right of the edge; =0: on the edge
    T cross(Vertex2D<T> const &q0, Vertex2D<T> const &q1) const {
        return (q0-q1).cross(*this-q1);
    }

    T cross(typename Polygon<T>::Edge const &edge) const {
        return (edge.p0-edge.p1).cross(*this-edge.p1);
    }

    bool left_of(typename Polygon<T>::Edge const &edge) const {
        return cross(edge)<0;
    }

    bool left_of(typename Box<T>::Edge const &edge) const {
        switch(edge.side) {
            case Box<T>::Edge::Left: return x > edge.x;
            case Box<T>::Edge::Bottom: return y > edge.y;
            case Box<T>::Edge::Right: return x < edge.x;
            case Box<T>::Edge::Top: return y < edge.y;
        }
    }
};

template<typename T>
struct Polygon {
    std::list<Vertex2D<T>> data;

    Polygon(std::list<Vertex2D<T>> const &data): data(data) {

    }

    Polygon(Box<T> const &box) {
        data.push_back({box.xmin, box.ymin});
        data.push_back({box.xmax, box.ymin});
        data.push_back({box.xmax, box.ymax});
        data.push_back({box.xmin, box.ymax});
    }

    void debug() {
        std::cout<<"Polygon ";
        for(auto const &p: this->data)
            std::cout<<p.x<<","<<p.y<<"  ";
        std::cout<<std::endl;
    }

    struct Edge {
        Vertex2D<T> p0, p1;

        static Vertex2D<T> intersect(Vertex2D<T> const &p0, Vertex2D<T> const &p1, Vertex2D<T> const &q0, Vertex2D<T> const &q1) {
            T dp = p0.cross(p1), dq = q0.cross(q1),
                x = Cross2D(dp, dq, p0.x-p1.x, q0.x-q1.x),
                y = Cross2D(dp, dq, p0.y-p1.y, q0.y-q1.y),
                n = (p0-p1).cross(q0-q1);
            return {x/n, y/n};
        }

        Vertex2D<T> intersect(Edge const &edge) {
            return intersect(this->p0, this->p1, edge.p0, edge.p1);
        }

        Vertex2D<T> intersect(typename Box<T>::Edge const &edge) {
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

    Clipper<Edge> asClipper() const {
        Clipper<Edge> ret;
        for(auto c1 = data.begin(); c1!=data.end(); ++c1) {
            auto c0 = std::prev(c1 == data.begin()?data.end():c1);
            ret.push_back({*c0, *c1});
        }
        return ret;
    }
};

template<typename P, typename C>
P SutherlandClipping(P const &polygon, C const &clipper) {
    auto pout = polygon.data;
    for(auto clipper_edge: clipper) {
        auto pin = pout;
        pout.clear();
        for(auto p1 = pin.begin(); p1!=pin.end(); ++p1) {
            auto p0 = std::prev(p1 == pin.begin()?pin.end():p1);
            typename P::Edge edge = {*p0, *p1};
            if (p1->left_of(clipper_edge)) {
                if (!(p0->left_of(clipper_edge)))
                    pout.push_back(edge.intersect(clipper_edge));
                pout.push_back(*p1);
            }
            else if (p0->left_of(clipper_edge))
                pout.push_back(edge.intersect(clipper_edge));
        }
    }
    return P(pout);
}

int main(int, char const **) {
    Box<float> box = {0,0,2.5,2.5};
    Polygon<float> poly1({{1,2},{2,1},{4,1},{4,2},{3,3}}), poly2(box);
    SutherlandClipping(poly1, poly2.asClipper()).debug();
    SutherlandClipping(poly1, box.asClipper()).debug();
    return 0;
}

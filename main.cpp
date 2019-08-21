#include <iostream>
#include <list>

template <typename T>
inline T Cross2D(T ax, T ay, T bx, T by) {
    return ax * by - ay * bx;
}

template <typename T>
struct Vertex2D {
    T x, y;

    Vertex2D<T> operator-(Vertex2D<T> const &b) const {
        return { this->x-b.x, this->y-b.y };
    }

    /// a x b = det(a b) = a1 * b2 - a2 * b1
    T operator*(Vertex2D<T> const &b) const {
        return Cross2D(this->x, this->y, b.x, b.y);
    }

    /// <0: left of the edge; >0: right of the edge; =0: on the edge
    T CrossEdge(Vertex2D<T> const &q0, Vertex2D<T> const &q1) const {
        return (q0-q1) * (*this-q1);
    }

    static Vertex2D<T> Intersect(const Vertex2D<T> &p0, const Vertex2D<T> & p1, const Vertex2D<T> & q0, const Vertex2D<T> & q1) {
        T dp = p0 * p1, dq = q0 * q1,
            x = Cross2D(dp, dq, p0.x-p1.x, q0.x-q1.x),
            y = Cross2D(dp, dq, p0.y-p1.y, q0.y-q1.y),
            n = (p0-p1) * (q0-q1);
        return {x/n, y/n};
    }
};

template<typename T>
using Polygon = std::list<Vertex2D<T>>;

template<typename T>
void PrintPolygon(const Polygon<T> &polygon) {
    std::cout<<"Polygon ";
    for(auto const &p: polygon)
        std::cout<<p.x<<","<<p.y<<"  ";
    std::cout<<std::endl;
}

template<typename T>
Polygon<T> SutherlandClipping(const Polygon<T> &polygon, const Polygon<T> &clipper) {
    Polygon<T> pout = polygon;
    for(auto c1 = clipper.begin(); c1!=clipper.end(); ++c1) {
        auto c0 = std::prev(c1 == clipper.begin()?clipper.end():c1);
        Polygon<T> pin = pout;
        // PrintPolygon(pin);
        pout.clear();
        for(auto p1 = pin.begin(); p1!=pin.end(); ++p1) {
            auto p0 = std::prev(p1 == pin.begin()?pin.end():p1);
            auto i = Vertex2D<T>::Intersect(*p0, *p1, *c0, *c1);
            if (p1->CrossEdge(*c0, *c1)<0) {
                if (!(p0->CrossEdge(*c0, *c1)<0))
                    pout.push_back(i);
                pout.push_back(*p1);
            }
            else if (p0->CrossEdge(*c0, *c1)<0)
                pout.push_back(i);
        }
    }
    return pout;
}

int main(int, char const **) {
    Polygon<float> poly1 = {{1,2},{2,1},{4,1},{4,2},{3,3}},
        poly2 = {{0,0},{2.5,0},{2.5,2.5},{0,2.5}},
        output = SutherlandClipping(poly1, poly2);
    PrintPolygon(output);
    return 0;
}

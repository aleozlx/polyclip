#include <iostream>
#include <vector>

template <typename T>
struct Vertex2D { T x, y; };

typedef std::vector<Vertex2D<float>> Ploygon;

int main(int, char const **) {
    Ploygon ploy1 = {{1,2},{2,1},{4,1},{4,2},{3,3}};
    Ploygon ploy2 = {{0,0},{2.5,0},{2.5,2.5},{0,2.5}};
    return 0;
}

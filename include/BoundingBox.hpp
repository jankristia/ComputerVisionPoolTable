#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP

struct BoundingBox {
    int x;
    int y;
    int width;
    int height;
    int ballType; // 0 = background 1 = white ball, 2 = black, 3 = solid, 4 = striped, 5 = playing field
};

#endif
// Author Jan Kristian Alstergren
#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP

struct BoundingBox {
    int x;
    int y;
    int width;
    int height;
    int ballType; // 0 = background 1 = white ball, 2 = black, 3 = solid, 4 = striped, 5 = playing field
     // Constructor
    BoundingBox(int _x, int _y, int _width, int _height, int _ballType)
        : x(_x), y(_y), width(_width), height(_height), ballType(_ballType) {}
};

#endif
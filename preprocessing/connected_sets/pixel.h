#pragma once

#include <iostream>

const int INITIAL_LABEL = -1;
const int BLACK = 0;
const int WHITE = 255;

class Pixel{
private:
    int row;
    int col;
    int value;
public:
    int label;
    Pixel(){}
    Pixel(int row_,int col_, int value_);
    int get_row() const;
    int get_col() const;
    bool is_black() const;
    bool has_label() const;
};

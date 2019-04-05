#include "pixel.h"

#include <algorithm>

Pixel::Pixel(int row_, int col_, int value_) {
    row = row_;
    col = col_;
    value = value_;
    label = INITIAL_LABEL;
}

int Pixel::get_row() const {
    return row;
}

int Pixel::get_col() const {
    return col;
}

bool Pixel::is_black() const {
    return value == BLACK;
}

bool Pixel::has_label() const {
    return label != INITIAL_LABEL;
}

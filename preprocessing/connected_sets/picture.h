#pragma once

#include "pixel.h"

#include <Imagine/Graphics.h> // Import byte.
#include <cassert> // Import assert.


class Picture{
private:
    int height;
    int width;
    Pixel* tab;
public:
    Picture(byte* image, int height_,int width_);
    ~Picture();
    int get_height() const;
    int get_width() const;
    bool is_black(int row, int col) const;
    int get_label(int row, int col) const;
    void set_label(int row, int col, int label_);
    bool has_label(int row, int col) const;
    void update_neighbours(int row, int col); // Propagate label to black neighbours.
};

#pragma once

#include "pixel.h"
#include "picture.h"

class ConnectedSet{
private:
    int upper_row;
    int left_col;
    int bottom_row;
    int right_col;
    int pixel_count;
public:
    // Corners are initialized at most restricting values.
    ConnectedSet(int picture_width, int picture_height, int pixel_count_);
    int get_upper_row() const;
    void set_upper_row(int value);
    int get_left_col() const;
    void set_left_col(int value);
    int get_bottom_row() const;
    void set_bottom_row(int value);
    int get_right_col() const;
    void set_right_col(int value);
    int get_pixel_count() const;
    int get_height() const;
    int get_width() const;
    int get_size() const;
};

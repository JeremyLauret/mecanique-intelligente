#include "connected_set.h"

ConnectedSet::ConnectedSet(int picture_width, int picture_height, int pixel_count_) {
    upper_row = picture_height - 1;
    left_col = picture_width - 1;
    bottom_row = 0;
    right_col = 0;
    pixel_count = pixel_count_;
}

int ConnectedSet::get_upper_row() const {
    return upper_row;
}

void ConnectedSet::set_upper_row(int value) {
    assert (value >= 0);
    upper_row = value;
}

int ConnectedSet::get_left_col() const {
    return left_col;
}

void ConnectedSet::set_left_col(int value) {
    assert (value >= 0);
    left_col = value;
}

int ConnectedSet::get_bottom_row() const {
    return bottom_row;
}

void ConnectedSet::set_bottom_row(int value) {
    assert (value >= 0);
    bottom_row = value;
}

int ConnectedSet::get_right_col() const {
    return right_col;
}

void ConnectedSet::set_right_col(int value) {
    assert (value >= 0);
    right_col = value;
}

int ConnectedSet::get_pixel_count() const {
    return pixel_count;
}

int ConnectedSet::get_height() const {
    return bottom_row - upper_row + 1;
}

int ConnectedSet::get_width() const {
    return right_col - left_col + 1;
}

int ConnectedSet::get_size() const {
    return get_width() * get_height();
}

#include "picture.h"

Picture::Picture(byte* image, int height_,int width_) {
    height = height_;
    width = width_;
    tab = new Pixel[height * width];
    for (int row = 0 ; row < height ; row++) {
        for (int col = 0 ; col < width ; col++) {
            Pixel pixel(row, col, (int)image[row + height * col]);
            tab[row + height * col] = pixel;
        }
    }
}

Picture::~Picture(){
    delete[] tab;
}

int Picture::get_height() const {
    return height;
}

int Picture::get_width() const {
    return width;
}

bool Picture::is_black(int row,  int col) const {
    assert (row >= 0 && row < height && col >= 0 && col < width);
    return tab[row + height * col].is_black();
}

int Picture::get_label(int row, int col) const {
    assert (row >= 0 && row < height && col >= 0 && col < width);
    return tab[row + height * col].label;
}

void Picture::set_label(int row, int col, int label_) {
    assert (row >= 0 && row < height && col >= 0 && col < width);
    tab[row + height * col].label = label_;
}

bool Picture::has_label(int row, int col) const {
    assert (row >= 0 && row < height && col >= 0 && col < width);
    return tab[row + height * col].has_label();
}

void Picture::update_neighbours(int row, int col) {
    assert (row >= 0 && row < height && col >= 0 && col < width);
    int center_label = get_label(row, col); // Label to be propagated.
    if (row > 0) {
        if (is_black(row - 1, col) && !has_label(row - 1, col)) { // Up.
            set_label(row - 1, col, center_label);
            update_neighbours(row - 1, col);
        }
        if (col > 0) {
            if (is_black(row - 1, col - 1) && !has_label(row - 1, col - 1)) { // Up-left.
                set_label(row - 1, col - 1, center_label);
                update_neighbours(row - 1, col - 1);
            }
        }
        if (col < width - 1) {
            if (is_black(row - 1, col + 1) && !has_label(row - 1, col + 1)) { // Up-right.
                set_label(row - 1, col + 1, center_label);
                update_neighbours(row - 1, col + 1);
            }
        }
    }
    if (row < height - 1) {
        if (is_black(row + 1, col) && !has_label(row + 1, col)) { // Down.
            set_label(row + 1, col, center_label);
            update_neighbours(row + 1, col);
        }
        if (col > 0) {
            if (is_black(row + 1, col - 1) && !has_label(row + 1, col - 1)) { // Down-left.
                set_label(row + 1, col - 1, center_label);
                update_neighbours(row + 1, col - 1);
            }
        }
        if (col < width - 1) {
            if (is_black(row + 1, col + 1) && !has_label(row + 1, col + 1)) { // Down-right.
                set_label(row + 1, col + 1, center_label);
                update_neighbours(row + 1, col + 1);
            }
        }
    }
    if (col > 0) {
        if (is_black(row, col - 1) && !has_label(row, col - 1)) { // Left.
            set_label(row, col - 1, center_label);
            update_neighbours(row, col - 1);
        }
    }
    if (col < width - 1) {
        if (is_black(row, col + 1) && !has_label(row, col + 1)) { // Right.
            set_label(row, col + 1, center_label);
            update_neighbours(row, col + 1);
        }
    }
}

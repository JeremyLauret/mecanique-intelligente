#include "picture.h"

Picture::Picture(byte* image, int height_,int width_) {
    height = height_;
    width = width_;
    tab = new Pixel[height * width];
    for (int row = 0 ; row < height ; row++) {
        for (int col = 0 ; col < width ; col++) {
            Pixel pixel(row, col, (int)image[col + width * row]);
            tab[col + width * row] = pixel;
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
    return tab[col + width * row].is_black();
}

void Picture::set_white(int row, int col) {
    assert (row >= 0 && row < height && col >= 0 && col < width);
    return tab[col + width * row].set_white();
}

int Picture::get_label(int row, int col) const {
    assert (row >= 0 && row < height && col >= 0 && col < width);
    return tab[col + width * row].label;
}

void Picture::set_label(int row, int col, int label_) {
    assert (row >= 0 && row < height && col >= 0 && col < width);
    tab[col + width * row].label = label_;
}

bool Picture::has_label(int row, int col) const {
    assert (row >= 0 && row < height && col >= 0 && col < width);
    return tab[col + width * row].has_label();
}

void Picture::update_neighbours(int row, int col, int range) {
    assert (row >= 0 && row < height && col >= 0 && col < width && range > 0);
    int center_label = get_label(row, col); // Label to be propagated.
    for (int delta_row = -range ; delta_row < range + 1 ; delta_row++) {     // For each neighbour
        for (int delta_col = -range ; delta_col < range + 1 ; delta_col++) { // pixel.
            if (row + delta_row >= 0 &&
                row + delta_row < height &&
                col + delta_col >= 0 &&
                col + delta_col < width) { // Pixel is in the window.
                if (is_black(row + delta_row, col + delta_col) &&   // Pixel is black.
                    !has_label(row + delta_row, col + delta_col)) { // Pixel has no label.
                    set_label(row + delta_row, col + delta_col, center_label);
                    update_neighbours(row + delta_row, col + delta_col, range);
                }
            }
        }
    }
}

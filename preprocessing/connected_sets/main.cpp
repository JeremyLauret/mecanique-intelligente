#include "pixel.h"
#include "picture.h"
#include "connected_set.h"

#include <vector>
#include <algorithm>

/*
 * Construct a binary image from the input.
 *
 * The maximum value in the input is interpreted as white. Everything else is cast to black.
 */
byte* to_binary(byte* image, int height, int width) {
    // ---- Find the maximal value in the input ----
    int max_value = (byte)0;
    for (int row = 0 ; row < height ; row++) {
        for (int col = 0 ; col < width ; col++) {
            if (max_value < image[col + width * row]) {
                max_value = image[col + width * row];
            }
        }
    }
    // ---- Fill the binary image ----
    byte* binary_image = new byte[height * width];
    for (int row = 0 ; row < height ; row++) {
        for (int col = 0 ; col < width ; col++) {
            binary_image[col + width * row] = (
                max_value > image[col + width * row] ?
                (byte)BLACK :
                (byte)WHITE
            );
        }
    }
    return binary_image;
}

/*
 * Return a colored image from the labeled picture.
 *
 * Each connected set of the picture is mapped to a color.
 */
byte* to_color_image(const Picture& picture, int nb_labels) {
    int height = picture.get_height();
    int width = picture.get_width();
    // ---- Map labels to colors ----
    std::vector<byte> red_shades;
    std::vector<byte> green_shades;
    std::vector<byte> blue_shades;
    for (int label_count = 0; label_count < nb_labels ; label_count++) {
        red_shades.push_back((byte)(std::rand() % WHITE));
        green_shades.push_back((byte)(std::rand() % WHITE));
        blue_shades.push_back((byte)(std::rand() % WHITE));
    }
    // ---- Create RGB arrays depending on the pixel label ----
    byte* red = new byte[height * width];
    byte* green = new byte[height * width];
    byte* blue  = new byte[height * width];
    for (int row = 0 ; row < height ; row++) {
        for (int col = 0 ; col < width ; col++) {
            if (!picture.has_label(row, col)) {
                red[col + width * row] = (byte)WHITE;
                green[col + width * row] = (byte)WHITE;
                blue[col + width * row] = (byte)WHITE;
                continue;
            }
            red[col + width * row] = red_shades[picture.get_label(row, col)];
            green[col + width * row] = green_shades[picture.get_label(row, col)];
            blue[col + width * row] = blue_shades[picture.get_label(row, col)];
        }
    }
    // ---- Create and fill the colored image ----
    byte* image = new byte[3 * height * width];
    for (int i = 0 ; i < width * height ; i++) {
        image[3 * i]     = red[i];
        image[3 * i + 1] = green[i];
        image[3 * i + 2] = blue[i];
    }
    // ---- Cleanup ----
    delete[] red;
    delete[] green;
    delete[] blue;
    return image;
}

/*
 * Return a vector containing the number of pixels of each connected set.
 */
std::vector<int> count_pixels(const Picture& labeled_picture, int nb_labels) {
    std::vector<int> pixel_counts;
    for (int label = 0 ; label < nb_labels ; label++) {
        pixel_counts.push_back(0);
    }
    for (int row = 0 ; row < labeled_picture.get_height() ; row++) {
        for (int col = 0 ; col < labeled_picture.get_width() ; col++) {
            if (labeled_picture.has_label(row, col)) {
                pixel_counts[labeled_picture.get_label(row, col)]++;
            }
        }
    }
    return pixel_counts;
}


/*
 * Remove smaller connected sets from the picture.
 *
 * Each connected set with strictly less than min_pixels_nb pixels is removed
 * from the picture : its pixels are set to white and labels to -1.
 * The pixel_counts vector and nb_pixels integer are updated.
 */
void remove_smaller_sets(
    Picture& labeled_picture,
    std::vector<int>& pixel_counts,
    int& nb_labels,
    int min_pixel_nb
) {
    // ---- Unlabel and turn to white pixels from smaller connected sets ----
    for (int row = 0 ; row < labeled_picture.get_height() ; row++) {
        for (int col = 0 ; col < labeled_picture.get_width() ; col++) {
            if (!labeled_picture.has_label(row, col)) {
                continue;
            }
            if (pixel_counts[labeled_picture.get_label(row, col)] < min_pixel_nb) {
                labeled_picture.set_label(row, col, INITIAL_LABEL);
                labeled_picture.set_white(row, col);
            }
        }
    }
    // ---- Update labels to match vector indexes ----
    std::vector<int> remaining_labels;
    // Fill the vector with the remaining labels.
    for (int row = 0 ; row < labeled_picture.get_height() ; row++) {
        for (int col = 0 ; col < labeled_picture.get_width() ; col++) {
            if (!labeled_picture.has_label(row, col)) {
                continue;
            }
            // If this label has not been encountered yet.
            if (std::find(
                    remaining_labels.begin(),
                    remaining_labels.end(),
                    labeled_picture.get_label(row, col)
                ) == remaining_labels.end()) {
                remaining_labels.push_back(labeled_picture.get_label(row, col));
            }
        }
    }
    // Sort the vector.
    std::sort(remaining_labels.begin(), remaining_labels.end());
    // Relabel pixels with a label to match vector indexes.
    int new_label = -1;
    for (int row = 0 ; row < labeled_picture.get_height() ; row++) {
        for (int col = 0 ; col < labeled_picture.get_width() ; col++) {
            if (!labeled_picture.has_label(row, col)) {
                continue;
            }
            for (int new_label = 0 ; new_label < remaining_labels.size() ; new_label++) {
                if (remaining_labels[new_label] == labeled_picture.get_label(row, col)) {
                    labeled_picture.set_label(row, col, new_label);
                    break;
                }
            }
        }
    }
    // ---- Remove the smaller connected sets from the pixel_counts vector. ----
    for (int label = pixel_counts.size() - 1 ; label >= 0 ; label--) {
        if (pixel_counts[label] < min_pixel_nb) {
            pixel_counts.erase(pixel_counts.begin() + label);
        }
    }
    nb_labels = pixel_counts.size();
}

/*
 * Set each connected set upper-left and bottom-right corners coordinates.
 */
void set_connected_sets_corners(
    std::vector<ConnectedSet>& connected_sets,
    const Picture& labeled_picture
) {
    int current_label = -1;
    for (int row = 0 ; row < labeled_picture.get_height() ; row++) {
        for (int col = 0 ; col < labeled_picture.get_width() ; col++) {
            if (!labeled_picture.has_label(row, col)) {
                continue;
            }
            current_label = labeled_picture.get_label(row, col);
            if (row < connected_sets[current_label].get_upper_row()) {
                connected_sets[current_label].set_upper_row(row);
            }
            if (col < connected_sets[current_label].get_left_col()) {
                connected_sets[current_label].set_left_col(col);
            }
            if (row > connected_sets[current_label].get_bottom_row()) {
                connected_sets[current_label].set_bottom_row(row);
            }
            if (col > connected_sets[current_label].get_right_col()) {
                connected_sets[current_label].set_right_col(col);
            }
        }
    }
}

/*
 * Return a vector of labels ordered by smallest containing window.
 */
std::vector<int> get_labels_ordered_by_window(std::vector<ConnectedSet> connected_sets) {
    std::vector<int> labels;
    for (int label = 0 ; label < connected_sets.size() ; label++) {
        labels.push_back(label);
    }
    int first_label = -1;
    for (int i = 0 ; i < labels.size() ; i++) {
        int max = i; // Index of the label with maximum window size.
        for (int j = i + 1 ; j < labels.size() ; j++) {
            if (connected_sets[labels[j]].get_size() > connected_sets[labels[max]].get_size()) {
                max = j;
            }
        }
        // Swap labels.
        first_label = labels[i];
        labels[i] = labels[max];
        labels[max] = first_label;
    }
    return labels;
}

/*
 * Return a black and white image containing the connected set with given label.
 */
byte* to_black_and_white_image(
    const Picture& picture,
    std::vector<ConnectedSet> connected_sets,
    int label
) {
    ConnectedSet connected_set = connected_sets[label];
    // ---- Create RGB arrays depending on the pixel label ----
    byte* image = new byte[connected_set.get_size()];
    for (int row = 0 ; row < connected_set.get_height() ; row++) {
        for (int col = 0 ; col < connected_set.get_width() ; col++) {
            if (picture.get_label(
                    row + connected_set.get_upper_row(),
                    col + connected_set.get_left_col()
                ) != label) {
                image[col + connected_set.get_width() * row] = (byte)WHITE;
            }
            else {
                image[col + connected_set.get_width() * row] = (byte)BLACK;
            }
        }
    }
    return image;
}

int main() {
    // ---- Load input ----
    int range = 2; // Size of the neighbourhood of one pixel.
    int min_pixel_nb = 20; // Minimum number of pixels in a connected set.
    std::string input_file_name = "structure";
    std::string input_extension = ".png";
    std::string output_name = "./output/" + input_file_name + "_" + std::to_string(range);
    std::string output_extension = ".jpg";
    std::string input_image_path = stringSrcPath(input_file_name + input_extension);
    byte* input_image;
    int input_width, input_height;
    Imagine::loadGreyImage(
        input_image_path,
        input_image,
        input_width,
        input_height
    );

    // ---- Cast input to binary ----
    // Note : Imagine++ fills arrays row-wise (first row first, then second row, etc).
    byte* binary_image = to_binary(input_image, input_height, input_width);

    // ---- Display binary (uncomment if wanted) ----
    std::cout<<"Image width : "<<input_width<<std::endl;
    std::cout<<"Image height : "<<input_height<<std::endl;
    Imagine::Window window;
    /*window = Imagine::openWindow(input_width, input_height);
    Imagine::setActiveWindow(window);
    std::cout<<"Displaying binary image..."<<std::endl;
    Imagine::putGreyImage(Imagine::IntPoint2(0, 0), binary_image, input_width, input_height);
    Imagine::click();*/

    // ---- Create initial picture ----
    Picture picture(binary_image, input_height, input_width);
    int nb_labels = 0;

    // ---- Label connected sets and fill the labels vector ----
    for (int row = 0 ; row < picture.get_height() ; row++) {
        for (int col = 0 ; col < picture.get_width() ; col++) {
            if (picture.is_black(row, col) && !picture.has_label(row, col)) {
                picture.set_label(row, col, nb_labels);
                nb_labels++;
                picture.update_neighbours(row, col, range);
            }
        }
    }

    // ---- Compute number of pixels in each connected set ----
    std::vector<int> pixel_counts = count_pixels(picture, nb_labels);

    // ---- Remove smaller connected set ----
    remove_smaller_sets(picture, pixel_counts, nb_labels, min_pixel_nb);

    // ---- Create the vector of connected sets ----
    std::vector<ConnectedSet> connected_sets;
    for (int label = 0 ; label < nb_labels ; label++) {
        ConnectedSet connected_set(picture.get_width(), picture.get_height(), pixel_counts[label]);
        connected_sets.push_back(connected_set);
    }
    set_connected_sets_corners(connected_sets, picture);

    // ---- Order connected sets by smallest fully containing window ----
    std::vector<int> labels_by_window_order = get_labels_ordered_by_window(connected_sets);

    // ---- Create connected sets images in size order ----
    std::vector<byte*> connected_sets_images;
    for (int label = 0 ; label < labels_by_window_order.size() ; label++) {
        byte* connected_set_image = to_black_and_white_image(
            picture,
            connected_sets,
            labels_by_window_order[label]
        );
        connected_sets_images.push_back(connected_set_image);
    }

    // ---- Display smallest windows in size order ----
    /*for (int label = 0 ; label < labels_by_window_order.size() ; label++) {
        int set_width = connected_sets[labels_by_window_order[label]].get_width();
        int set_height = connected_sets[labels_by_window_order[label]].get_height();
        std::cout<<"Width : "<<set_width<<" Height : "<<set_height<<std::endl;
        window = Imagine::openWindow(
            set_width,
            set_height
        );
        Imagine::setActiveWindow(window);
        Imagine::putGreyImage(
            Imagine::IntPoint2(0, 0),
            connected_set_images[label],
            set_width,
            set_height
        );
    }*/

    // ---- Save connected sets images ----
    for (int label = 0 ; label < connected_sets_images.size() ; label++) {
        std::string set_name = output_name + "_(" + std::to_string(label) + ")" + output_extension;
        Imagine::saveGreyImage(
            stringSrcPath(set_name),
            connected_sets_images[label],
            connected_sets[labels_by_window_order[label]].get_width(),
            connected_sets[labels_by_window_order[label]].get_height()
        );
    }

    // ---- Create colored image from the connected sets ----
    std::srand((unsigned int)std::time(0)); // Initialize random seed.
    byte* color_image = to_color_image(picture, nb_labels);

    // ---- Display colored image (uncomment if wanted) ----
    std::cout<<"Number of labels : "<<nb_labels<<std::endl;
    /*std::cout<<"Displaying colored image..."<<std::endl;
    window = Imagine::openWindow(input_width, input_height);
    Imagine::setActiveWindow(window);
    Imagine::putColorImage(Imagine::IntPoint2(0, 0), color_image, input_width, input_height);
    Imagine::click();*/

    // ---- Save colored image ----
    std::string color_name = output_name + output_extension;
    Imagine::saveColorImage(stringSrcPath(color_name), color_image, input_width, input_height);

    // ---- Cleanup ----
    delete[] input_image;
    delete[] binary_image;
    delete[] color_image;

    return 0;
}

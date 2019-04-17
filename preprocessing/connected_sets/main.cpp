#include "pixel.h"
#include "picture.h"

#include <vector>

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
            if (max_value < image[row + height * col]) {
                max_value = image[row + height * col];
            }
        }
    }
    // ---- Fill the binary image ----
    byte* binary_image = new byte[height * width];
    for (int row = 0 ; row < height ; row++) {
        for (int col = 0 ; col < width ; col++) {
            binary_image[row + height * col] = (
                max_value > image[row + height * col] ?
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
                red[row + height * col] = (byte)WHITE;
                green[row + height * col] = (byte)WHITE;
                blue[row + height * col] = (byte)WHITE;
                continue;
            }
            red[row + height * col] = red_shades[picture.get_label(row, col)];
            green[row + height * col] = green_shades[picture.get_label(row, col)];
            blue[row + height * col] = blue_shades[picture.get_label(row, col)];
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
    // Unlabel and turn to white pixels from smaller connected sets.
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
    // Remove the smaller connected sets from the pixel_counts vector.
    for (int label = pixel_counts.size() - 1 ; label >= 0 ; label--) {
        if (pixel_counts[label] < min_pixel_nb) {
            pixel_counts.erase(pixel_counts.begin() + label);
        }
    }
    nb_labels = pixel_counts.size();
}

int main() {
    // ---- Load input ----
    int range = 2; // Size of the neighbourhood of one pixel.
    int min_pixel_nb = 10; // Minimum number of pixels in a connected set.
    std::string input_file_name = "structure";
    std::string input_extension = ".png";
    std::string output_name = "./output/" + input_file_name + "_" + std::to_string(range) + ".jpg";
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
    byte* binary_image = to_binary(input_image, input_height, input_width);

    // ---- Display binary (uncomment if wanted) ----
    std::cout<<"Image width : "<<input_width<<std::endl;
    std::cout<<"Image height : "<<input_height<<std::endl;
    Imagine::Window window = Imagine::openWindow(input_width, input_height);
    Imagine::setActiveWindow(window);
    /*std::cout<<"Displaying binary image..."<<std::endl;
    Imagine::putGreyImage(Imagine::IntPoint2(0, 0), binary_image, input_width, input_height);
    Imagine::click();*/

    // ---- Create initial picture and labels vector ----
    Picture picture(binary_image, input_width, input_height);
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

    // ---- Create colored image from the connected sets ----
    std::srand((unsigned int)std::time(0)); // Initialize random seed.
    byte* color_image = to_color_image(picture, nb_labels);

    // ---- Display colored image (uncomment if wanted) ----
    std::cout<<"Number of labels : "<<nb_labels<<std::endl;
    std::cout<<"Displaying colored image..."<<std::endl;
    Imagine::putColorImage(Imagine::IntPoint2(0, 0), color_image, input_width, input_height);
    Imagine::click();

    // ---- Save colored image ----
    Imagine::saveColorImage(stringSrcPath(output_name), color_image, input_width, input_height);

    // ---- Cleanup ----
    delete[] input_image;
    delete[] binary_image;
    delete[] color_image;

    return 0;
}

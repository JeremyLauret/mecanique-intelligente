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
byte* to_color_image(const Picture& picture, const std::vector<int>& labels) {
    int height = picture.get_height();
    int width = picture.get_width();
    // ---- Map labels to colors ----
    std::vector<byte> red_shades;
    std::vector<byte> green_shades;
    std::vector<byte> blue_shades;
    for (int label_count = 0; label_count < labels.size() ; label_count++) {
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

int main() {
    // ---- Load input ----
    std::string input_file_name = "hello_world";
    std::string input_extension = ".png";
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
    /*std::cout<<"Image width : "<<input_width<<std::endl;
    std::cout<<"Image height : "<<input_height<<std::endl;
    Imagine::Window window = Imagine::openWindow(input_width, input_height);
    Imagine::setActiveWindow(window);
    std::cout<<"Displaying binary image..."<<std::endl;
    Imagine::putGreyImage(Imagine::IntPoint2(0, 0), binary_image, input_width, input_height);
    Imagine::click();*/

    // ---- Create initial picture and labels vector ----
    Picture picture(binary_image, input_width, input_height);
    std::vector<int> labels;

    // ---- Label connected sets and fill the labels vector ----
    for (int row = 0 ; row < picture.get_height() ; row++) {
        for (int col = 0 ; col < picture.get_width() ; col++) {
            if (picture.is_black(row, col) && !picture.has_label(row, col)) {
                picture.set_label(row, col, labels.size());
                picture.update_neighbours(row, col);
                labels.push_back(labels.size());
            }
        }
    }

    // ---- Create colored image from the connected sets ----
    std::srand((unsigned int)std::time(0)); // Initialize random seed.
    byte* color_image = to_color_image(picture, labels);

    // ---- Display colored image (uncomment if wanted) ----
    /*std::cout<<"Number of labels : "<<labels.size()<<std::endl;
    std::cout<<"Displaying colored image..."<<std::endl;
    Imagine::putColorImage(Imagine::IntPoint2(0, 0), color_image, input_width, input_height);
    Imagine::click();*/

    // ---- Save colored image ----
    std::string output_name = "./output/" + input_file_name + ".jpg";
    Imagine::saveColorImage(stringSrcPath(output_name), color_image, input_width, input_height);

    // ---- Cleanup ----
    delete[] input_image;
    delete[] binary_image;
    delete[] color_image;

    return 0;
}

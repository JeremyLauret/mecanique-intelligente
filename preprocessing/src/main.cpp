#include <vector>
#include <algorithm>
#include <dirent.h> // Provide directory manipulation.
#include <cstdio>   // Provide perror() and remove().

#include "pretraitement.h"
#include "pixel.h"
#include "picture.h"
#include "connected_set.h"


bool PROCESS_RAW_INPUT = true;
int RANGE = 15;          // Range of pixels considered to be in the neighborhood of another pixel.
int MIN_PIXELS_NB = 10; // Minimum number of pixels in a connected set.
std::string RAW_INPUT_PATH = "../raw_input/";
std::string INPUT_PATH = "../input/";
std::string LARGEST_SETS_PATH = "../largest_sets/";
std::string HOUGH_PATH = "../hough_output/";
std::string OUTPUT_PATH = "../output/";
std::string OUTPUT_EXTENSION = ".jpg";


/*
 * Construct a binary image from the input.
 *
 * The maximum value in the input is interpreted as white. Everything else is cast to black.
 */
byte* to_binary(byte* image, int height, int width) {
    // ---- Find the maximal value in the input ----
    byte max_value = (byte)0;
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
                max_value / 2 > image[col + width * row] ?
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
 *
 * If height and width are sufficiently large, the returned image will be set with these dimensions.
 */
byte* to_black_and_white_image(
    const Picture& picture,
    std::vector<ConnectedSet> connected_sets,
    int label,
    int height = -1,
    int width = -1
) {
    ConnectedSet connected_set = connected_sets[label];
    // If height and width are sufficiently large.
    if (height >= connected_set.get_height() + connected_set.get_upper_row() &&
        width > connected_set.get_width() + connected_set.get_left_col()) {
        byte* image = new byte[height * width];
        for (int row = 0 ; row < height ; row++) {
            for (int col = 0 ; col < width ; col++) {
                image[col + width * row] = (byte)WHITE;
            }
        }
        for (int row = connected_set.get_upper_row() ;
             row < connected_set.get_upper_row() + connected_set.get_height() ;
             row++) {
            for (int col = connected_set.get_left_col() ;
                 col < connected_set.get_left_col() + connected_set.get_width() ;
                 col++) {
                if (picture.get_label(row, col) == label) {
                    image[col + width * row] = (byte)BLACK;
                }
            }
        }
        return image;
    }
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


/*
 * Core function of the program. Process a file belonging to the directory_path directory and save
 * its connected sets in the OUTPUT_PATH directory.
 *
 * If isolate_largest_set is true, the largest connected set will be saved in LARGEST_SETS_PATH
 * with same size as the original image.
 *
 * The parameters range and min_pixesl_nb alter the behavior of the separation algorithm. Range is
 * the size of each pixel neighborhood, and min_pixel_nb is the minimal number of pixels in each
 * connected set.
 */
void compute_and_save_connected_sets(
    std::string input_path,
    std::string input_full_name,
    bool isolate_largest_set,
    int range=1,
    int min_pixels_nb=0
) {
    // ---- Load input ----
    // input_name refers to the name of the input file without its file extension.
    std::string input_name = input_full_name.substr(
        0,
        input_full_name.length() - input_full_name.substr(input_full_name.find('.')).length()
    );
    std::string input_image_path = stringSrcPath(input_path + input_full_name);
    byte* input_image;
    int input_width, input_height;
    Imagine::loadGreyImage(
        input_image_path,
        input_image,
        input_width,
        input_height
    );

    // ---- Cast input to binary ----
    // Note : Imagine++ fills arrays row-wise (first row, then second row, etc).
    byte* binary_image = to_binary(input_image, input_height, input_width);

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
    remove_smaller_sets(picture, pixel_counts, nb_labels, min_pixels_nb);

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
    // Largest window.
    byte* connected_set_image = to_black_and_white_image(
        picture,
        connected_sets,
        labels_by_window_order[0],
        isolate_largest_set ? input_height : -1,
        isolate_largest_set ? input_width : -1
    );
    connected_sets_images.push_back(connected_set_image);
    // Other windows.
    for (int label = 1 ; label < labels_by_window_order.size() ; label++) {
        byte* connected_set_image = to_black_and_white_image(
            picture,
            connected_sets,
            labels_by_window_order[label]
        );
        connected_sets_images.push_back(connected_set_image);
    }

    // ---- Save connected sets images ----
    // Largest window.
    int set_upper_row = connected_sets[labels_by_window_order[0]].get_upper_row();
    int set_left_col = connected_sets[labels_by_window_order[0]].get_left_col();
    std::string set_name = (
        isolate_largest_set ? LARGEST_SETS_PATH + input_name + OUTPUT_EXTENSION :
        OUTPUT_PATH + input_name + "_" + std::to_string(set_upper_row) + "_" +
        std::to_string(set_left_col) + OUTPUT_EXTENSION
    );
    Imagine::saveGreyImage(
        stringSrcPath(set_name),
        connected_sets_images[0],
        isolate_largest_set ? input_width : connected_sets[labels_by_window_order[0]].get_width(),
        isolate_largest_set ? input_height : connected_sets[labels_by_window_order[0]].get_height()
    );
    // Other windows.
    for (int label = 1 ; label < connected_sets_images.size() ; label++) {
        set_upper_row = connected_sets[labels_by_window_order[label]].get_upper_row();
        set_left_col = connected_sets[labels_by_window_order[label]].get_left_col();
        set_name = (
            OUTPUT_PATH + input_name + "_" + std::to_string(set_upper_row) + "_" +
            std::to_string(set_left_col) + OUTPUT_EXTENSION
        );
        Imagine::saveGreyImage(
            stringSrcPath(set_name),
            connected_sets_images[label],
            connected_sets[labels_by_window_order[label]].get_width(),
            connected_sets[labels_by_window_order[label]].get_height()
        );
    }

    // ---- Cleanup ----
    delete[] input_image;
    delete[] binary_image;
    for (int label = 0 ; label < connected_sets_images.size() ; label++) {
        delete[] connected_sets_images[label];
    }
}


void removeVariableIllumination(Img img, const Img& img_or) {
    // j inverse l image pour travailler avec le modele additif
    Img fond_blanc = img.clone();
    fond_blanc.fill(255);
    img = fond_blanc - img;
    for(int i = 0 ; i < nb_iter ; i++){
        for(int x = 0 ; x< img.width()  ; x++){
            for(int y = 0 ; y < img.height() ; y++){
                pix_trans(img, x, y);
            }
        }
    }
    int p ;
    for(int x = 0 ; x< img.width()  ; x++){
        for(int y = 0 ; y < img.height() ; y++){
             p = img_or(x,y) + img(x,y) ;
             if(p>255){
                 p=255;
             }
             img(x,y) = p ;
        }
    }
}


void projection(Img img) {
    int hist[256], hist_derv[256] ;
    histogram(hist, img);
    double m = 0. ;
    for(int i = 0 ; i<256 ; i++){
        m+=hist[i]*i/(img.height()*img.width());
    }
    projection_seuil(img,int(m)-5);
}


/**
 * @brief getPreprocessedFullImage : retire l'illumination variable et effectue une projection sur
 * le noir et le blanc de l'image, puis ne garde que des traits d'un pixel de large.
 * @param img : image que l'on traite, qui est la sortie intéressante de cette fonction.
 * @param img_or : image d'origine, extraite du fichier input.
 */
void getPreprocessedFullImage(Img img, const Img& img_or) {
    // Retire l'illumination variable
    removeVariableIllumination(img, img_or);
    // Retire les pixels isolés de l'image
    supprimePixelsIsoles(img);
    // Effectue une projection sur le noir et le blanc de tous les pixels de l'image
    projection(img);
    // Réduit toutes les droites à une seule ligne d'un pixel de large
    supprimeDroites(img);
    retireDroites(img);
}


int main() {
    // ---- Apply first cleanup on the raw input ----
    if (PROCESS_RAW_INPUT) {
        // c_str() casts a string to const char*.
        DIR* raw_input_directory =  opendir(stringSrcPath(RAW_INPUT_PATH).c_str());
        if (raw_input_directory == NULL) { // An error has occured.
            perror("");
        }
        struct dirent* raw_input_file;
        while ((raw_input_file = readdir(raw_input_directory)) != NULL) {
            if (!strcmp(raw_input_file->d_name, ".") || !strcmp(raw_input_file->d_name, "..")) {
                continue;
            }
            Img raw_input_image, cleaned_image;
            if (!Imagine::load(
                raw_input_image,
                stringSrcPath(RAW_INPUT_PATH + std::string(raw_input_file->d_name)))
            ){
                perror("");
            }
            //Imagine::Window raw_window = Imagine::openWindow(
            //    raw_input_image.width(),
            //    raw_input_image.height()
            //);
            cleaned_image = raw_input_image.clone();
            getPreprocessedFullImage(cleaned_image, raw_input_image);
            std::string cleaned_name = INPUT_PATH + std::string(raw_input_file->d_name);
            Imagine::save(cleaned_image, stringSrcPath(cleaned_name));
        }
        if (closedir(raw_input_directory) == -1) { // Close directory.
            perror("");
        }
    }

    // ---- Apply connexity to the clean input (largest set saved in LARGEST_SETS_PATH) ----
    DIR* input_directory =  opendir(stringSrcPath(INPUT_PATH).c_str());
    if (input_directory == NULL) { // An error has occured.
        perror("");
    }
    struct dirent* input_file;
    while ((input_file = readdir(input_directory)) != NULL) {
        if (!strcmp(input_file->d_name, ".") || !strcmp(input_file->d_name, "..")) {
            continue;
        }
        std::string input_full_name = std::string(input_file->d_name);
        compute_and_save_connected_sets(INPUT_PATH, input_full_name, true, RANGE, MIN_PIXELS_NB);
    }

    // ---- Apply beam removal on largest sets ----
    DIR* largest_sets_directory =  opendir(stringSrcPath(LARGEST_SETS_PATH).c_str());
    if (largest_sets_directory == NULL) { // An error has occured.
        perror("");
    }
    struct dirent* largest_set_file;
    while ((largest_set_file = readdir(largest_sets_directory)) != NULL) {
        if (!strcmp(largest_set_file->d_name, ".") || !strcmp(largest_set_file->d_name, "..")) {
            continue;
        }
        //Imagine::milliSleep(1500);
        Img largest_set_image;
        if (!Imagine::load(
            largest_set_image,
            stringSrcPath(LARGEST_SETS_PATH + std::string(largest_set_file->d_name)))
        ){
            perror("");
        }
        // Transformée de Hough
        //Imagine::Window hough_window = Imagine::openWindow(
        //    largest_set_image.width(),
        //    largest_set_image.height()
        //);
        //Imagine::setActiveWindow(hough_window);
        std::vector<Img> hough_output = hough(largest_set_image);
        std::string no_beam_name = HOUGH_PATH + std::string(largest_set_file->d_name);
        std::string beam_name = HOUGH_PATH + "beam_" + std::string(largest_set_file->d_name);
        Imagine::save(hough_output[0], stringSrcPath(no_beam_name));
        Imagine::save(hough_output[1], stringSrcPath(beam_name));
    }

    // ---- Apply connexity to the new input ----
    DIR* hough_directory =  opendir(stringSrcPath(HOUGH_PATH).c_str());
    if (hough_directory == NULL) { // An error has occured.
        perror("");
    }
    struct dirent* hough_file;
    while ((hough_file = readdir(hough_directory)) != NULL) {
        if (!strcmp(hough_file->d_name, ".") || !strcmp(hough_file->d_name, "..")) {
            continue;
        }
        std::string hough_full_name = std::string(hough_file->d_name);
        compute_and_save_connected_sets(HOUGH_PATH, hough_full_name, false, RANGE, MIN_PIXELS_NB);
    }

    // ---- Remove intermediary files ----
    // Largest sets directory.
    rewinddir(largest_sets_directory);
    while ((largest_set_file = readdir(largest_sets_directory)) != NULL) {
        if (!strcmp(largest_set_file->d_name, ".") || !strcmp(largest_set_file->d_name, "..")) {
            continue;
        }
        if (remove(stringSrcPath(LARGEST_SETS_PATH + std::string(largest_set_file->d_name)).c_str()) != 0) {
            perror("");
        }
    }
    // Hough output directory.
    rewinddir(hough_directory);
    while ((hough_file = readdir(hough_directory)) != NULL) {
        if (!strcmp(hough_file->d_name, ".") || !strcmp(hough_file->d_name, "..")) {
            continue;
        }
        if (remove(stringSrcPath(HOUGH_PATH + std::string(hough_file->d_name)).c_str()) != 0) {
            perror("");
        }
    }

    // ---- Close opened directories ----
    if (closedir(input_directory) == -1) {
        perror("");
    }
    if (closedir(largest_sets_directory) == -1) {
        perror("");
    }
    if (closedir(hough_directory) == -1) {
        perror("");
    }

    return 0;
}

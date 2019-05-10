#include "hough.h"


/**
 * Remplit l'histogramme H des couples (rho, theta) obtenus par transformée de Hough des pixels de
 * l'image.
 *
 * H est de taille nb_theta x (2*rho_max + 1) (rho pouvant prendre des valeurs négatives).
 */
void computeHoughHistogram(const Img& img, int H[], int rho_max, int nb_theta) {
    int rho{ 0 };
    for (int x = 0 ; x < img.height() ; x++){
        for (int y = 0 ; y < img.width() ; y++){
            if (img(y, x) == black){
                for (int theta = 0 ; theta < nb_theta ; theta++) {
                    rho = (int)(
                        x * cos(theta * M_PI / nb_theta) + y * sin(theta * M_PI / nb_theta)
                    );
                    H[(rho + rho_max) * nb_theta + theta] += 1;
                }
            }
        }
    }
}


/*
 * Colorie les pixels situés sur le segment [(x_deb, y_deb), (x_fin, y_fin)].
 */
void colorSegment(Img& img, int x_deb, int x_fin, int y_deb, int y_fin, int color) {
    int x_min{ (x_deb < x_fin ? x_deb : x_fin) };
    int x_max{ (x_deb > x_fin ? x_deb : x_fin) };
    int y_min{ (y_deb < y_fin ? y_deb : y_fin) };
    int y_max{ (y_deb > y_fin ? y_deb : y_fin) };
    if (x_deb == x_fin) {
        if (x_deb < 0 || x_deb >= img.height()) {
            return;
        }
        for (int y = (y_min >= 0 ? y_min : 0) ;
             y <= (y_max < img.width() ? y_max : img.width() - 1) ;
             y++) {
            img(y, x_deb) = color;
        }
    }
    else {
        for (int x = (x_min >= 0 ? x_min : 0) ;
             x <= (x_max < img.height() ? x_max : img.height() - 1) ;
             x++) {
            int y{ (int)(y_deb + (x - x_deb) * (y_fin - y_deb) / (x_fin - x_deb)) };
            if (y >= 0 && y < img.width()) {
                img(y, x) = color;
            }
        }
    }
}


/*
 * Détermine la position et la longueur de la plus longue succession de pixels noirs située sur la
 * droite paramétrée par rho et theta.
 *
 * La longueur et les coordonnées du segment sont affectés aux paramètres.
 */
void findSegmentsEdges(const Img& img, int rho, int theta, int& x_debut, int& y_debut,
                    int& x_fin, int& y_fin, int pixel_range, int nb_theta) {
    // Attention, x pour la hauteur et y pour la largeur.
    int x_current{ 0 };
    int y_current{ 0 };
    int longueurActuelle{ 0 };
    int longueurPoutreMax{ 0 };
    bool black_pixel_ahead{ false };

    // Horizontal line.
    if (theta == 0) {
        assert (rho >= 0 && rho < img.height());
        x_debut = rho;
        x_fin = rho;
        for (int y = 0 ; y < img.width() ; y++) {
            black_pixel_ahead = false;
            if (longueurActuelle == 0 && y != img.width() - 1) {
                if (img(y, rho) == black) {
                    y_current = y;
                    longueurActuelle = 1;
                }
                continue;
            }
            // Check if there is a black pixel not too far away.
            for (int y2 = y ;
                 y2 < (y + pixel_range + 1 < img.width() ? y + pixel_range + 1 : img.width()) ;
                 y2++) {
                if (img(y2, rho) == black) {
                    black_pixel_ahead = true;
                    longueurActuelle += 1;
                    break;
                }
            }
            if (!black_pixel_ahead || y == img.width() - 1) {
                if (longueurActuelle > longueurPoutreMax) {
                    y_debut = y_current;
                    y_fin = black_pixel_ahead ? y : y - 1;
                    longueurPoutreMax = longueurActuelle;
                }
                longueurActuelle = 0;
            }
        }
        return;
    }
    // Vertical line.
    if (theta == 90) {
        assert (rho >= 0 && rho < img.width());
        y_debut = rho;
        y_fin = rho;
        for (int x = 0 ; x < img.height() ; x++) {
            black_pixel_ahead = false;
            if (longueurActuelle == 0 && x != img.height() - 1) {
                if (img(rho, x) == black) {
                    x_current = x;
                    longueurActuelle = 1;
                }
                continue;
            }
            // Check if there is a black pixel not too far away.
            for (int x2 = x ;
                 x2 < (x + pixel_range + 1 < img.height() ? x + pixel_range + 1 : img.height()) ;
                 x2++) {
                if (img(rho, x2) == black) {
                    black_pixel_ahead = true;
                    longueurActuelle += 1;
                    break;
                }
            }
            if (!black_pixel_ahead || x == img.height() - 1) {
                if (longueurActuelle > longueurPoutreMax) {
                    x_debut = x_current;
                    x_fin = black_pixel_ahead ? x : x - 1;
                    longueurPoutreMax = longueurActuelle;
                }
                longueurActuelle = 0;
            }
        }
        return;
    }
    if (theta > 45 && theta < 135) { // x varies more than y for these angles.
        // Should be set to true as soon as valid x and y have been visited.
        bool stepped_in_frame = false;
        for (int x = 0 ; x < img.height() ; x++) {
            int y { (int)(
                (rho - x * cos(theta * M_PI / nb_theta)) / sin(theta * M_PI / nb_theta)
            ) };
            if (!stepped_in_frame && y >= 0 && y < img.width()) {
                stepped_in_frame = true;
            }
            if (y < 0 || y >= img.width()) {
                if (!stepped_in_frame) {
                    continue;
                }
                else {
                    if (longueurActuelle > longueurPoutreMax) {
                        x_debut = x_current;
                        x_fin = x - 1;
                        y_debut = y_current;
                        y_fin = (int)(
                            (rho - (x - 1) * cos(theta * M_PI / nb_theta)) / sin(theta * M_PI / nb_theta)
                        );
                    }
                    break;
                }
            }
            black_pixel_ahead = false;
            if (longueurActuelle == 0 && x != img.height() - 1) {
                if (img(y, x) == black) {
                    x_current = x;
                    y_current = y;
                    longueurActuelle = 1;
                }
                continue;
            }
            // Check if there is a black pixel not too far away.
            for (int x2 = x ;
                 x2 < (x + pixel_range + 1 < img.height() ? x + pixel_range + 1 : img.height()) ;
                 x2++) {
                int y2 { (int)(
                    (rho - x2 * cos(theta * M_PI / nb_theta)) / sin(theta * M_PI / nb_theta)
                ) };
                if (y2 < 0 || y2 >= img.width()) {
                    break;
                }
                if (img(y2, x2) == black) {
                    black_pixel_ahead = true;
                    longueurActuelle += 1;
                    break;
                }
            }
            if (!black_pixel_ahead || x == img.height() - 1) {
                if (longueurActuelle > longueurPoutreMax) {
                    x_debut = x_current;
                    x_fin = black_pixel_ahead ? x : x - 1;
                    y_debut = y_current;
                    y_fin = black_pixel_ahead ? y : y - 1;
                    longueurPoutreMax = longueurActuelle;
                }
                longueurActuelle = 0;
            }
        }
        return;
    }
    // Should be set to true as soon as valid x and y have been visited.
    bool stepped_in_frame = false;
    // 0 < theta <= 45 or 135 <= theta < 180 : y varies more than x.
    for (int y = 0 ; y < img.width() ; y++) {
        int x { (int)(
            (rho - y * sin(theta * M_PI / nb_theta)) / cos(theta * M_PI / nb_theta)
        ) };
        if (!stepped_in_frame && x >= 0 && x < img.height()) {
            stepped_in_frame = true;
        }
        if (x < 0 || x >= img.height()) {
            if (!stepped_in_frame) {
                continue;
            }
            else {
                if (longueurActuelle > longueurPoutreMax) {
                    x_debut = x_current;
                    x_fin = (int)(
                        (rho - (y - 1) * sin(theta * M_PI / nb_theta)) / cos(theta * M_PI / nb_theta)
                    );
                    y_debut = y_current;
                    y_fin = y - 1;
                }
                break;
            }
        }
        black_pixel_ahead = false;
        if (longueurActuelle == 0 && y != img.width() - 1) {
            if (img(y, x) == black) {
                x_current = x;
                y_current = y;
                longueurActuelle = 1;
            }
            continue;
        }
        // Check if there is a black pixel not too far away.
        for (int y2 = y ;
             y2 < (y + pixel_range + 1 < img.width() ? y + pixel_range + 1 : img.width()) ;
             y2++) {
            int x2 { (int)(
                (rho - y2 * sin(theta * M_PI / nb_theta)) / cos(theta * M_PI / nb_theta)
            ) };
            if (x2 < 0 || x2 >= img.height()) {
                break;
            }
            if (img(y2, x2) == black) {
                black_pixel_ahead = true;
                longueurActuelle += 1;
                break;
            }
        }
        if (!black_pixel_ahead || y == img.width() - 1) {
            if (longueurActuelle > longueurPoutreMax) {
                x_debut = x_current;
                x_fin = black_pixel_ahead ? x : x - 1;
                y_debut = y_current;
                y_fin = black_pixel_ahead ? y : y - 1;
                longueurPoutreMax = longueurActuelle;
            }
            longueurActuelle = 0;
        }
    }
}


/**
 * Identifie les lignes de l'image les plus longues et stocke leurs coordonnées de début et de fin
 * dans les vecteurs en paramètre.
 *
 * min_relative_size fixe la taille minimale relative à la ligne avec le plus de pixels pour qu'une
 * autre ligne soit retenue.
 */
void findLongestLines(Img img, int H[], int rho_max, std::vector<int> &beg_rows,
                      std::vector<int> &end_rows, std::vector<int> &beg_cols,
                      std::vector<int> &end_cols, int min_relative_size, int pixel_range,
                      int nb_theta) {
    // Identification de la ligne avec le plus de pixels.
    int most_pixels_rho{ -rho_max };
    int most_pixels_theta{ 0 };
    for (int rho = -rho_max ; rho < rho_max + 1 ; rho++){
        for (int theta = 0 ; theta < nb_theta ; theta++){
            if (H[(rho + rho_max) * nb_theta + theta] >
                H[(most_pixels_rho + rho_max) * nb_theta + most_pixels_theta]) {
                most_pixels_rho = rho;
                most_pixels_theta = theta;
            }
        }
    }
    int most_pixels{ H[(most_pixels_rho + rho_max) * nb_theta + most_pixels_theta] };
    // Recherche des extrémités des lignes avec suffisamment de pixels.
    for (int rho = -rho_max ; rho < rho_max + 1 ; rho++){
        for (int theta = 0 ; theta < nb_theta ; theta++){
            if (H[(rho + rho_max) * nb_theta + theta] >= (int)(most_pixels * min_relative_size / 100)){
                int beg_row{ 0 };
                int beg_col{ 0 };
                int end_row{ 0 };
                int end_col{ 0 };
                findSegmentsEdges(img, rho, theta, beg_row, beg_col, end_row, end_col, pixel_range,
                               nb_theta);
                beg_rows.push_back(beg_row);
                beg_cols.push_back(beg_col);
                end_rows.push_back(end_row);
                end_cols.push_back(end_col);
            }
        }
    }
}

/**
 * Applique l'algorithme de détection de droites de Hough à l'image fournie pour en retirer les
 * droites les plus longues.
 *
 * Renvoie un vecteur de taille 2 contenant d'une part l'image d'origine privée des plus longues
 * droites, d'autre part l'image contenant uniquement les droites retirées.
 *
 * Toute droite contenant plus de min_relative_size * <nombre de pixels de la plus longue droite>
 * pixels est retirée.
 */
std::vector<Img> hough(const Img& img, int min_relative_size, int pixel_range,
                       int beam_thickness, int nb_theta){
   int rho_max = (int)(sqrt(img.width() * img.width() + img.height() * img.height()));
   int nb_rho = 2 * rho_max + 1;

   // Histogramme des transformées de Hough.
   // Ligne horizontale sur le bord inférieur de l'image => (rho = <hauteur>, theta = 0).
   // Ligne verticale sur le bord droit de l'image => (rho = <largeur>, theta = 90).
   int H[nb_theta * nb_rho];
   for (int i = 0 ; i < nb_theta * nb_rho ; i++){
       H[i] = 0;
   }
   computeHoughHistogram(img, H, rho_max, nb_theta);

   // Coordonnées des points de début et de fin des lignes de l'image avec le plus de pixels.
   std::vector<int> beg_rows;
   std::vector<int> end_rows;
   std::vector<int> beg_cols;
   std::vector<int> end_cols;

   // On recherche les plus longues lignes de l'image.
   findLongestLines(img, H, rho_max, beg_rows, end_rows, beg_cols, end_cols, min_relative_size,
                    pixel_range, nb_theta);

   // On retire de l'image les lignes repérées.
   Img no_beam_img = img.clone(); // Original image after beam deletion.
   for (int i = 0 ; i < beg_rows.size() ; i++) {
       for (int d_row = -beam_thickness ; d_row < beam_thickness + 1 ; d_row++) {
           for (int d_col = -beam_thickness ; d_col < beam_thickness + 1 ; d_col++) {
               colorSegment(
                   no_beam_img,
                   beg_rows[i] + d_row,
                   end_rows[i] + d_row,
                   beg_cols[i] + d_col,
                   end_cols[i] + d_col,
                   white
               );
           }
       }
   }
   Img beam_img = img + Imagine::WHITE - no_beam_img;

   // Output de la fonction
   std::vector<Img> outputImages;
   outputImages.push_back(no_beam_img);
   outputImages.push_back(beam_img);

   return outputImages;
}

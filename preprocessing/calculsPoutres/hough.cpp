#include "hough.h"


/**
 * @brief calculTransformeeSurImage : Calcul le tableau M des rho et theta correspondant aux droites de l'image img
 * @param img : image d'origine
 * @param M : tableau de valeurs des rho et theta, de taille nb_theta*nb_rho
 * @param NB_theta
 * @param rho_max
 */
void calculTransformeeSurImage(const Img& img, int M[], const int& rho_max) {
    for (int x=0; x<img.height(); x++){
        for (int y=0; y<img.width(); y++){
            if (img(y,x) == black){
                for (int theta=0; theta<nb_theta; theta++){
                    float rho = x*cos(theta * M_PI / nb_theta) + y*sin(theta * M_PI / nb_theta);
                    int rho_int = static_cast<int>(rho);
                    M[(rho_int+rho_max)*nb_theta + theta] = M[(rho_int+rho_max)*nb_theta + theta] + 1;
                }
            }
        }
    }
}



/**
 * @brief rechercheMaxRhoTheta : recherche les rho et theta correspondant à des droites très visibles et assez longues dans l'image
 * @param img
 * @param M : tableau des rho et theta
 * @param rho_max
 * @param imgDroitesLarges
 * @param imgDroites
 */
void rechercheMaxRhoTheta(const Img& img, int M[], const int& rho_max, Img imgDroitesLarges, Img imgDroites) {
    // On recherche les maxima de M et on les affiche, si ils dépassent un certain seuil
    for (int rho=-rho_max; rho<rho_max+1; rho++){
        for (int theta=0; theta<nb_theta; theta++){

            int nb_repetition_ThetaRho = M[(rho+rho_max)*nb_theta + theta];
            if (nb_repetition_ThetaRho > seuil_hough){
                cout << "rho = " << rho << ", theta = " << theta << ", repet = " << nb_repetition_ThetaRho << endl;  // Affichage pouvant être supprimé
                int xR, yR, xRf, yRf;
                int seuilDetectionDroite = 2;
                debutFinDroite(img, rho, theta, xR, yR, xRf, yRf, 0, seuilDetectionDroite);
                retireSegment(img, xR, yR, xRf, yRf);
                drawRect(xR, yR, abs(xRf-xR), abs(yRf-yR), RED);

                // On supprime les droites avec des rho ou theta très proches, i.e. plus proche que "ecart"
                for (int dtheta=-ecart_hough; dtheta<ecart_hough+1; dtheta++){
                    for (int drho=-ecart_hough; drho<ecart_hough+1; drho++) {
                        if (dtheta != 0 && drho != 0) {
                            M[(rho+rho_max + drho)*nb_theta + theta + dtheta] = 0;
                        }
                    }
                }


                dessineDroite(imgDroites, rho, theta, img);


                for (int drho = -tolerance_hough; drho<tolerance_hough+1; drho++) {
                    dessineDroite(imgDroitesLarges, rho+drho, theta, img);
                }
            }
        }
    }
}




void debutFinDroite(const Img& img, const int& rho, const int& theta, int x_debut, int y_debut, int x_fin, int y_fin, int longueurPoutreMax, const int& seuil) {
    // Attention, x pour la hauteur et y pour la largeur
    x_debut = 0,
        y_debut = 0,
        x_fin = 0,
        y_fin = 0;  // Coordonnées de début et fin de la poutre
    longueurPoutreMax = 0;  // Longueur de la plus longue poutre détectée
    int x_current = 0,
        y_current = 0,
        x_end_current = 0,
        y_end_current = 0;  // Coordonnées de la poutre sur laquelle on travaille au fur et à mesure du calcul
    int longueurActuelle = 0;   // On calcule la longueur de la poutre le long de l'image


    if (theta == 0 || theta == 90) {
        if (theta == 0 && rho>= 0 && rho<img.width()) {
            for (int y=0; y<img.width(); y++) {
                // On touche un pixel noir
                if (img(y, rho) == black) {

                    // Initialisation : on arrive sur une potentielle poutre donc on stocke les coordonnées du début de cette poutre
                    if (longueurActuelle == 0){
                        x_current = rho;
                        y_current = y;
                    }

                    longueurActuelle += 1;
                }

                // Fin : on touche un pixel blanc donc on est hors de la poutre
                else {
                    if (longueurActuelle > longueurPoutreMax) { // Si on a détecté une poutre plus longue qu'avant, on la stocke (elle remplace la précédente)
                        x_debut = x_current;
                        y_debut = y_current;
                        x_fin = rho;
                        y_fin = y-1;
                        longueurPoutreMax = longueurActuelle;
                        y_current = y;
                        longueurActuelle = 0;
                    }
                }
            }
        }
        else {  // theta = pi/2
            if (rho>=0 && rho<img.height()) {
                for (int x=0; x<img.height(); x++) {

                    // On touche un pixel noir
                    if (img(rho, x) == black) {

                        // Initialisation : on arrive sur une potentielle poutre donc on stocke les coordonnées du début de cette poutre
                        if (longueurActuelle == 0){
                            x_current = x;
                            y_current = rho;
                        }

                        longueurActuelle += 1;
                    }

                    // Fin : on touche un pixel blanc donc on est hors de la poutre
                    else {
                        if (longueurActuelle > longueurPoutreMax) { // Si on a détecté une poutre plus longue qu'avant, on la stocke (elle remplace la précédente)
                            x_debut = x_current;
                            y_debut = y_current;
                            x_fin = x-1;
                            y_fin = rho;
                            longueurPoutreMax = longueurActuelle;
                            x_current = x;
                            longueurActuelle = 0;
                        }
                    }
                }
            }
        }
    }

    else {
        if (theta > 45 && theta < 135)  {
            for (int x=0; x<img.height(); x++) {
                float numerateur0 = rho - x*cos(theta * M_PI/nb_theta);
                float numerateur1 = rho +1 - x*cos(theta * M_PI/nb_theta);
                float numerateur2 = rho +2 - x*cos(theta * M_PI/nb_theta);
                float numerateur3 = rho -1 - x*cos(theta * M_PI/nb_theta);
                float numerateur4 = rho -2 - x*cos(theta * M_PI/nb_theta);
                float float_y0 = numerateur0 / sin(theta * M_PI/nb_theta);
                int y0 = static_cast<int>(float_y0);
                float float_y1 = numerateur1 / sin(theta * M_PI/nb_theta);
                int y1 = static_cast<int>(float_y1);
                float float_y2 = numerateur2 / sin(theta * M_PI/nb_theta);
                int y2 = static_cast<int>(float_y2);
                float float_y3 = numerateur3 / sin(theta * M_PI/nb_theta);
                int y3 = static_cast<int>(float_y3);
                float float_y4 = numerateur4 / sin(theta * M_PI/nb_theta);
                int y4 = static_cast<int>(float_y4);

                // On touche un pixel noir
                if ((y0>=0 && y0<img.width() && img(y0,x)==black) ||
                    (y1>=0 && y1<img.width() && img(y1,x)==black) ||
                    (y2>=0 && y2<img.width() && img(y2,x)==black) ||
                    (y3>=0 && y3<img.width() && img(y3,x)==black) ||
                    (y4>=0 && y4<img.width() && img(y4,x)==black)) {

                    // Initialisation : on arrive sur une potentielle poutre donc on stocke les coordonnées du début de cette poutre
                    if (longueurActuelle == 0){
                        x_current = x;
                        y_current = y0;
                    }

                    longueurActuelle += 1;
                    x_end_current = x;
                    y_end_current = y0;
                }

                // Fin : on touche un pixel blanc donc on est hors de la poutre
                else {
                    if (longueurActuelle > longueurPoutreMax) { // Si on a détecté une poutre plus longue qu'avant, on la stocke (elle remplace la précédente)
                        x_debut = x_current;
                        y_debut = y_current;
                        x_fin = x_end_current;
                        y_fin = y_end_current;
                        longueurPoutreMax = longueurActuelle;
                        x_current = x;
                        y_current = y0;
                        longueurActuelle = 0;
                    }
                }
            }
        }
        else { // Cas theta <= 45 ou theta >= 135
            for (int y=0; y<img.width(); y++) {
                float numerateur0 = rho - y*sin(theta * M_PI/nb_theta);
                float float_x0 = numerateur0 / cos(theta * M_PI/nb_theta);
                int x0 = static_cast<int>(float_x0);
                float numerateur1 = rho - y*sin(theta * M_PI/nb_theta);
                float float_x1 = numerateur1 / cos(theta * M_PI/nb_theta);
                int x1 = static_cast<int>(float_x1);
                float numerateur2 = rho - y*sin(theta * M_PI/nb_theta);
                float float_x2 = numerateur2 / cos(theta * M_PI/nb_theta);
                int x2 = static_cast<int>(float_x2);
                float numerateur3 = rho - y*sin(theta * M_PI/nb_theta);
                float float_x3 = numerateur3 / cos(theta * M_PI/nb_theta);
                int x3 = static_cast<int>(float_x3);
                float numerateur4 = rho - y*sin(theta * M_PI/nb_theta);
                float float_x4 = numerateur4 / cos(theta * M_PI/nb_theta);
                int x4 = static_cast<int>(float_x4);

                // On touche un pixel noir
                if ((x0>=0 && x0<img.height()  && img(y,x0)==black) ||
                    (x1>=0 && x1<img.height()  && img(y,x1)==black) ||
                    (x2>=0 && x2<img.height()  && img(y,x2)==black) ||
                    (x3>=0 && x3<img.height()  && img(y,x3)==black) ||
                    (x4>=0 && x4<img.height()  && img(y,x4)==black)) {

                    // Initialisation : on arrive sur une potentielle poutre donc on stocke les coordonnées du début de cette poutre
                    if (longueurActuelle == 0){
                        x_current = x0;
                        y_current = y;
                    }

                    longueurActuelle += 1;
                    x_end_current = x0;
                    y_end_current = y;
                }

                // Fin : on touche un pixel blanc donc on est hors de la poutre
                else {
                    if (longueurActuelle > longueurPoutreMax) { // Si on a détecté une poutre plus longue qu'avant, on la stocke (elle remplace la précédente)
                        x_debut = x_current;
                        y_debut = y_current;
                        x_fin = x_end_current;
                        y_fin = y_end_current;
                        longueurPoutreMax = longueurActuelle;
                        x_current = x0;
                        y_current = y;
                        longueurActuelle = 0;
                    }
                }
            }
        }
    }

    cout << x_debut << " " << y_debut << " " << x_fin << " " << y_fin << " " << longueurPoutreMax << endl;

}



/**
 * @brief retireSegment
 * @param img
 * @param x_deb
 * @param x_fin
 * @param y_deb
 * @param y_fin
 */
void retireSegment(Img img, const int& x_deb, const int& x_fin, const int& y_deb, const int& y_fin) {

    if (x_deb == x_fin) {
        for (int y=y_deb; y<=y_fin; y++) {
            img(y, x_deb) = WHITE;
            }
        }
    else {
        if (y_deb == y_fin) {
            for (int x=x_deb; x<x_fin; x++){
                img(y_deb,x) = WHITE;
            }
        }
        else {
            for (int x=x_deb; x<=x_fin; x++) {
                double y_calcul = y_deb + (x-x_deb)*(y_fin-y_deb)/(x_fin-x_deb);
                int y = static_cast<int>(y_calcul);
                img(y,x) = WHITE;
            }
        }
    }
};


/**
 * @brief nettoyageDroites : supprime les artefacts présents dans l'image
 * @param img
 * @param imgDroites : Image des droites détectées précédemment
 */
void nettoyageDroites(const Img& img, Img imgDroites) {
    // On rajoute une condition pour avoir un segment détecté : parmi les voisins d'un pixel détecté il doit y en avoir au moins un qui est noir
    for (int x=2; x<img.height()-2; x++){
        for (int y=2; y<img.width()-2; y++){
            if (imgDroites(y,x) == black){
                int somme_byte = 0;
                for (int eps_x = -2; eps_x<3; eps_x++){
                    for (int eps_y = -2; eps_y<3; eps_y++){
                        if(eps_x!=0 || eps_y!=0){
                            somme_byte += imgDroites(y+eps_y, x+eps_x);
                        }
                    }
                }

                if (somme_byte > 3*black+21*white){
                    imgDroites(y, x) = white;
                }
            }
        }
    }
}



/**
 * @brief dessineDroitesLarges : dessine les droites avec un certain seuil de tolérance sur la direction, pour quand même détecter des traits tracés à la main et donc pas parfaitement réguliers
 * @param imgDroitesLarges
 * @param rho_max
 */
void dessineDroitesLarges(const Img& img, Img imgDroitesLarges, int M[], const int& rho_max){
    for (int rho=-rho_max; rho<rho_max+1; rho++){
        for (int theta=0; theta<nb_theta; theta++){
            if (M[(rho+rho_max)*nb_theta + theta] > seuil_hough){
                for (int drho = -tolerance_hough; drho<tolerance_hough+1; drho++) {
                    dessineDroite(imgDroitesLarges, rho+drho, theta, img);
                }
            }
        }
    }
}



/**
 * @brief effaceDroitesLargesCalculees : à partir des images img et imgDroitesLarges, on retire dans img les droites présentes dans imgDroitesLarges, pour ne garder que les éléments qui ne sont pas des poutres
 * @param img
 * @param imgDroitesLarges
 */
void effaceDroitesLargesCalculees(Img img, const Img& imgDroitesLarges){
    for (int x=0; x<img.height(); x++){
        for (int y=0; y<img.width(); y++){
            if (img(y,x) == black && imgDroitesLarges(y,x) == black){
                img(y,x) = white;
            }
        }
    }
}

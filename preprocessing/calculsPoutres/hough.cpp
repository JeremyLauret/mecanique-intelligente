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

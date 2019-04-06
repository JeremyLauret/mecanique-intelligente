#include "pretraitement.h"


// FONCTIONS GLOBALES
// min(i,j)
int min (int i , int j){
    if(i<=j)
        return i ;
    return j ;
}

// Histogramme d'une image img
void histogram(int hist[256],Img img){
    for (int k=0; k<256 ;k++){
        hist[k]=0;
    }
    for (int i=0 ; i<img.width() ; i++){
        for (int j=0 ; j<img.height() ; j++){
            hist[img(i,j)]++;
        }
    }
}



/**
 * @brief pix_trans : calcul du fond de l'image, pour le retrancher à l'image d'origine
 * @param img : Image en entrée de la fonction
 * @param x, y : coordonnées du pixel considéré
 */
void pix_trans(Img img , int x, int y ){
    int a = 255 ;
    // on declare les differentes moyennes suivant differentes directions
    int h, v, d1, d2 ;
    int x1 , x2 , y1 , y2 ; // les postions des pixels a calculer la moyenne

    if( 0<=x-1 ){
        x1 = x-1 ;
    }else{
        x1 = x ;
    }
    if( x+1<img.width() ){
        x2 = x+1 ;
    }else{
        x2 = x ;
    }

    if( 0<=y-1 ){
        y1 = y-1 ;
    }else{
        y1 = y ;
    }
    if( y+1<img.height() ){
        y2 = y+1 ;
    }else{
        y2 = y ;
    }

    // on calcule les moyennes
    h = (img(x1,y)+img(x2,y))/2;
    v = (img(x,y1)+img(x,y2))/2;
    d1 = (img(x1,y1)+img(x2,y2))/2;
    d2 = (img(x2,y1)+img(x1,y2))/2;

    // on calcule leur minimum
    a = min(a, h);
    a = min(a, v);
    a = min(a, d1);
    a = min(a, d2);
    img(x,y) = a ;
}



/**
 * @brief projection_seuil : Projection des pixels d'une image sur du blanc ou du noir, en fonction d'un seuil
 * @param img : image d'origine
 * @param seuil
 */
void projection_seuil(Img img, int seuil){
    for (int i=0;i<img.width();i++){
        for (int j=0;j<img.height(); j++){
            if (img(i,j)> seuil)
                img(i,j)=255;   // BLANC
            else
                img(i,j)=0;     // NOIR
        }
    }
    display(img);
}


/**
 * @brief supprimePixelsIsoles : suppression des pixels noirs isolés dans l'image, i.e. entourés de blancs uniquement
 * @param img
 */
void supprimePixelsIsoles(Img img) {
    int white = 255;
    int black = 0;
    for (int i=1; i<img.width()-1; i++) {
        for (int j=1; j<img.height()-1; j++) {
            if (img(i,j) == black &&
                img(i-1,j-1) == white &&
                img(i-1, j) == white &&
                img(i-1, j+1) == white &&
                img(i, j-1) == white &&
                img(i, j+1) == white &&
                img(i+1, j-1) == white &&
                img(i+1, j) == white &&
                img(i+1, j+1) == white) {
                    img(i,j) = white;
            }
        }
    }
    display(img);
}





// SUPPRESSION DES POUTRES
/**
 * @brief supprimeDroites : détection de droites par transformée de Hough, puis suppression de celles-ci
 * @param img
 */
void supprimeDroites(Img img) {
    int white = 255;
    int black = 0;

    Img directions = img.clone();
    for (int i=0; i<img.width(); i++){
        for (int j=0; j<img.height(); j++){
            directions(i,j) = 0;
        }
    }

    for (int i=1; i<img.width()-1; i++) {
        for (int j=1; j<img.height()-1; j++) {
            if (img(i,j) == black) {
                if (img(i-1,j-1) == black || img(i+1, j+1) == black){
                    directions(i,j) ++;
                }
                if (img(i-1,j) == black || img(i+1, j) == black){
                    directions(i,j) ++;
                }
                if (img(i,j-1) == black || img(i, j+1) == black){
                    directions(i,j) ++;
                }
                if (img(i+1,j-1) == black || img(i-1, j+1) == black){
                    directions(i,j) ++;
                }
            }
        }
    }

    for (int i=1; i<img.width()-1; i++) {
        for (int j=1; j<img.height()-1; j++) {
            if (directions(i,j) >1 && (img(i-1,j)==black || img(i+1,j)==black) && (img(i,j-1)==black || img(i,j+1)==black)) {
                img(i,j) = white;
            }
        }
    }

    display(img);
}




void retireDroites(Img img){

    Img directions = img.clone();
    for (int i=0; i<img.width(); i++){
        for (int j=0; j<img.height(); j++){
            directions(i,j) = 255;  //255 est la valeur "vide"
        }
    }

    for (int i=1; i<img.width()-1; i++){
        for (int j=1; j<img.height()-1; j++){
            if (img(i,j)==black){
                if (img(i,j-1)==black || img(i,j+1)==black){
                    directions(i,j) = 0;
                }
                if (img(i-1,j+1)==black || img(i+1,j-1)==black){
                    directions(i,j) = 1;
                }
                if (img(i-1,j)==black || img(i+1,j)==black){
                    directions(i,j) = 2;
                }
                if (img(i-1,j-1)==black || img(i+1,j+1)==black){
                    directions(i,j) = 3;
                }
            }
        }
    }
    display(directions);
}



// Attention, on considère ici x = en hauteur et y = en largeur
void dessineDroite(Img imgDroites, int rho, int theta, Img img) {
    int rho_max = static_cast<int>(sqrt(imgDroites.width()*imgDroites.width() + imgDroites.height() * imgDroites.height()));
    int nb_rho = 2*rho_max + 1;

    int nb_theta = 180;

    if (theta == 0 || theta == 90) {
        if (theta == 0) {
            if (rho>=0 && rho<imgDroites.width()) {
                for (int y=0; y<imgDroites.width(); y++) {
                    if (img(y, rho) == black){
                        imgDroites(y, rho) = black;
                    }
                }
            }
        }
        else {  // theta = pi/2
            if (rho>=0 && rho<imgDroites.height()) {
                for (int x=0; x<imgDroites.height(); x++) {
                    if (img(rho, x) == black){
                        imgDroites(rho, x) = black;
                    }
                }
            }
        }
    }

    else {
        if (theta > 45 && theta < 135)  {
            for (int x=0; x<imgDroites.height(); x++) {
                float numerateur = rho - x*cos(theta * M_PI/nb_theta);
                float float_y = numerateur / sin(theta * M_PI/nb_theta);
                int y = static_cast<int>(float_y);
                if (y>=0 && y<imgDroites.width() && img(y,x)==black) {
                    imgDroites(y, x) = black;
                }
            }
        }
        else { // Cas theta <= 45 ou theta >= 135
            for (int y=0; y<imgDroites.width(); y++) {
                float numerateur = rho - y*sin(theta * M_PI/nb_theta);
                float float_x = numerateur / cos(theta * M_PI/nb_theta);
                int x = static_cast<int>(float_x);
                if (x>=0 && x<imgDroites.height()  && img(y,x)==black) {
                    imgDroites(y, x) = black;
                }
            }
        }
    }
}



// Transformée de Hough
// Attention, x pour la hauteur et y pour la largeur
void hough(Img img){
    // CONSTANTES

    int rho_max = static_cast<int>(sqrt(img.width()*img.width() + img.height() * img.height()));
    int nb_rho = 2*rho_max + 1;

    // Création de la matrice M(rho, theta)
    int M[nb_theta*nb_rho];
    for (int i=0; i<nb_theta*nb_rho; i++){
        M[i] = 0;
    }

    // Calcul de la transformée de Hough
    calculTransformeeSurImage(img, M, rho_max);


    // Nouvelle image où on affichera uniquement les droites détectées
    Img imgIsolated = img.clone();
    Img imgDroites = img.clone();
    imgDroites.fill(white);
    Img imgDroitesLarges = imgDroites.clone();

    // On recherche les maxima de M et on les affiche, si ils dépassent un certain seuil
    rechercheMaxRhoTheta(img, M, rho_max, imgDroitesLarges, imgDroites);



    // FENETRE 2
    Window W2 = openWindow(img.width(), img.height(), "Droites détectées par transformée de Hough");
    setActiveWindow(W2);

    // On rajoute une condition pour avoir un segment détecté : parmi les voisins d'un pixel détecté il doit y en avoir au moins un qui est noir
    nettoyageDroites(img, imgDroites);
    display(imgDroites);



    // FENETRE 3
    Window W3 = openWindow(img.width(), img.height(), "Droites détectées par transformée de Hough, plus larges");
    setActiveWindow(W3);

    imgDroitesLarges = imgDroites.clone();
    dessineDroitesLarges(img, imgDroitesLarges, M, rho_max);
    display(imgDroitesLarges);



    // FENETRE 4
    Window W4 = openWindow(img.width(), img.height(), "Schéma décomposé, en ayant retiré les poutres et possiblement les flèches");
    setActiveWindow(W4);
    effaceDroitesLargesCalculees(img, imgDroitesLarges);
    display(img);
}

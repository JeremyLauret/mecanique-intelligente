#include "pretraitement.h"


// FONCTIONS GLOBALES
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
 * @brief pix_trans : Fixe la valeur du pixel (x, y) au minimum des moyennes de ses voisins selon
 * l'horizontale, la verticale et les deux diagonales.
 * Utile au calcul du fond en vue de le retrancher à l'image.
 * @param img : Image en entrée de la fonction.
 * @param x, y : coordonnées du pixel considéré.
 */
void pix_trans(Img img , int x, int y ){
    // Coordonnées des pixels voisins.
    int x1 = x > 0 ? x - 1 : x;
    int x2 = x < img.width() - 1 ? x + 1 : x;
    int y1 = y > 0 ? y - 1 : y;
    int y2 = y < img.height() - 1 ? y + 1 : y;
    // Calcul des moyennes.
    int h = (img(x1, y) + img(x2, y)) / 2;
    int v = (img(x, y1) + img(x, y2)) / 2;
    int d1 = (img(x1, y1) + img(x2, y2)) / 2;
    int d2 = (img(x2, y1) + img(x1, y2)) / 2;
    // Affectation du minimum.
    int m = h < v ? h : v;
    m = d1 < m ? d1 : m;
    m = d2 < m ? d2 : m;
    img(x, y) = m;
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
    //display(img);
}


/**
 * @brief supprimePixelsIsoles : suppression des pixels noirs isolés dans l'image, i.e. entourés de blancs uniquement
 * @param img
 */
void supprimePixelsIsoles(Img img) {
    int white = 255;
    for (int i=1; i<img.width()-1; i++) {
        for (int j=1; j<img.height()-1; j++) {
            if (img(i,j) == Imagine::BLACK &&
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
//    display(img);
}


// SUPPRESSION DES POUTRES
/**
 * @brief supprimeDroites : détection de droites par transformée de Hough, puis suppression de celles-ci
 * @param img
 */
void supprimeDroites(Img img) {
    int white = 255;

    Img directions = img.clone();
    for (int i=0; i<img.width(); i++){
        for (int j=0; j<img.height(); j++){
            directions(i,j) = 0;
        }
    }

    for (int i=1; i<img.width()-1; i++) {
        for (int j=1; j<img.height()-1; j++) {
            if (img(i,j) == Imagine::BLACK) {
                if (img(i-1,j-1) == Imagine::BLACK || img(i+1, j+1) == Imagine::BLACK){
                    directions(i,j) ++;
                }
                if (img(i-1,j) == Imagine::BLACK || img(i+1, j) == Imagine::BLACK){
                    directions(i,j) ++;
                }
                if (img(i,j-1) == Imagine::BLACK || img(i, j+1) == Imagine::BLACK){
                    directions(i,j) ++;
                }
                if (img(i+1,j-1) == Imagine::BLACK || img(i-1, j+1) == Imagine::BLACK){
                    directions(i,j) ++;
                }
            }
        }
    }

    for (int i=1; i<img.width()-1; i++) {
        for (int j=1; j<img.height()-1; j++) {
            if (directions(i,j) >1 && (img(i-1,j)==Imagine::BLACK || img(i+1,j)==Imagine::BLACK) && (img(i,j-1)==Imagine::BLACK || img(i,j+1)==Imagine::BLACK)) {
                img(i,j) = white;
            }
        }
    }

    //display(img);
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
            if (img(i,j) == Imagine::BLACK){
                if (img(i,j-1)==Imagine::BLACK || img(i,j+1)==Imagine::BLACK){
                    directions(i,j) = 0;
                }
                if (img(i-1,j+1)==Imagine::BLACK || img(i+1,j-1)==Imagine::BLACK){
                    directions(i,j) = 1;
                }
                if (img(i-1,j)==Imagine::BLACK || img(i+1,j)==Imagine::BLACK){
                    directions(i,j) = 2;
                }
                if (img(i-1,j-1)==Imagine::BLACK || img(i+1,j+1)==Imagine::BLACK){
                    directions(i,j) = 3;
                }
            }
        }
    }
    //display(directions);
}

#include <Imagine/Images.h>
#include <Imagine/Graphics.h>
#include <iostream>
#include <cmath>

using namespace Imagine;
using namespace std;

typedef Image<byte> Img;

#include "pretraitement.h"


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
 * @brief getPreprocessedFullImage : retire l'illumination variable et effectue une projection sur le noir et le blanc de l'image, puis ne garde que des traits d'un pixel de large
 * @param img : image que l'on traite, qui est la sortie intéressante de cette fonction
 * @param img_or : image d'origine, extraite du fichier input
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
    /////////////////////////////////////////////////////////////////////////
    // pretraitement avec les moyennes suivant les directions de chaque pixel
    /////////////////////////////////////////////////////////////////////////
    // Img est un type representant une image et img est le nom de la variable
    // img_or = image originale, chargée avec le fichier input, img = image traitée
    Img img_or, img;

    if (!load(img_or,srcPath("schema_isolated_set.jpg"))) // Stop si l'image n'est pas chargee. ATTENTION : seules les images JPG sont prises en charge pour le moment.
        return 0;

    img = img_or.clone();
    Window W1 = openWindow(img.width(), img.height(), "Image traitée");
    cout << "Image : width = " << img.width() << ", height = " << img.height() << endl;

    display(img_or);
    milliSleep(1000);

    // Pré-traitement de l'image, pour obtenir une image noir&blanc sans illumination et avec des traits d'un pixel de large uniquement
    getPreprocessedFullImage(img, img_or);

    milliSleep(1500);

    // Transformée de Hough
    std::vector<Img> outputHough = hough(img);  // outputHough = (Image traitée, avec les composantes connexes isolée, Image complémentaire, avec les poutres isolées)


    endGraphics(); // En realite, on n'arrive jamais ici...
    return 0;
}

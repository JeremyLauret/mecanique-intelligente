#include <Imagine/Images.h>
#include <Imagine/Graphics.h>
#include <iostream>
#include <cmath>

using namespace Imagine;
using namespace std;

typedef Image<byte> Img;

#include "pretraitement.h"


int main() {
    /////////////////////////////////////////////////////////////////////////
    // pretraitement avec les moyennes suivant les directions de chaque pixel
    /////////////////////////////////////////////////////////////////////////
    // Img est un type representant une image et img est le nom de la variable
    Img img_or, img,  fond_blanc ;

    if (!load(img_or,srcPath("schema_isolated_set.jpg"))) // Stop si l'image n'est pas chargee. ATTENTION : seules les images JPG sont prises en charge pour le moment.
        return 0;

    img = img_or.clone();
    Window W1 = openWindow(img.width(), img.height(), "Image traitée");
    cout << "Image : width = " << img.width() << ", height = " << img.height() << endl;

    display(img_or);
    milliSleep(1000);

    // j inverse l image pour travailler avec le modele additif
    fond_blanc = img.clone();
    fond_blanc.fill(255);
    img = fond_blanc - img;
    for(int i = 0 ; i < nb_iter ; i++){
        for(int x = 0 ; x< img.width()  ; x++){
            for(int y = 0 ; y < img.height() ; y++){
                pix_trans(img, x, y);
            }
        }
        //display(img);
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
    display(img);

    /////////////////////////////////////////////////////////////////////////
    // calcul de seuil et projection sur noir ou blanc des pixels
    /////////////////////////////////////////////////////////////////////////

    int hist[256], hist_derv[256] ;
    histogram(hist, img);
    double m = 0. ;
    for(int i = 0 ; i<256 ; i++){
        //hist_derv[i] = (hist[i+1]-hist[i-1])/2 ;
        //drawLine(i,int(img.height()/2)*0,i,0*int(img.height()/2)+hist[i],RED);
        m+=hist[i]*i/(img.height()*img.width());
    }

    //hist_derv[0] = hist[1]-hist[0];
    //hist_derv[255] = hist[255]-hist[254];

    //cout << m << endl;
    projection_seuil(img,int(m)-5);
    supprimePixelsIsoles(img);



    // Suppression des poutres
    supprimeDroites(img);

    retireDroites(img);
    milliSleep(1500);

    hough(img);


    endGraphics(); // En realite, on n'arrive jamais ici...
    return 0;
}

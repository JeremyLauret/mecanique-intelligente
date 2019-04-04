#include "pixel.h"

#include <algorithm>

const float POURCENT=0.5f;

// Fonction affine amenant les min et max de F (ignorant les POURCENT valeurs
// extremes de chaque cote) a 0 et 255.
void affineContraste(Image<float> F, float& a, float& b) {
    F=F.clone();
    std::sort(F.begin(), F.end());
    int p = (int)(F.totalSize()*POURCENT/100);
    float min = F[p];
    float max = F[F.totalSize()-1-p];
    a = (min<max)? 255.0f/(max-min): 0;
    b = (min<max)? -a*min: 255.0f/2;
}

// Renvoie une image affichable.
Image<byte> affichable(const Image<float>& F) {
    float a,b;
    affineContraste(F, a, b);
    Image<byte> I(F.width(), F.height());
    for(int i=0; i<I.height(); i++)
        for(int j=0; j<I.width(); j++) {
            float f = a*F(j,i)+b + 0.5f; 
            if(f<0) f=0;
            if(f>255) f=255;
            I(j,i) = (byte)f;
        }
    return I;
}

// Affichage d'une image avec intensites reelles.
void affiche(const Image<float>& F) {
    display(affichable(F));
}

// Prend la partie reelle de chaque pixel d'une image complexe.
Image<float> realImage(const Image< std::complex<float> >& F) {
    const int w=F.width(), h=F.height();
    Image<float> I(w,h);
    for(int i=0; i<h; i++)
        for(int j=0; j<w; j++)
            I(j,i) = F(j,i).real();    
    return I;
}


// Genere une image plus grande en rajoutant des 0.
Image< std::complex<float> > agrandis(const Image< std::complex<float> >& I,
                                      int w, int h) {
    if(w==I.width() && h==I.height())
        return I.clone();
    Image< std::complex<float> > I2(w,h);
    I2.fill(0.0f);
    for(int i=0; i<I.height(); i++)
        for(int j=0; j<I.width(); j++)
            I2(j,i) = I(j,i);
    return I2;
}

Etiquette::Etiquette(){

}

Etiquette::Etiquette(int n){
    numero=n;
    taille=1;
}

//constructeur
Pixel::Pixel(){

}

Pixel::Pixel(int i0, int j0){
    i=i0;
    j=j0;
    etiquette=new int(0);
    nb_etiquettes_vois=0;
}

//destructeur
Pixel::~Pixel(){

}

//recuperer le numero de la ligne
int Pixel::get_i() const{
    return i;
}

//recupere le numero de la colonne
int Pixel::get_j() const{
    return j;
}



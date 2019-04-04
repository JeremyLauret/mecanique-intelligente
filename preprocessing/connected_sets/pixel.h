#pragma once

#include <complex>
#include <Imagine/Images.h>
using namespace Imagine;

Image<byte> affichable(const Image<float>& F);
void affiche(const Image<float>& F);

class Etiquette{
public:
    int numero;
    int taille;
    Etiquette();
    Etiquette(int n);
};

class Pixel{
private:
    int i;
    int j;
public:
    int* etiquette;
    int nb_etiquettes_vois;
    std::vector<int*> etiquettes_vois;
    Pixel(); //constructeur
    Pixel(int i0,int j0);
    ~Pixel(); //destructeur
    int get_i() const;
    int get_j() const;

};



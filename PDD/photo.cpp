#include "photo.h"
#include "pixel.h"

//constructeur
Photo::Photo(){

}

Photo::Photo(int n0,int m0){
    n=n0;
    m=m0;
    tab=new Pixel[n*m];
}

//destructeur
Photo::~Photo(){

}

//recupere le nb de lignes
int Photo::get_n(){
    return n;
}

//recupere le nb de colonnes
int Photo::get_m(){
    return m;
}

//mets un pixel de coordonnees i,j dans le tableau (a la place i,j)
void Photo::set(int i,int j){
    Pixel p=Pixel(i,j);
    tab[i+j*n]=p;
}

void Photo::set(int i,int j,Pixel p){
    tab[i+j*n]=p;
}

//recupere le pixel de coordonnees i,j
Pixel Photo::get(int i,  int j){
    return tab[i+j*n];
}

//augmenter le nb d'etiquettes voisines des voisins
void Photo::update_voisins(int i, int j,int* ne){
    if(i!=0){
        tab[(i-1)+n*j].nb_etiquettes_vois+=1;
        tab[(i-1)+n*j].etiquettes_vois.push_back(ne);
        if(j!=0){
            tab[(i-1)+n*(j-1)].nb_etiquettes_vois+=1;
            tab[(i-1)+n*(j-1)].etiquettes_vois.push_back(ne);
        }
        if(j!=m){
            tab[(i-1)+n*(j+1)].nb_etiquettes_vois+=1;
            tab[(i-1)+n*(j+1)].etiquettes_vois.push_back(ne);
        }
    }
    if(i!=n){
        tab[(i+1)+n*j].nb_etiquettes_vois+=1;
        tab[(i+1)+n*j].etiquettes_vois.push_back(ne);
        if(j!=0){
            tab[(i+1)+n*(j-1)].nb_etiquettes_vois+=1;
            tab[(i+1)+n*(j-1)].etiquettes_vois.push_back(ne);
        }
        if(j!=m){
            tab[(i+1)+n*(j+1)].nb_etiquettes_vois+=1;
            tab[(i+1)+n*(j+1)].etiquettes_vois.push_back(ne);
        }
    }
    if(j!=0){
        tab[(i)+n*(j-1)].nb_etiquettes_vois+=1;
        tab[(i)+n*(j-1)].etiquettes_vois.push_back(ne);
    }
    if(j!=m){
        tab[(i)+n*(j+1)].nb_etiquettes_vois+=1;
        tab[(i)+n*(j+1)].etiquettes_vois.push_back(ne);
    }

}

#ifndef PHOTO_H
#define PHOTO_H
#include "pixel.h"

class Photo{
private:
    int n;
    int m;
    Pixel* tab;
public:
    Photo();
    Photo(int n0,int m0);
    ~Photo();
    int get_n();
    int get_m();
    void set(int i,int j);
    void set(int i,int j, Pixel p);
    Pixel get(int i,  int j);
    void update_voisins(int i, int j,int* ne);
};
#endif // PHOTO_H

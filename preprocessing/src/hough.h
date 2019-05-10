#pragma once

#include <Imagine/Images.h>
#include <Imagine/Graphics.h>
#include <iostream>
#include <cmath>
#include <vector>

#include "pretraitement.h"

typedef Imagine::Image<byte> Img;

const int white = 255;
const int black = 0;


// CONSTANTES SPECIFIQUES A LA TRANSFORMEE DE HOUGH
const int _NB_THETA{ 180 };


// FONCTIONS
/**
 * Remplit l'histogramme H des couples (rho, theta) obtenus par transformée de Hough des pixels de
 * l'image.
 *
 * H est de taille nb_theta x (2*rho_max + 1) (rho pouvant prendre des valeurs négatives).
 */
void computeHoughHistogram(const Img& img, int H[], int rho_max, int nb_theta=_NB_THETA);

/*
 * Colorie les pixels situés sur le segment [(x_deb, y_deb), (x_fin, y_fin)].
 */
void colorSegment(Img& img, int x_deb, int x_fin, int y_deb, int y_fin, int color);

/*
 * Détermine la position et la longueur de la plus longue succession de pixels noirs située sur la
 * droite paramétrée par rho et theta.
 *
 * La longueur et les coordonnées du segment sont affectés aux paramètres.
 */
void findSegmentsEdges(const Img& img, int rho, int theta, int& x_debut, int& y_debut,
                    int& x_fin, int& y_fin, int pixel_range, int nb_theta=_NB_THETA);

/**
 * Identifie les lignes de l'image les plus longues et stocke leurs coordonnées de début et de fin
 * dans les vecteurs en paramètre.
 *
 * min_relative_size fixe la taille minimale relative à la ligne avec le plus de pixels pour qu'une
 * autre ligne soit retenue.
 */
void findLongestLines(Img img, int H[], int rho_max, std::vector<int>& beg_rows,
                      std::vector<int>& end_rows, std::vector<int>& beg_cols,
                      std::vector<int>& end_cols, int min_relative_size, int pixel_range,
                      int nb_theta);


/**
 * Applique l'algorithme de détection de droites de Hough à l'image fournie pour en retirer les
 * droites les plus longues.
 *
 * Renvoie un vecteur de taille 2 contenant d'une part l'image d'origine privée des plus longues
 * droites, d'autre part l'image contenant uniquement les droites retirées.
 *
 * Toute droite contenant plus de min_relative_size * <nombre de pixels de la plus longue droite>
 * pixels est retirée.
 */
std::vector<Img> hough(const Img& img, int min_relative_size, int pixel_range, int beam_thickness,
                       int nb_theta=_NB_THETA);

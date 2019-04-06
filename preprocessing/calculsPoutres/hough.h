#pragma once

#include <Imagine/Images.h>
#include <Imagine/Graphics.h>
#include <iostream>
#include <cmath>

#include "pretraitement.h"

using namespace Imagine;
using namespace std;

typedef Image<byte> Img;

const int grheight =128,
          barheight =32,
          nb_iter = 15 ;

const int white = 255;
const int black = 0;


// CONSTANTES SPECIFIQUES A LA TRANSFORMEE DE HOUGH
const int nb_theta = 180;

const int seuil_hough = 60;
const int ecart_hough = 5;
const int tolerance_hough = 2;




// FONCTIONS
/**
 * @brief calculTransformeeSurImage : Calcul le tableau M des rho et theta correspondant aux droites de l'image img
 * @param img : image d'origine
 * @param M : tableau de valeurs des rho et theta, de taille nb_theta*nb_rho
 * @param NB_theta
 * @param rho_max
 */
void calculTransformeeSurImage(const Img& img, int M[], const int& rho_max);


void rechercheMaxRhoTheta(const Img& img, int M[], const int& rho_max, Img imgDroitesLarges, Img imgDroites);

/**
 * @brief nettoyageDroites : supprime les artefacts présents dans l'image
 * @param img
 * @param imgDroites : Image des droites détectées précédemment
 */
void nettoyageDroites(const Img& img, Img imgDroites);

/**
 * @brief dessineDroitesLarges : dessine les droites avec un certain seuil de tolérance sur la direction, pour quand même détecter des traits tracés à la main et donc pas parfaitement réguliers
 * @param imgDroitesLarges
 * @param rho_max
 */
void dessineDroitesLarges(const Img &img, Img imgDroitesLarges, int M[], const int& rho_max);

/**
 * @brief effaceDroitesLargesCalculees : à partir des images img et imgDroitesLarges, on retire dans img les droites présentes dans imgDroitesLarges, pour ne garder que les éléments qui ne sont pas des poutres
 * @param img
 * @param imgDroitesLarges
 */
void effaceDroitesLargesCalculees(Img img, const Img& imgDroitesLarges);

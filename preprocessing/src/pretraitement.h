#pragma once

#include <Imagine/Images.h>
#include <Imagine/Graphics.h>
#include <iostream>
#include <cmath>

typedef Imagine::Image<byte> Img;


// FONCTIONS GLOBALES
//Min(i,j)
int min (int i , int j);
// Histogramme d'une image img
void histogram(int hist[256],Img img);


// TRAITEMENT DE L'ILLUMINATION VARIABLE ET PROJECTION EN NOIR ET BLANC
/**
 * @brief pix_trans : calcul du fond de l'image, pour le retrancher à l'image d'origine
 * @param img : Image en entrée de la fonction
 * @param x, y : coordonnées du pixel considéré
 */
void pix_trans(Img img , int x, int y );

/**
 * @brief projection_seuil : Projection des pixels d'une image sur du blanc ou du noir, en fonction d'un seuil
 * @param img : image d'origine
 * @param seuil
 */
void projection_seuil(Img img, int seuil);

/**
 * @brief supprimePixelsIsoles : suppression des pixels noirs isolés dans l'image, i.e. entourés de blancs uniquement
 * @param img
 */
void supprimePixelsIsoles(Img img);

// SUPPRESSION DES POUTRES
/**
 * @brief supprimeDroites : détection de droites par transformée de Hough, puis suppression de celles-ci
 * @param img
 */
void supprimeDroites(Img img);


void retireDroites(Img img);

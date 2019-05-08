import numpy as np
from attributions import *
from data_example import *


class Poutre :
    """ Contient toutes les informations d'une poutre
    Prend en compte un numéro d'identifiant pour la poutre, correspondant à son index dans data"""
    def __init__(self, identifiant):
        ## Informations extraites des autres patchs
        self.id = identifiant
        # Cotes définies sur la figure, on enregistre la lettre et on met à jour la longueur de la poutre
        self.longueurs=[]
        # Forces ponctuelles appliquées à la poutre, on enregistre sa lettre de norme, son vecteur directeur, son point d'application proportionnellement à la longueur de la poutre
        self.forces_ponctuelles=[]
        # Forces réparties, on enregistre, la densité et la longueur selon la poutre (points d'application de début et de fin)
        self.forces_lineiques=[]
        # Type de support et point d'application
        self.liaisons=[]
        # Moments : Points d'application et valeur des moments et sens de la flèche
        self.moments=[]
        # Points spéciaux : on note la lettre et la coordonnée (projeté du centre de la fenêtre sur la poutre)
        self.points_speciaux=[]


        xmin = data[identifiant, 0]
        xmax = data[identifiant, 1]
        ymin = data[identifiant, 2]
        ymax = data[identifiant, 3]
        # Coordonnées par rapport à l'image toute entière. ATTENTION : DETERMINER DANS QUELLE DIRECTION EST LA POUTRE PAR RAPPORT A LA FENETRE
        self.origine = originePoutre(I, xmin, xmax, ymin, ymax)
        # Vecteur directeur par rapport à l'horizontale de la fenêtre. Pour des fenêtres pas trop petites, c'est la diagonale de la fenêtre, à voir en fonction d'un seuil du type "si l'angle est supérieur à 5 degrés alors c'est une poutre selon la diagonale". Le seuil est le quotient entre la hauteur et la largeur de la fenêtre
        self.vecteur_directeur = vecteurDirecteur(self.origine, xmin, xmax, ymin, ymax)
        # Distance en pixels, i.e. taille de la poutre en pixels dans l'image
        self.distance= np.sqrt((xmax-xmin)**2 + (ymax-ymin)**2)

        # Poutres avec laquelle la poutre actuelle est liée
        self.poutres_voisines=[]

    def display(self):
        print("Poutre n°" + str(self.id))
        print("Origine : x = " + str(self.origine[0]) + ", y = " + str(self.origine[1]))
        print("Vecteur directeur : [" + str(self.vecteur_directeur[0]) + ", " + str(self.vecteur_directeur[1]) + "]")
        print("Longueur de la poutre (en pixels de l\'image) : " + str(self.distance))
        print("Poutres connectées : " + str(self.poutres_voisines))
        print("Points spéciaux de la poutre : " + str(self.points_speciaux))
        print("Cotes liées à la poutre : " + str(self.longueurs))
        print("Liaisons : " + str(self.liaisons))
        print("Moments : " + str(self.moments))
        print("Forces ponctuelles : " + str(self.forces_ponctuelles))
        print("Forces linéiques : " + str(self.forces_lineiques))
        print()

    def getOrigin(self):
        return self.origine

    def getVectDir(self):
        return self.vecteur_directeur

    def setCote(self, lettreNorme, proportion):
        """ Prend la lettre de la longueur de la cote ainsi que le rapport de la longueur de la cote sur celle de la poutre """
        self.longueurs.append((lettreNorme, proportion))
    
    def setForce(self, force, prop):
        if (force.getType() == "force_ponctuelle"):
            self.forces_ponctuelles.append((force.getNorme(), force.getVectDir(), prop))
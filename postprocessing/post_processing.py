import numpy as np


""" Variable globale : data = [[xmin, xmax, ymin, ymax, label], ...] sorted by label numbers"""

# Détermine, pour une fenêtre contenant une poutre, dans quel sens est orientée la poutre
def originePoutre(I, xmin, xmax, ymin, ymax):
    alpha = 1/10
    dim_x_fenetre = (xmax-xmin)/4
    dim_y_fenetre = (ymax-ymin)/4
    
    count_diag_haut_gauche = np.sum(I[xmin:xmin+dim_x_fenetre, ymin:y_min+dim_y_fenetre]) + np.sum(I[xmax-dim_x_fenetre:xmax, ymax-dim_y_fenetre:ymax])
    count_diag_bas_gauche = np.sum(I[xmin:xmin+dim_x_fenetre, ymax-dim_y_fenetre:ymax]) + np.sum(I[xmax-dim_x_fenetre:xmax, ymin:y_min+dim_y_fenetre])
    
    if ((xmax-xmin) < alpha * (ymax-ymin)):
        return (xmax+xmin)/2, ymin
    elif ((ymax-ymin) < alpha * (xmax-xmin)):
        return xmin, (ymin+ymax)/2

    if count_diag_haut_gauche > count_diag_bas_gauche :
        return xmin, ymin
    else:
        return xmin, ymax


def vecteurDirecteur(origine, xmin, xmax, ymin, ymax):
    alpha = 1/10

    if ((xmax-xmin) < alpha * (ymax-ymin)): #Poutre horizontale
        return [0,ymax-ymin]
    elif ((ymax-ymin) < alpha * (xmax-xmin)):   #Poutre verticale
        return [xmax-xmin,0]

    if (origine[0] == xmin and origine[1]==ymin):   #Cas où l'origine est en haut à gauche de la fenêtre
        return [xmax-xmin, ymax-ymin]
    else:
        return[xmin-xmax, ymax-ymin]


class Poutre :
    """ Contient toutes les informations d'une poutre 
    Prend en compte un numéro d'identifiant pour la poutre, correspondant à son index dans data"""
    def __init__(self, identifiant):
        ## Informations extraites des autres patchs
        self.id = identifiant
        # Cotes définies sur la figure, on enregistre la lettre et on met à jour la longueur de la poutre
        self.longueurs=[]
        # Forces ponctuelles appliquées à la poutre, on enregistre sa lettre de norme, son vecteur directeur
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
        # Vecteur directeur par rapport à l'horizontale de la fenêtre. Pour des fenêtres pas trop petites, c'est la diagonale de la fenêtre, à voir en fonction d'un seuil du type "si l'angle est supérieur à 5 degrés alors c'est une poutre selon la diagonale". Le seuil est le quotient entre la hauteur et la largeur de la fenêtre
        self.vecteur_directeur = vecteurDirecteur(self.origine, xmin, xmax, ymin, ymax)
        # Coordonnées par rapport à l'image toute entière. ATTENTION : DETERMINER DANS QUELLE DIRECTION EST LA POUTRE PAR RAPPORT A LA FENETRE
        self.origine = originePoutre(I, xmin, xmax, ymin, ymax)
        # Distance en pixels, i.e. taille de la poutre en pixels dans l'image
        self.distance= np.sqrt((xmax-xmin)**2 + (ymax-ymin)**2)
        # Poutres avec laquelle la poutre actuelle est liée
        self.poutres_voisines=[]



## 1ere étape : Vecteur directeur + origine + distance
## 2eme étape : Longueurs
## 3eme étape : Liaisons
## 4eme étape : Forces
## 5eme étape : Moments
## 6eme étape : Points spéciaux
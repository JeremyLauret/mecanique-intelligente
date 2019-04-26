import numpy as np
import matplotlib.pyplot as plt

I = plt.imread("schema_crop.jpg")


""" Variable globale : data = [[xmin, xmax, ymin, ymax, label], ...] sorted by label numbers"""
## Exemple de data, code à adapter pour aller effectivement chercher les bonnes images dans les répertoires afin de créer cette matrice

##### ATTENTION : C++ NE CONSIDERE PAS LES MEMES AXES POUR LES IMAGES QUE PYTHON (LES AXES X ET Y SONT PROBABLEMENT INVERSES DONC IL FAUDRA PRENDRE CA EN COMPTE !!!)
LABELS = {"poutre": 1, "appui_simple":6, "appui_glissiere":7, "encastrement":8, "rotule":9, "cote":15, "force_ponctuelle":16, "force_distribuee":17, "a":18, "l":26, "m":27, "F":35, "theta":57}

data = np.array([
    [119, 43, 127, 245, LABELS["poutre"]], 
    [52, 366, 54, 499, LABELS["poutre"]], 
    [57, 245, 123, 358, LABELS["poutre"]], 
    [122, 7, 172, 69, LABELS["appui_simple"]],
    [122, 213, 168, 271, LABELS["appui_glissiere"]],
    [33, 498, 54, 499, LABELS["encastrement"]],
    [48, 356, 60, 369, LABELS["rotule"]],
    [45, 240, 111, 353, LABELS["cote"]],
    [189, 21, 210, 247, LABELS["cote"]],
    [187, 361, 209, 511, LABELS["cote"]],
    [47, 110, 114, 134, LABELS["force_ponctuelle"]],
    [62, 283, 73, 295, LABELS["a"]],
    [204, 112, 232, 129, LABELS["l"]],
    [209, 419, 221, 438, LABELS["m"]],
    [60, 133, 81, 150, LABELS["F"]],
    [103, 279, 120, 303, LABELS["theta"]]
])
col0 = np.copy(data[:,1])
data[:,1]=data[:,2]
data[:,2]=col0      # On a bien data = [xmin, xmax, ymin, ymax, label]


#Nombre total de poutres dans l'image
nb_poutres = np.sum(np.where(data[:,4]==LABELS["poutre"], 1, 0))

# Indices des cotes dans data
ind_cotes = []
# Indices des forces dans data
ind_forces = []
# Indices des liaisons dans data
ind_liaisons = []
for i in range(nb_poutres, len(data)):
    if data[i,-1] == LABELS["cote"]:
        ind_cotes.append(i)
    elif data[i,-1] == LABELS["force_ponctuelle"] or data[i,-1] == LABELS["force_distribuee"]:
        ind_forces.append(i)
    elif data[i,-1] == LABELS["appui_simple"] or data[i,-1] == LABELS["appui_glissiere"] or data[i,-1] == LABELS["encastrement"] or data[i,-1] == LABELS["rotule"]:
        ind_liaisons.append(i)


## FONCTIONS AUXILIAIRES DE LA CLASSE POUTRE
# Détermine, pour une fenêtre contenant une poutre, dans quel sens est orientée la poutre
def originePoutre(I, xmin, xmax, ymin, ymax):
    alpha = 1/10
    dim_x_fenetre = int((xmax-xmin)/4)
    dim_y_fenetre = int((ymax-ymin)/4)
    
    count_diag_haut_gauche = np.sum(I[xmin:xmin+dim_x_fenetre, ymin:ymin+dim_y_fenetre]) + np.sum(I[xmax-dim_x_fenetre:xmax, ymax-dim_y_fenetre:ymax])
    count_diag_bas_gauche = np.sum(I[xmin:xmin+dim_x_fenetre, ymax-dim_y_fenetre:ymax]) + np.sum(I[xmax-dim_x_fenetre:xmax, ymin:ymin+dim_y_fenetre])
    
    if ((xmax-xmin) < alpha * (ymax-ymin)):
        return int((xmax+xmin)/2), ymin
    elif ((ymax-ymin) < alpha * (xmax-xmin)):
        return xmin, int((ymin+ymax)/2)

    if count_diag_haut_gauche < count_diag_bas_gauche : #Pixel blanc = 255, pixel noir = 0, donc on cherche le minimum
        return xmin, ymin
    else:
        return xmax, ymin


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


# 1ere étape : Vecteur directeur + origine + distance
## 2eme étape : Longueurs
## 3eme étape : Liaisons
## 4eme étape : Forces
## 5eme étape : Moments
## 6eme étape : Points spéciaux



## Lecture des données et création des poutres
Poutres = []

for i in range(nb_poutres):
    poutre = Poutre(i)
    Poutres.append(poutre)

## Affichage de toutes les données des poutres
for i in range(nb_poutres):
    Poutres[i].display()
import numpy as np
import matplotlib.pyplot as plt

I = plt.imread("schema_crop.jpg")


""" Variable globale : data = [[xmin, xmax, ymin, ymax, label], ...] sorted by label numbers"""

##### ATTENTION : C++ NE CONSIDERE PAS LES MEMES AXES POUR LES IMAGES QUE PYTHON (LES AXES X ET Y SONT PROBABLEMENT INVERSES DONC IL FAUDRA PRENDRE CA EN COMPTE !!!)
# Dictionnaire des labels : en ayant le nom d'un élément, on retrouve son label (int)
LABELS = {"poutre": 1, "appui_simple":6, "appui_glissiere":7, "encastrement":8, "rotule":9, "cote":15, "force_ponctuelle":16, "force_distribuee":17, "a":18, "l":26, "m":27, "F":35, "theta":57}

# Calcul du nombre de labels
nb_labels = 1
for label in LABELS:
    if LABELS[label] > nb_labels:
        nb_labels = LABELS[label] +1

# Création d'un dictionnaire inversé : à partir du label on retrouve le nom de l'élément en LABELS_INV[label]
LABELS_INV = [""]*nb_labels
for label in LABELS:
    LABELS_INV[LABELS[label]] = label


## Exemple de data, code à adapter pour aller effectivement chercher les bonnes images dans les répertoires afin de créer cette matrice
data = np.array([
    [119, 43, 127, 245, LABELS["poutre"]],
    [52, 366, 54, 499, LABELS["poutre"]],
    [57, 245, 123, 358, LABELS["poutre"]],
    [122, 7, 172, 69, LABELS["appui_simple"]],
    [122, 213, 168, 271, LABELS["appui_glissiere"]],
    [33, 498, 72, 514, LABELS["encastrement"]],
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


## Constantes liées à la discrétisation du problème et aux proportions
NB_PAS_POUTRES = 5   # On discrétise les poutres en NB_PAS_POUTRES afin de définir ensuite les proportions des cotes par rapport à cette discrétisation (ex : une cote vaut 3/5 de la poutre associée et non 0.57385219)




##FONCTIONS GENERALES POUR LA PROXIMITE
def dist((x1,y1),(x2,y2)):
    '''norme euclidienne entre deux points du plan'''
    return(np.sqrt((x1-x2)**2 + (y1-y2)**2))

def rect_distance((xmin_1,xmax_1,ymin_1,ymax_1), (xmin_2,xmax_2,ymin_2,ymax_2)):
    '''distance entre deux rectangles dont les côtés sont selon les mêmes axes (pas de rotation)'''
    left = ymax_2 < ymin_1 #the second rectangle is at the left side of the first
    right = ymax_1 < ymin_2
    bottom = xmax_1 < xmin_2
    top = xmax_2 < xmin_1
    if top and left:
        return dist((xmin_1, ymin_1), (xmax_2, ymax_2))
    elif left and bottom:
        return dist((xmax_1, ymin_1), (xmin_2, ymax_2))
    elif bottom and right:
        return dist((xmax_1, ymax_1), (xmin_2, ymin_2))
    elif right and top:
        return dist((xmin_1, ymax_1), (xmax_2, ymin_2))
    elif left:
        return ymin_1 - ymax_2
    elif right:
        return ymin_2 - ymax_1
    elif bottom:
        return xmin_2 - xmax_1
    elif top:
        return xmin_1 - xmax_2
    else:             # rectangles intersect
        return 0

##FONCTION POUR ASSOCIER UN CARACTERE A UN patch
def attribueCaractere(I,id_car):
    '''attribue le caractère dont l'index dans data est id_car à l'élément sémantique le plus proche'''
    [xmin, xmax, ymin, ymax, label] = data[id_car]
    d_min = pow(10,8)#on initialise la distance minimum
    id_min = -1#on initialise l'index du patch le plus proche du caractère dans data
    label_min = -1
    for k in range(len(data)):
        if(k!=id_car):
            [xmin2, xmax2, ymin2, ymax2, label2] = data[k]
            dist = rect_distance((xmin,xmax,ymin,ymax), (xmin2,xmax2,ymin2,ymax2))
            if(dist<d_min):
                id_min = k
                d_min = dist
                label_min = label2
    print('Caractère {} avec index {} dans data attribué à élément {} index {}'.format(label,id_car,label2,id_min))
    return(id_min,label_min)


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
    alpha = 1/5
    dx = xmax - xmin
    dy = ymax - ymin

    if (dx < alpha * dy): #Poutre horizontale
        return [0,dy]
    elif (dy < alpha * dx):   #Poutre verticale
        return [dx,0]

    if (origine[0] == xmin and origine[1]==ymin):   #Cas où l'origine est en haut à gauche de la fenêtre
        return [dx, dy]
    else:
        return[-dx, dy]


def origineVecteurDirecteurForce(I, xmin, xmax, ymin, ymax):
    alpha = 1/2
    dx = xmax-xmin
    dy = ymax-ymin
    dim_x_fenetre = int(dx/10)
    dim_y_fenetre = int(dy/10)

    # Cas d'une force horizontale
    if (dx < alpha * dy):
        count_gauche = np.sum(I[:, ymin:ymin+dim_y_fenetre])
        count_droite = np.sum(I[:, ymax-dim_y_fenetre:ymax])

        if count_gauche < count_droite : # Pixels blancs = 255, noirs = 0, donc on cherche le minimum qui indique le bout de la flèche
            #Pointe de la flèche à gauche, donc on choisit l'origine à droite
            origine = [int((xmin+xmax)/2), ymax]
            vecteurDirecteur = [0, -dy]
            return origine, vecteurDirecteur
        else :
            #Pointe de la flèche à droite, donc on choisit l'origine à gauche
            origine = [int((xmin+xmax)/2), ymin]
            vecteurDirecteur = [0, dy]
            return origine, vecteurDirecteur


    # Cas d'une force verticale
    if (dy < alpha * dx):
        count_haut = np.sum(I[xmin:xmin+dim_x_fenetre, :])
        count_bas = np.sum(I[xmax-dim_x_fenetre:xmax, :])

        if count_haut < count_bas : # Pixels blancs = 255, noirs = 0, donc on cherche le minimum qui indique le bout de la flèche
            #Pointe de la flèche en haut, donc on choisit l'origine en bas
            origine = [xmax, int((ymin+ymax)/2)]
            vecteurDirecteur = [-dx, 0]
            return origine, vecteurDirecteur
        else :
            #Pointe de la flèche en bas, donc on choisit l'origine en haut
            origine = [xmin, int((ymin+ymax)/2)]
            vecteurDirecteur = [dx, 0]
            return origine, vecteurDirecteur


    # Sinon, Cas d'une force quelconque
    count_hg = np.sum(I[xmin:xmin+dim_x_fenetre, ymin:ymin+dim_y_fenetre])
    count_bd = np.sum(I[xmax-dim_x_fenetre:xmax, ymax-dim_y_fenetre:ymax])
    count_hd = np.sum(I[xmin:xmin+dim_x_fenetre, ymax-dim_y_fenetre:ymax])
    count_bg = np.sum(I[xmax-dim_x_fenetre:xmax, ymin:ymin+dim_y_fenetre])
    count_diag_haut_gauche = count_hg + count_bd
    count_diag_bas_gauche = count_hd + count_bg

    if count_diag_haut_gauche < count_diag_bas_gauche :
        #Cas où la force est selon la droite (haut gauche, bas droite)
        if count_hg < count_bd :
            # Cas où la pointe de la flèche est en haut à gauche
            origine = [xmax, ymax]
            vecteurDirecteur = [-dx, -dy]
            return origine, vecteurDirecteur
        else :
            # Cas où la pointe de la flèche est en bas à droite
            origine = [xmin, ymin]
            vecteurDirecteur = [dx, dy]
            return origine, vecteurDirecteur
    else :
        #Cas où la force est selon la droite (bas gauche, haut droite)
        if count_bg < count_hd:
            # Cas où la pointe de la flèche est en bas à gauche
            origine = [xmin, ymax]
            vecteurDirecteur = [dx, -dy]
            return origine, vecteurDirecteur
        else:
            # Cas où la pointe de la flèche est en haut à droite
            origine = [xmax, ymin]
            vecteurDirecteur = [-dx, dy]
            return origine, vecteurDirecteur


def angleAxeX(vecteurDirecteur):
    # Angle retourné en degrés, entre -90° et 90°
    if vecteurDirecteur[0] == 0:
        return 90
    else:
        return np.arctan(vecteurDirecteur[1]/vecteurDirecteur[0]) * 180/np.pi


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

    def getOrigin(self):
        return self.origine

    def getVectDir(self):
        return self.vecteur_directeur

    def setCote(self, lettreNorme, proportion):
        """ Prend la lettre de la longueur de la cote ainsi que le rapport de la longueur de la cote sur celle de la poutre """
        self.longueurs.append((lettreNorme, proportion))


class Cote:
    def __init__(self, identifiant):
        self.id = identifiant
        self.length = ""

        xmin, xmax, ymin, ymax = data[identifiant,:4]

        self.origine = originePoutre(I, xmin, xmax, ymin, ymax)
        self.vecteurDirecteur = vecteurDirecteur(self.origine, xmin, xmax, ymin, ymax)

    def display(self):
        print('Cote n°' + str(self.id))
        print("Longueur : " + str(self.length))
        print("Origine de la cote : x = " + str(self.origine[0]) + ", y = " + str(self.origine[1]))
        print("Vecteur directeur de la cote : " + str(self.vecteurDirecteur))
        print()

    def getId(self):
        return self.id

    def getLength(self):
        return self.length

    def getOrigin(self):
        return self.origine

    def getVectDir(self):
        return self.vecteurDirecteur


class Force:
    def __init__(self, identifiant):
        self.id = identifiant

        self.type = LABELS_INV[data[identifiant,4]]

        xmin, xmax, ymin, ymax = data[identifiant,:4]
        self.origine, self.vecteurDirecteur = origineVecteurDirecteurForce(I, xmin, xmax, ymin, ymax)

        self.angle = angleAxeX(self.vecteurDirecteur)   # Angle en degrés, par rapport à l'axe X

    def display(self):
        print("Force n°" + str(self.id))
        print("Force de type " + self.type)
        print("Origine : x = " + str(self.origine[0]) + ", y = " + str(self.origine[1]))
        print("Vecteur directeur : " + str(self.vecteurDirecteur))
        print("Angle " + str(self.angle))
        print()

    def getId(self):
        return self.id

    def getType(self):
        return self.type

    def getOrigin(self):
        return self.origine

    def getVectDir(self):
        return self.vecteurDirecteur

    def getAngle(self):
        return self.angle


class Liaison:
    def __init__(self, identifiant):
        self.id = identifiant
        self.type = LABELS_INV[data[identifiant,4]]

        xmin, xmax, ymin, ymax = data[identifiant, :4]
        self.centre = [int((xmin+xmax)/2), int((ymin+ymax)/2)]
        self.rayon = np.maximum((xmax-xmin)/2, (ymax-ymin)/2)

    def display(self):
        print("Liaison n°" + str(self.id))
        print("Liaison de type : " + self.type)
        print("Centre du patch de la liaison : x = " + str(self.centre[0]) + ", y = " + str(self.centre[1]))
        print("Demi-plus grand côté : " + str(self.rayon))
        print()

    def getId(self):
        return self.id

    def getType(self):
        return self.type

    def getCenter(self):
        return self.centre

    def getRadius(self):
        return self.rayon


# 1ere étape : Vecteur directeur + origine + distance
## 2eme étape : Longueurs
## 3eme étape : Liaisons
## 4eme étape : Forces
## 5eme étape : Moments
## 6eme étape : Points spéciaux





## Lecture des données et création des poutres, cotes et forces
Poutres = []

for ind_poutre in range(nb_poutres):
    poutre = Poutre(ind_poutre)
    Poutres.append(poutre)

Cotes = []
for ind_cote in range(len(ind_cotes)):
    cote = Cote(ind_cotes[ind_cote])
    Cotes.append(cote)

Forces = []
for ind_force in range(len(ind_forces)):
    force = Force(ind_forces[ind_force])
    Forces.append(force)

Liaisons = []
for ind_liaison in range(len(ind_liaisons)):
    liaison = Liaison(ind_liaisons[ind_liaison])
    Liaisons.append(liaison)


## Attribution d'une cote à une poutre
def attribueCote(cote):
    """
    Prend en entrée une cote et donne l'identifiant de la poutre à laquelle affecter la cote
    """
    epsScal = 0.05 # Seuil de tolérance sur le parallélisme
    alpha = 0.1

    vdC = cote.getVectDir()
    origC = cote.getOrigin()
    normC = np.sqrt(np.dot(vdC, vdC))

    for poutre in Poutres:
        vdP = poutre.getVectDir()
        origP = poutre.getOrigin()
        normP = np.sqrt(np.dot(vdP, vdP))

        prodScalaire = abs( np.dot(vdC, vdP) / (normC * normP) )
        diffX0 = abs(origP[0] - origC[0])
        diffY0 = abs(origP[1] - origC[1])
        diffXf = abs(origP[0] + vdP[0] - origC[0] - vdC[0])
        diffYf = abs(origP[1] + vdP[1] - origC[1] - vdC[1])

        if ((prodScalaire >= 1-epsScal) and
            (   (diffX0 <= alpha * poutre.distance) or
                (diffY0 <= alpha * poutre.distance) or
                (diffXf <= alpha * poutre.distance) or
                (diffYf <= alpha * poutre.distance) ) ) : # Cas de cote parallèle à une poutre, et avec au moins une extrêmité de la cote alignée à celle de la poutre
            proportion = int(normC / normP * NB_PAS_POUTRES) / NB_PAS_POUTRES
            poutre.setCote(cote.length, proportion)

        elif ((prodScalaire < 1-epsScal) and
            (   ((diffX0 <= alpha * poutre.distance) and (diffXf <= alpha * poutre.distance)) or
                ((diffY0 <= alpha * poutre.distance) and (diffYf <= alpha * poutre.distance)) ) ) : # Cas de poutre non parallèle à la cote, mais la cote est exactement alignée avec le début et la fin de la poutre (ex: poutre transverse)
            # même code que dans le if précédent
            proportion = int(normC / normP * NB_PAS_POUTRES) / NB_PAS_POUTRES
            poutre.setCote(cote.length, proportion)


# Application de attribueCote()
for cote in Cotes:
    attribueCote(cote)


## Attribution d'une force à une poutre
def equationDroite(vectDir, orig, x):
    """ Retourne l'ordonnée d'un point d'abscisse x sur un droite caractérisée par son origine et son vecteur directeur """
    a, b = vectDir
    x0, y0 = orig
    c = b*x0 - a*y0
    if (a == 0):
        return None
    else :
        return ((b * x - c)/a)


def attribueForce(force):

    origF = np.array(force.getOrigin())
    vdF = np.array(force.getVectDir())

    for poutre in Poutres:
        origP = np.array(poutre.getOrigin())
        vdP = np.array(poutre.getVectDir())
        normP = np.linalg.norm(vdP)

        #Projection du vecteur force selon le vecteur directeur unitaire de la poutre
        pi_F = np.dot(vdF, vdP)/(normP**2) * vdP

        # Equation de la poutre de vecteur directeur (a,b) : -b*x + a*y + c = 0
        # c de l'équation
        c = vdP[1] * origP[0] - vdP[0] * origP[1]

        # L = distance origine de la force à la poutre
        L = abs(-vdP[1] * origF[0] + vdP[0] * origF[1] + c) / np.sqrt(np.dot(vdP, vdP))
        # d = distance origine de la force à origine de la poutre
        d = np.linalg.norm(origF - origP)
        # x0 = abscisse curviligne du projeté de la force sur la poutre
        x0 = np.sqrt(L*L + d*d)

        X0 = x0 * vdP / normP + origP

        # F - pi_F
        diff = vdF - pi_F
        X = X0 + L/np.linalg.norm(diff) * pi_F

        # Calcul de l'abscisse curviligne du point d'application de la force sur la poutre (valeur comprise entre 0 et 1 si la force s'applique bien à la poutre)
        normeX = np.linalg.norm(X - origP)
        proportion = normeX / normP

        if (np.dot(vdF, vdP) != 0):
            proportion *= np.sign(np.dot(vdF, vdP))
        else :
            proportion *= np.sign(np.dot(vdP, vdF+origF-origP))

        if (proportion >= 0 and proportion <= 1):
            print("Poutre " + str(poutre.id))
            print("Proportion : " + str(proportion))
        #### A COMPLETER


## Affichage de toutes les données des poutres
def displayAll():
    print("# --------------- POUTRES --------------- ")
    for ind_poutre in range(nb_poutres):
        Poutres[ind_poutre].display()

    print("# --------------- COTES --------------- ")
    for ind_cote in range(len(ind_cotes)):
        Cotes[ind_cote].display()

    print("# --------------- FORCES --------------- ")
    for ind_force in range(len(ind_forces)):
        Forces[ind_force].display()

    print("# --------------- LIAISONS --------------- ")
    for ind_liaison in range(len(ind_liaisons)):
        Liaisons[ind_liaison].display()


displayAll()

## Visualisation de tous les patchs
def visualisePatch(dataPatch):
    xmin, xmax, ymin, ymax = dataPatch[:4]
    plt.plot([ymin, ymin], [xmin, xmax], 'r')
    plt.plot([ymax, ymax], [xmin, xmax], 'r')
    plt.plot([ymin, ymax], [xmin, xmin], 'r')
    plt.plot([ymin, ymax], [xmax, xmax], 'r')
    plt.show()

plt.imshow(I, cmap='gray')
for i in range(len(data)):
    visualisePatch(data[i])

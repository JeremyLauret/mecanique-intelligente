import numpy as np
from constantes import *
from data_example import *

## FONCTION POUR LA DETERMINATION DE PROPORTION
def proportion(x):
    """ Pour un réel 0<= x <= 1, on détermine son arrondi en 1/5, 1/4, 1/3 ou 1/2
        Exemple : proportion sur une poutre du point d'application d'une force"""
    dist0 = abs(x)
    dist1 = abs(1-x)

    distMin = dist0
    proportionOptimale = 0
    if (dist1 < dist0):
        distMin = dist1
        proportionOptimale = 1

    for N in range(2,NB_PAS_POUTRES+1):
        for k in range(1, N):
            distInterm = abs(x-k/N)
            if distInterm < distMin :
                distMin = distInterm
                proportionOptimale = k/N

    return proportionOptimale

##FONCTIONS GENERALES POUR LA PROXIMITE
def dist(p1,p2):
    '''norme euclidienne entre deux points du plan'''
    x1,y1=p1
    x2,y2=p2
    return(np.sqrt((x1-x2)**2 + (y1-y2)**2))

def rect_distance(fenetre1, fenetre2):
    '''distance entre deux rectangles dont les côtés sont selon les mêmes axes (pas de rotation)'''
    xmin_1,xmax_1,ymin_1,ymax_1 = fenetre1
    xmin_2,xmax_2,ymin_2,ymax_2 = fenetre2
    
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





## Attribution d'une force à une poutre
def attribueForce(force, Poutres):
    # Origine et vecteur directeur de la force à attribuer
    origF = np.array(force.getOrigin())
    vdF = np.array(force.getVectDir())
    normF = np.linalg.norm(vdF)

    # pour chaque poutre
    for poutre in Poutres:
        # Origine, vecteur directeur et norme de la poutre
        origP = np.array(poutre.getOrigin())
        vdP = np.array(poutre.getVectDir())
        normP = np.linalg.norm(vdP)

        #Projection du vecteur force selon le vecteur directeur unitaire de la poutre
        pi_F = np.dot(vdF, vdP)/(normP**2) * vdP

        # Equation de la poutre de vecteur directeur (a,b) : -b*x + a*y + c = 0
        # c de l'équation
        c = vdP[1] * origP[0] - vdP[0] * origP[1]

        # L = distance origine de la force à la poutre
        L = abs(-vdP[1] * origF[0] + vdP[0] * origF[1] + c) / normP
        # x0 = abscisse curviligne du projeté de la force sur la poutre
        x0 = np.dot(origF-origP, vdP) / normP

        X0 = x0 * vdP / normP + origP
        
        # F - pi_F
        diff = vdF - pi_F
        X = X0 + L/np.linalg.norm(diff) * pi_F  # X = point d'application de la force sur la poutre
        
        # Calcul de l'abscisse curviligne du point d'application de la force sur la poutre (valeur comprise entre 0 et 1 si la force s'applique bien à la poutre)
        normeX = np.linalg.norm(X - origP)
        prop = normeX / normP
        
        if (np.dot(vdF, vdP) != 0):
            prop *= np.sign(np.dot(vdF, vdP))
        else :
            prop *= np.sign(np.dot(vdP, vdF+origF-origP))

        if (prop >= 0 and prop <= 1):
            propOptim = proportion(prop)
            poutre.setForce(force, propOptim)






## Attribution d'une cote à une poutre
def attribueCote(cote, Poutres):
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
            prop = proportion(normC / normP)
            poutre.setCote(cote.length, prop)

        elif ((prodScalaire < 1-epsScal) and
            (   ((diffX0 <= alpha * poutre.distance) and (diffXf <= alpha * poutre.distance)) or
                ((diffY0 <= alpha * poutre.distance) and (diffYf <= alpha * poutre.distance)) ) ) : # Cas de poutre non parallèle à la cote, mais la cote est exactement alignée avec le début et la fin de la poutre (ex: poutre transverse)
            # même code que dans le if précédent
            prop = proportion(normC / normP)
            poutre.setCote(cote.length, prop)






##FONCTION POUR ASSOCIER UN CARACTERE A UN patch
def attribueCaractere(id_car, Cotes, Forces):
    '''attribue le caractère dont l'index dans data est id_car à l'élément sémantique le plus proche'''
    [xmin, xmax, ymin, ymax, label] = data[id_car]
    caractere = LABELS_INV[label] #Caractère à attribuer
    d_min = pow(10,8)#on initialise la distance minimum
    id_min = -1#on initialise l'index du patch le plus proche du caractère dans data
    label_min = -1
    
    indices = ind_cotes+ind_forces
    donnees = data[indices]
    
    for k in range(len(donnees)):
        [xmin2, xmax2, ymin2, ymax2, label2] = donnees[k]
        dist = rect_distance((xmin,xmax,ymin,ymax), (xmin2,xmax2,ymin2,ymax2))
        if(dist<d_min):
            id_min = indices[k]
            d_min = dist
            label_min = label2
    # print('Caractère {} avec index {} dans data attribué à élément {} index {}'.format(caractere,id_car,LABELS_INV[label_min],id_min))
    if (LABELS_INV[label_min] == 'cote_longueur' or LABELS_INV[label_min] == 'cote_rayon_courbe'):
        for cote in Cotes:
            if cote.getId() == id_min:
                cote.setCaractere(caractere)
    else:
        for force in Forces:
            if force.getId() == id_min:
                force.setCaractere(caractere)


## Connexion des poutres entre elles
def connectePoutre(poutre, Poutres):
    if len(Poutres) == 1:
        return []
    listePoutresConnectees = []
    extrem1 = np.array(poutre.getOrigin())
    extrem2 = np.array(poutre.getOrigin()) + np.array(poutre.getVectDir())
    
    sensibilite = np.minimum(I.shape[0], I.shape[1]) * 2/100
    
    for beam in Poutres:
        if (beam.getId() != poutre.getId()):
            extremB1 = np.array(beam.getOrigin())
            extremB2 = extremB1 + np.array(beam.getVectDir())
            
            if (dist(extrem1, extremB1) <= sensibilite or
                dist(extrem1, extremB2) <= sensibilite or
                dist(extrem2, extremB1) <= sensibilite or
                dist(extrem2, extremB2) <= sensibilite):
                    listePoutresConnectees.append(beam.getId())
    
    for beam in listePoutresConnectees:
        poutre.setConnexionPoutre(beam)


## Connexion des liaisons aux poutres
def connecteLiaison(liaison, Poutres):
    #Connexion des liaisons rotule
    center = np.array(liaison.getCenter())
    radius = liaison.getRadius()
    radiusLarge = 1.2*radius
    
    for poutre in Poutres:
        extrem1 = np.array(poutre.getOrigin())
        extrem2 = extrem1 + np.array(poutre.getVectDir())
        if (dist(extrem1, center) <= radiusLarge or dist(extrem2, center) <= radiusLarge):
            poutre.setLiaison(liaison.getId())
            liaison.setPoutre(poutre.getId())
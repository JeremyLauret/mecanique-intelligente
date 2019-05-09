import numpy as np
import matplotlib.pyplot as plt

## Import des autres modules
# Import du dictionnaire de labels (constantes)
from labels import *
# Import des constantes du problème
from constantes import *
# Import de l'example de donnée
from data_example import *
# Import des fonctions de visualisation
from visualisation import *

# Import des fonctions pour les attributions de patchs à d'autres patchs
from attributions import *

# Import de la classe "Poutres"
from Poutres import *
# Import de la classe "Cotes"
from Cotes import *
# Import de la classe "Forces"
from Forces import *
# Import de la classe "Liaisons"
from Liaisons import *



""" Variable globale : data = [[xmin, xmax, ymin, ymax, label], ...] sorted by label numbers"""
""" L'image I, sur laquelle on travaille, est définie dans data_example, tout comme le tableau data"""

##### ATTENTION : C++ NE CONSIDERE PAS LES MEMES AXES POUR LES IMAGES QUE PYTHON (LES AXES X ET Y SONT PROBABLEMENT INVERSES DONC IL FAUDRA PRENDRE CA EN COMPTE !!!)


# 1ere étape : Vecteur directeur + origine + distance
# 2eme étape : Longueurs
# 3eme étape : Liaisons
# 4eme étape : Forces
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




## Affectation des éléments entre eux
# Application de attribueCaractere() : Affectation de chaque caractère à un élément de la structure
for ind_caractere in ind_caracteres:
    attribueCaractere(ind_caractere, Cotes, Forces)


# Application de attribueCote()
for cote in Cotes:
    attribueCote(cote, Poutres)

# Application de attribueForce()
for force in Forces:
    attribueForce(force, Poutres)

# Connexion des poutres entre elles
for poutre in Poutres:
    connectePoutre(poutre, Poutres)

# Connexion des liaisons sur les poutres
for liaison in Liaisons:
    connecteLiaison(liaison, Poutres)


## Demande à l'utilisateur d'entrer la valeur de ses constantes
for caractere in caracteres:
    val = input("Quelle est la valeur de " + caractere + " ? La valeur doit être exprimée dans les dimensions du système international (m, N). \n")
    inputValeurSchema(caractere, val)
# Les valeurs sont stockées dans le dictionnaire VALEURS


## Affichage de toutes les données des poutres
def dispAll():
    displayAll(Poutres, nb_poutres, Cotes, ind_cotes, Forces, ind_forces, Liaisons, ind_liaisons)

# dispAll()

## Visualisation de tous les patchs
plt.imshow(I, cmap='gray')
for i in range(len(data)):
    visualisePatch(data[i])

import numpy as np
import matplotlib.pyplot as plt

from labels import *

# Image que l'on traite
I = plt.imread("schema_crop.jpg")

# Données issues du post_processing
data = np.array([
    [119, 43, 127, 245, LABELS["poutre_droite"]],
    [52, 366, 54, 499, LABELS["poutre_droite"]],
    [57, 245, 123, 358, LABELS["poutre_droite"]],
    [122, 7, 172, 69, LABELS["liaison_appui_simple"]],
    [122, 213, 168, 271, LABELS["liaison_glissiere"]],
    [33, 498, 72, 514, LABELS["liaison_encastrement"]],
    [48, 356, 60, 369, LABELS["liaison_rotule"]],
    [45, 240, 111, 353, LABELS["cote_longueur"]],
    [189, 21, 210, 247, LABELS["cote_longueur"]],
    [187, 361, 209, 511, LABELS["cote_longueur"]],
    [47, 110, 114, 134, LABELS["force_ponctuelle"]],
    [62, 283, 73, 295, LABELS["a"]],
    [204, 112, 232, 129, LABELS["l"]],
    [209, 419, 221, 438, LABELS["m"]],
    [60, 133, 81, 150, LABELS["f"]]
])  # On peut rajouter l'angle [103, 279, 120, 303, LABELS["o"]]
col0 = np.copy(data[:,1])
data[:,1]=data[:,2]
data[:,2]=col0      # On a bien data = [xmin, xmax, ymin, ymax, label]


#Nombre total de poutres dans l'image
nb_poutres = np.sum(np.where(data[:,4]==LABELS["poutre_droite"], 1, 0))

## Liste des indices dans data
# Indices des cotes dans data
ind_cotes = []
# Indices des forces dans data
ind_forces = []
# Indices des liaisons dans data
ind_liaisons = []
# Indices des caracteres
ind_caracteres = []
caracteres = []

for i in range(nb_poutres, len(data)):
    if (data[i,-1] == LABELS["cote_longueur"] or
        data[i,-1] == LABELS["cote_rayon_courbe"]):
        ind_cotes.append(i)
    elif (data[i,-1] == LABELS["force_ponctuelle"] or
         data[i,-1] == LABELS["force_homogene"] or
         data[i,-1] == LABELS["force_autre"]):
        ind_forces.append(i)
    elif (data[i,-1] == LABELS["liaison_appui_simple"] or 
         data[i,-1] == LABELS["liaison_double_glissiere"] or 
         data[i,-1] == LABELS["liaison_encastrement"] or 
         data[i,-1] == LABELS["liaison_rotule"] or
         data[i,-1] == LABELS["liaison_glissiere"] or
         data[i,-1] == LABELS["liaison_pivot"]):
        ind_liaisons.append(i)
    elif (data[i,-1] <= 25):
        ind_caracteres.append(i)
        caracteres.append(LABELS_INV[data[i,-1]])
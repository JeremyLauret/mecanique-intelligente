import numpy as np
import matplotlib.pyplot as plt

from labels import *

# Image que l'on traite
I = plt.imread("schema_crop.jpg")

# Données issues du post_processing
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

## Liste des indices dans data
# Indices des cotes dans data
ind_cotes = []
# Indices des forces dans data
ind_forces = []
# Indices des liaisons dans data
ind_liaisons = []
# Indices des caracteres
ind_caracteres = []

for i in range(nb_poutres, len(data)):
    if data[i,-1] == LABELS["cote"]:
        ind_cotes.append(i)
    elif (data[i,-1] == LABELS["force_ponctuelle"] or
         data[i,-1] == LABELS["force_distribuee"]):
        ind_forces.append(i)
    elif (data[i,-1] == LABELS["appui_simple"] or 
         data[i,-1] == LABELS["appui_glissiere"] or 
         data[i,-1] == LABELS["encastrement"] or 
         data[i,-1] == LABELS["rotule"]):
        ind_liaisons.append(i)
    elif (data[i,-1] >= 18 and data[i,-1] <= 35):
        ind_caracteres.append(i)
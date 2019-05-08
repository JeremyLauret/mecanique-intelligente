import matplotlib.pyplot as plt
from data_example import *

## Affichage de toutes les données des poutres
def displayAll(Poutres, nb_poutres, Cotes, ind_cotes, Forces, ind_forces, Liaisons, ind_liaisons):
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


## Visualisation de tous les patchs
def visualisePatch(dataPatch):
    xmin, xmax, ymin, ymax = dataPatch[:4]
    plt.plot([ymin, ymin], [xmin, xmax], 'r')
    plt.plot([ymax, ymax], [xmin, xmax], 'r')
    plt.plot([ymin, ymax], [xmin, xmin], 'r')
    plt.plot([ymin, ymax], [xmax, xmax], 'r')
    plt.show()

# Visualisation d'un seul patch
def visualiseSinglePatch(ind_element):
    plt.figure()
    plt.imshow(I, cmap='gray')
    xmin, xmax, ymin, ymax = data[ind_element, :4]
    plt.plot([ymin, ymin], [xmin, xmax], 'g')
    plt.plot([ymax, ymax], [xmin, xmax], 'g')
    plt.plot([ymin, ymax], [xmin, xmin], 'g')
    plt.plot([ymin, ymax], [xmax, xmax], 'g')
    plt.show()
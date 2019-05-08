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
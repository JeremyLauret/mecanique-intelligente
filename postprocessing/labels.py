# Dictionnaire des labels : en ayant le nom d'un élément, on retrouve son label (int)
# LABELS = {"poutre": 1, "appui_simple":6, "appui_glissiere":7, "encastrement":8, "rotule":9, "cote":15, "force_ponctuelle":16, "force_distribuee":17, "a":18, "l":26, "m":27, "F":35, "theta":57}

LABELS = {
    "a":0,
    "b":1,
    "c":2,
    "d":3,
    "e":4,
    "f":5,
    "g":6,
    "h":7,
    "i":8,
    "j":9,
    "k":10,
    "l":11,
    "m":12,
    "n":13,
    "o":14,
    "p":15,
    "q":16,
    "r":17,
    "s":18,
    "t":19,
    "u":20,
    "v":21,
    "w":22,
    "x":23,
    "y":24,
    "z":25,
    "cote_longueur":26,
    "cote_rayon_courbe":27,
    "force_homogene":28,
    "force_autre":29,
    "force_ponctuelle":30,
    "poutre_droite":31,
    "poutre_circulaire":32,
    "liaison_appui_simple":33,
    "liaison_double_glissiere":34,
    "liaison_rotule":35,
    "liaison_glissiere":36,
    "liaison_encastrement":37,
    "liaison_pivot":38,
    "moments_homogenes":39,
    "moments_autres":40,
    "moments_ponctuels":41,
    "pointilles_alignes":42,
    "vecteurs_directeurs":43
}

# Calcul du nombre de labels
nb_labels = len(LABELS)

# Création d'un dictionnaire inversé : à partir du label on retrouve le nom de l'élément en LABELS_INV[label]
LABELS_INV = [""]*nb_labels
for label in LABELS:
    LABELS_INV[LABELS[label]] = label
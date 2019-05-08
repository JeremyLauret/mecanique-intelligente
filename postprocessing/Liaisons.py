import numpy as np
from labels import *
from data_example import *

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
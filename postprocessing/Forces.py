from labels import *
from data_example import *
from attributions import *

class Force:
    def __init__(self, identifiant):
        self.id = identifiant

        self.type = LABELS_INV[data[identifiant,4]]
        
        self.norme = ""

        xmin, xmax, ymin, ymax = data[identifiant,:4]
        self.origine, self.vecteurDirecteur = origineVecteurDirecteurForce(I, xmin, xmax, ymin, ymax)

        self.angle = angleAxeX(self.vecteurDirecteur)   # Angle en degrés, par rapport à l'axe X

    def display(self):
        print("Force n°" + str(self.id))
        print("Force de type " + self.type +", et de norme : " + str(self.norme))
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
    
    def setCaractere(self, caractere):
        self.norme = caractere
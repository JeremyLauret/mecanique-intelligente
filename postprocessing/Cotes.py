from data_example import *
from attributions import *

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
        
    def setCaractere(self, caractere):
        self.length = caractere
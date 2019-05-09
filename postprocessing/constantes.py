from data_example import *

## Constantes liées à la discrétisation du problème et aux proportions
NB_PAS_POUTRES = 5   # On discrétise les poutres en NB_PAS_POUTRES afin de définir ensuite les proportions des cotes par rapport à cette discrétisation (ex : une cote vaut 3/5 de la poutre associée et non 0.57385219)

## Valeur des lettres associées aux cotes ou normes de forces
VALEURS = {}

def inputValeurSchema(lettre, valeur):
    VALEURS.update({lettre: valeur})
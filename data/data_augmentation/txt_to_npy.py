import numpy as np
import os

# Charger les données depuis le fichier texte
for i in range(83):
    filename = "/home/julie/Documents/2A/PI/ProjetIngenieur/data/data_augmentation/data_original/DATA{}.txt".format(i)
    if os.path.exists(filename):
        data = np.loadtxt(filename, delimiter=';')

        # Créer un tableau numpy à partir des données
        array = np.array(data)

        # Sauvegarder le tableau numpy dans un fichier .npy
        np.save("/home/julie/Documents/2A/PI/ProjetIngenieur/data/data_augmentation/data_npy/DATA{}.npy".format(i), array)
    else:
        print("Le fichier {} n'existe pas.".format(filename))

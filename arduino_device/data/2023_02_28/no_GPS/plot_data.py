#====================================================
# Ingineering Project | TPS x Alcatel Lucent | 2023 |
#====================================================

import os
os.chdir(os.path.dirname(os.path.abspath(__file__)))

import pandas as pd
import matplotlib.pyplot as plt

nb_files = 6
data = []

for i in range(nb_files):
    data = pd.read_csv("DATA" + str(i+1) + ".TXT", sep=";", header=None, names=["ax", "ay", "az"])
    nb_measure = data.shape[0]

    fig, axs = plt.subplots(1,3)
    fig.set_tight_layout(True)
    fig.set_size_inches(12, 4)

    axs[0].plot(range(nb_measure), data["ax"])
    axs[0].set_title("Acceleration X")
    axs[1].plot(range(nb_measure), data["ay"])
    axs[1].set_title("Acceleration Y")
    axs[2].plot(range(nb_measure), data["az"])
    axs[2].set_title("Acceleration Z")

    fig.savefig("DATA" + str(i+1))

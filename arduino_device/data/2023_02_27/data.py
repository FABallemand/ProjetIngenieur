#====================================================
# Ingineering Project | TPS x Alcatel Lucent | 2023 |
#====================================================

import os
os.chdir(os.path.dirname(os.path.abspath(__file__)))

import pandas as pd
import matplotlib.pyplot as plt

nb_files = 4
data = []

for i in range(nb_files):
    data.append(pd.read_csv("DATA" + str(i+1) + ".TXT"))
    print(data[i])
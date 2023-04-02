#====================================================
# Ingineering Project | TPS x Alcatel Lucent | 2023 |
#====================================================

import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import gmplot
import gpxpy
import gpxpy.gpx

os.chdir(os.path.dirname(os.path.abspath(__file__)))

nb_folders = 6

for i in range(1, nb_folders+1):
    # Load Data ===============================================================
    # Locate files
    path = "path_" + str(i)
    acc_file = None
    gps_file = None
    for f in os.listdir(path):
        if f.endswith(".csv"):
            acc_file = f
        if f.endswith(".gpx"):
            gps_file = f

    # Acceleration data
    acc_data = pd.read_csv(path + "/" + acc_file, sep=";", header=1, names=["time", "ax", "ay", "az", "norm"])

    # GPS data
    gpx_file = open(path + "/" + gps_file, "r")
    gpx = gpxpy.parse(gpx_file)
    time = []
    lat = []
    long = []
    alt = []
    for track in gpx.tracks:
        for segment in track.segments:
            for point in segment.points:
                time.append(point.time)
                lat.append(point.latitude)
                long.append(point.longitude)
                alt.append(point.elevation)
    gps_data = pd.DataFrame(list(zip(time, lat, long, alt)), columns= ["time", "lat", "long", "alt"])

    # Plot Acceleration Data ==================================================
    nb_measure = acc_data.shape[0]
    fig, axs = plt.subplots(1,3)
    fig.set_tight_layout(True)
    fig.set_size_inches(12, 4)
    axs[0].plot(range(nb_measure), acc_data["ax"])
    axs[0].set_title("Acceleration X")
    axs[1].plot(range(nb_measure), acc_data["ay"])
    axs[1].set_title("Acceleration Y")
    axs[2].plot(range(nb_measure), acc_data["az"])
    axs[2].set_title("Acceleration Z")
    plt.savefig("path_" + str(i) + "/acceleration.png")

    # Plot GPS Data ===========================================================
    min_lat, max_lat = min(gps_data["lat"]), max(gps_data["lat"])
    min_lon, max_lon = min(gps_data["long"]), max(gps_data["long"])
    mymap = gmplot.GoogleMapPlotter(min_lat + (max_lat - min_lat) / 2,
                                    min_lon + (max_lon - min_lon) / 2,
                                    12)
    mymap.plot(gps_data["lat"], gps_data["long"], "red", edge_width=2)
    mymap.draw("path_" + str(i) + "/map.html")
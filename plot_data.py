#====================================================
# Ingineering Project | TPS x Alcatel-Lucent | 2023 |
#====================================================

# https://www.tutorialspoint.com/python/python_command_line_arguments.htm

import os
import sys, getopt
import pandas as pd
import gpxpy, gmplot
import matplotlib.pyplot as plt
import plotly.graph_objects as go
from plotly.subplots import make_subplots

os.chdir(os.path.dirname(os.path.abspath(__file__)))

help_string = "usage: plot_data.py [-h] [-f <file> | -d <directory>]\noptional arguments:\n-h\tshow this help message and exit\n-p, --picture\tcreate PNG graphs of the input acceleration data\n-i, --interactive\tcreate html graphs of the input acceleration data\n-g, --gps\tcreate html map of the input GPS data\nFor more details go to: https://github.com/FABallemand/ProjetIngenieur"

def picture_plot(file, data):
    """
    Plot data acceleration data contained in file in PNG with matplotlib

    Args:
        file (string): Name of the file
        data (Pandas.DataFrame): X, Y and Z acceleration data contained in the file
    """
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

    fig.savefig(file[:-4])
    plt.close()


def interactive_plot(file, data):
    """
    Plot data acceleration data contained in file in HTML with plotly

    Args:
        file (string): Name of the file
        data (Pandas.DataFrame): X, Y and Z acceleration data contained in the file
    """
    col_num = 1
    fig = make_subplots(rows=1, cols=3)
    for col in data.columns:
      fig.add_trace(go.Scatter(x=data.index,y=data[col],name=col), row=1, col=col_num)
      col_num += 1
    # fig.update_layout(height=600, width=800, title_text="Test")
    fig.update_layout(title_text=file)
    fig.write_html(file[:-4] + ".html", full_html=False, include_plotlyjs="cdn")


def main(argv):

    # Arguments parser
    input_file = ""
    input_directory = ""
    picture = 0
    interactive = 0
    gps = 0
    opts, args = getopt.getopt(argv, "hf:d:pig",["file", "directory", "picture", "interactive", "gps"])
    for opt, arg in opts:
        if opt == "-h":
            print(help_string)
            sys.exit()
        elif opt in ("-f", "--file") and input_file != -1:
            input_file = arg
            input_directory == -1
        elif opt in ("-d", "--directory") and input_directory != -1:
            input_directory = arg
            input_file = -1
        elif opt in ("-p", "--picture"):
            picture = 1
        elif opt in ("-i", "--interactive"):
            interactive = 1
        elif opt in ("-g", "--gps"):
            gps = 1

    # Look for files
    acc_files = []
    gps_files = []
    if input_file == "" and input_directory == "":
        print("error: must provide input source\nhelp: plot_data.py -h")
        sys.exit()
    elif input_file != -1:
        acc_files.append(input_file)
    else:
        for f in os.listdir(input_directory):
            if f.endswith(".TXT"):
                acc_files.append(input_directory + "/" + f)
            if f.endswith(".gpx") and gps:
                gps_files.append(input_directory + "/" + f)

    # Acceleration data
    print("# Acceleration Data")
    for f in acc_files:
        print("File: ", f)
        acc_data = pd.read_csv(f, sep=";", header=None, names=["ax", "ay", "az"])
        if picture:
            picture_plot(f, acc_data)
        if interactive:
            interactive_plot(f, acc_data)
    
    # GPS data
    print("# GPS Data")
    if gps:
        print("File: ", f)
        for f in gps_files:
            gpx_file = open(f, "r")
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
            min_lat, max_lat = min(gps_data["lat"]), max(gps_data["lat"])
            min_lon, max_lon = min(gps_data["long"]), max(gps_data["long"])
            mymap = gmplot.GoogleMapPlotter(min_lat + (max_lat - min_lat) / 2,
                                            min_lon + (max_lon - min_lon) / 2,
                                            12)
            mymap.plot(gps_data["lat"], gps_data["long"], "red", edge_width=2)
            mymap.draw(f[:-4] + "_map.html")
        

if __name__ == "__main__":
    main(sys.argv[1:])
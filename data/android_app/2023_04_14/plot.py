import os
import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots

path = os.path.dirname(os.path.abspath(__file__))
os.chdir(path)

def singleGraph(df):
    fig = go.Figure()
    for col in df.columns:
      fig.add_trace(go.Scatter(x=df.index,y=df[col],name=col))
    return fig

def tripleGraph(df):
    col_num = 1
    fig = make_subplots(rows=1, cols=3)
    for col in df.columns:
      fig.add_trace(go.Scatter(x=df.index,y=df[col],name=col), row=1, col=col_num)
      col_num += 1
    fig.update_layout(height=1080/2, width=1920/2, title_text="Test")
    return fig

files = [file for file in os.listdir(path) if os.path.isfile(file)]
files.remove("plot.py")
print(files)

for file in files:
    acc_data = pd.read_csv(file, sep=";", header=1, names=["ay", "ax", "az"])
    fig = singleGraph(acc_data[["ax", "ay", "az"]])
    fig.write_html(file + ".html", full_html=False, include_plotlyjs="cdn")
import csv
import sys
import fileinput
import math
from shutil import copyfile

def getXYZinitialMeanValues(filename):
    meanValues = [0.0,0.0,0.0] # x, y ,z initial value
    meanValuesForLargestWindow = [0.0, 0.0, 0.0] # mean value for larger windowSize
    accuracy = 0.01
    maxWindowSize = 0

    with open(filename) as csv_file:
            csv_reader = csv.DictReader(csv_file, delimiter=';')
            n = 0 #current size of the window where average is computed
            for row in csv_reader:
                if len(row) > 0:
                    #print(f'process new row :{n}')
                    x = float(row["x"])
                    y = float(row["y"])
                    z = float(row["z"])
                    if n == 0:
                        meanValues = [x,y,z]
                        n += 1
                    else:
                        deltax = meanValues[0] - x
                        deltay = meanValues[1] - y
                        deltaz = meanValues[2] - z
                        
                        #if difference between current mean value and next value is lower than accuracy, calculate new mean value
                        if (abs(deltax) < accuracy) and (abs(deltay) < accuracy) and (abs(deltaz) < accuracy):
                            meanValues[0] = (meanValues[0]*n + x) / (n+1)
                            meanValues[1] = (meanValues[1]*n + y) / (n+1)
                            meanValues[2] = (meanValues[2]*n + z) / (n+1)
                            #print(";".join([str(meanValues[0]),str(meanValues[1]),str(meanValues[2])]))
                            n+=1
                            if n > maxWindowSize:
                                maxWindowSize = n
                                #print(f'new windowmax is {maxWindowSize}')
                                i = 0
                                while i < len(meanValues):
                                    meanValuesForLargestWindow[i] = meanValues[i]
                                    i+=1
                                #print(";".join([str(meanValues[0]),str(meanValues[1]),str(meanValues[2])]))
                                #print(";".join([str(meanValuesForLargestWindow[0]),str(meanValuesForLargestWindow[1]),str(meanValuesForLargestWindow[2])]))
                        else:
                            #initialize with new mean value and look for a greater windowsize
                            n=1                           
                            meanValues[0] = x
                            meanValues[1] = y
                            meanValues[2] = z
                            #print(f'Reinitialize means')
                            #print(";".join([str(meanValues[0]),str(meanValues[1]),str(meanValues[2])]))
                            
                            
                        
    print(f'windowmax is {maxWindowSize}')
    print(";".join([str(meanValuesForLargestWindow[0]),str(meanValuesForLargestWindow[1]),str(meanValuesForLargestWindow[2])]))
    return meanValuesForLargestWindow

def calculateSinAlpha(meansval, beta):

    sinAlpha = meansval[1] / math.cos(beta)
    #sinAlpha = -1*meansval[1] / meansval[0]
    print(f'sinAlpha is {sinAlpha}')
    return sinAlpha

def calculateAlpha(sinalpha):

    alpha = math.asin(sinalpha)
    print(f'Alpha is {alpha}')
    return alpha

def calculateSinBeta(meansval):
    sinBeta = -1*meansval[0]
    print(f'sinBeta is {sinBeta}')
    return sinBeta

def calculateBeta(sinbeta):

    beta = math.asin(sinbeta)
    print(f'Beta is {beta}')
    return beta

def calculateCosGamma(meansval, alpha, beta):
    cosGamma = (meansval[1]*meansval[2]*math.tan(alpha))*(math.cos(beta) + math.tan(beta)*math.sin(beta))
    cosGamma = cosGamma -1*meansval[0]*meansval[2]* math.tan(beta)
    cosGamma = (cosGamma/(meansval[0]*meansval[0] + meansval[1]*meansval[1]))
    print(f'cosGamma is {cosGamma}')
    return cosGamma

def convertToExcel(filename) : #replace '.' with ',' in decimal number
    print('inside convertToExcel')
    with fileinput.input(files=(filename), inplace=True, mode='r') as f:
            reader = csv.DictReader(f, delimiter=';')
            print(";".join(reader.fieldnames))  # print back the headers = time, x,y,z
            for row in reader: 
                if len(row) > 0:
                    row["x"] = row["x"].replace('.',',')
                    row["y"] = row["y"].replace('.',',')
                    row["z"] = row["z"].replace('.',',')
                    print(";".join([row["time"],row["x"],row["y"],row["z"]]))

    return

file = sys.argv[1]
finalMeanValues = getXYZinitialMeanValues(file)
print(";".join([str(finalMeanValues[0]),str(finalMeanValues[1]),str(finalMeanValues[2])]))

sinBeta = calculateSinBeta(finalMeanValues)
beta = calculateBeta(sinBeta)

sinAlpha = calculateSinAlpha(finalMeanValues, beta)
alpha = calculateAlpha(sinAlpha)
cosAlpha = math.cos(alpha)

cosBeta = math.cos(beta)
cosGamma = calculateCosGamma(finalMeanValues, alpha, beta)
if cosGamma > 1:
    cosGamma = 1
gamma = math.acos(cosGamma)
sinGamma = math.sin(gamma)


#now normalized the data to car georeference
normfile = "norm" + file
copyfile(file, normfile)

with fileinput.input(files=(normfile), inplace=True, mode='r') as f:
    reader = csv.DictReader(f, delimiter=';')
    print(";".join(reader.fieldnames))  # print back the headers = time, x,y,z
    for row in reader: 
        if len(row) > 0:
            x = float(row["x"])
            y = float(row["y"])
            z = float(row["z"])
            xprim =cosBeta*cosAlpha*x -cosBeta*sinGamma*y + sinBeta*z
            yprim =(cosAlpha*sinGamma + cosGamma*sinBeta*sinAlpha)*x + (cosAlpha*cosGamma-sinAlpha*sinBeta*sinGamma)*y - sinAlpha*cosBeta*z
            zprim =(sinAlpha*sinGamma-sinBeta*cosAlpha*cosGamma)*x + (sinAlpha*cosGamma + sinBeta*cosAlpha*sinGamma)*y + cosAlpha*cosBeta*z
            row["x"] = str(xprim)
            row["y"] = str(yprim)
            row["z"] = str(zprim)
            print(";".join([row["time"],row["x"],row["y"],row["z"]]))

print('Normalized all data')

#convertToExcel(normfile)

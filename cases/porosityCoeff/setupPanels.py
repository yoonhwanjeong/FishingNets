#!/usr/bin/env python3

import math
import subprocess

fvOptionsLoc = "system/fvOptions"
topoSetDictLoc = "system/topoDict"

darcyLoc = "templates/darcy.txt"
darcyTopLoc = "templates/darcyTop.txt"

topoLoc = "templates/topo.txt"
topoTopLoc = "templates/topoTop.txt"
topoBottomLoc = "templates/topoBottom.txt"

class Panel():
    def __init__(self, name, Sn, Cd, e1 = None, e2 = None, AoA = None, projDir = None):
        self.name = name
        assert AoA or (e1 and e2), "Provide either AoA or e1 and e2"
        if AoA:
            self.e1 = (math.sin(math.radians(AoA)), math.cos(math.radians(AoA)), 0)
            self.e2 = (-math.cos(math.radians(AoA)), math.sin(math.radians(AoA)), 0)
        else:
            self.e1 = e1
            self.e2 = e2
        self.e1Str = f" {self.e1[0]:e} {self.e1[1]:e} {self.e1[2]:e} "
        self.e2Str = f" {self.e2[0]:e} {self.e2[1]:e} {self.e2[2]:e} "

        self.Sn = Sn
        self.Cd = Cd

        #If no explicit projection direction is given, the reference area is calculated using the e1 direciton
        if projDir:
            self.projDir = projDir
        else:
            self.projDir = self.e1

    def setupCoeff(self):
        self.area, self.volume = self.getGeometrics()

        self.a = 1.45
        self.b = 0.85
        self.S1 = self.area*self.Sn/2
        self.S2 = self.area*self.Sn/2

        self.calcCs()
        print(f"Setup panel {self.name} with:", f"e1 = {self.e1}", f"e2 = {self.e2}",
        f"projDir = {self.projDir}", f"area = {self.area}", f"volume = {self.volume}", 
        f"C1 = {self.C1}", f"C1 = {self.C2}", f"C1 = {self.C3}")
        return

    def calcCs(self):
        self.C1 = f"{1/self.volume*self.a*self.Cd*(self.S1+self.S2):e}"
        self.C2 = f"{1/self.volume*self.b*self.Cd*(self.S2):e}"
        self.C3 = f"{1/self.volume*self.b*self.Cd*(self.S1):e}"
        return

    def getGeometrics(self):
        process = subprocess.Popen(['geometricCalc', str(self.projDir[0]), str(self.projDir[1]), str(self.projDir[2])],
                            stdout=subprocess.PIPE, 
                            stderr=subprocess.PIPE)
        stdout, stderr = process.communicate()
        print(stdout)
        outstr = str(stdout)
        area = getProcessOutput(outstr, self.name, f"projected area = ")
        volume = getProcessOutput(outstr, self.name, f"cellZoneVol = ")
        return area, volume

def getProcessOutput(stringToSearch, lineStart, searchString):
    lines = stringToSearch.splitlines()
    for line in lines:
        if lineStart in line:
            id = line.index(searchString) + len(searchString)
            number = float(line[id:].split('\\')[0])
    return number

def addPanelTo(fileLoc, templateLoc, panel):
    with open(templateLoc, 'r') as templateFile:
        templateData = templateFile.read()
    
    templateData = templateData.replace('NAME', panel.name)
    templateData = templateData.replace('E1', panel.e1Str)
    templateData = templateData.replace('E2', panel.e2Str)

    if "fvOptions" in fileLoc:
        templateData = templateData.replace('COEFF1', panel.C1)
        templateData = templateData.replace('COEFF2', panel.C2)
        templateData = templateData.replace('COEFF3', panel.C3)

    with open(fileLoc, 'a') as file:
        file.write(templateData)

def overwrite(fileLoc, templateLoc):
    with open(templateLoc, 'r') as templateFile:
        templateData = templateFile.read()
    with open(fileLoc, 'w') as file:
        file.write(templateData)

def addTo(fileLoc, templateLoc):
    with open(templateLoc, 'r') as templateFile:
        templateData = templateFile.read()
    with open(fileLoc, 'a') as file:
        file.write(templateData)

#-------------------------------------------------------------------------------------------------------#

#Add panels and their properties here: name must currently be the same as stl name
panels = [
    Panel("box_1", 0.128, 1.6, e1 = (1,0,0), e2 = (0,1,0)),
]

#Prepare topoSetDict
overwrite(topoSetDictLoc, topoTopLoc)

#Configure topoSetDictLoc for all panels
for panel in panels:
    addPanelTo(topoSetDictLoc, topoLoc, panel)

#finalize topoSetDict
addTo(topoSetDictLoc, topoBottomLoc)

#run topoSetDict
process = subprocess.Popen(['topoSet'],
                            stdout=subprocess.PIPE, 
                            stderr=subprocess.PIPE)
topostdout, topostderr = process.communicate()
print(f"topoOut = {str(topostdout)}")
print(f"topostderr = {str(topostderr)}")

#Prepare fvOptions
overwrite(fvOptionsLoc, darcyTopLoc)

#Configure fvOptions for all panels
for panel in panels:
    panel.setupCoeff()
    addPanelTo(fvOptionsLoc, darcyLoc, panel)
#!/usr/bin/env python3

import math
import subprocess
import os
import csv

fvOptionsLoc = "system/fvOptions"
darcyLoc = "templates/darcy.txt"
darcyTopLoc = "templates/darcyTop.txt"

topoSetDictLoc = "system/topoSetDict"
topoLoc = "templates/topo.txt"
topoTopLoc = "templates/topoTop.txt"
topoBottomLoc = "templates/topoBottom.txt"

geometricCalcDictLoc = "constant/geometricCalcDict"
geometricLoc = "templates/geometric.txt"
geometricTopLoc = "templates/geometricTop.txt"

normalPath = "constant/triSurface/Normals.csv"
stlPath = "constant/triSurface/STL_Files"

class Panel():
	def __init__(self, zoneName, stlName, Sn, Cd, e1 = None, e2 = None, AoA = None, projDir = None):
		self.zoneName = zoneName
		self.stlName = stlName
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
		self.area, self.volume = geometrics.getGeometrics(self.zoneName)

		self.a = 1.45
		self.b = 0.85
		self.S1 = self.area*self.Sn/2
		self.S2 = self.area*self.Sn/2

		self.calcCs()
		print(f"Setup panel {self.zoneName} {self.stlName} with:", f"e1 = {self.e1}", f"e2 = {self.e2}",
		f"projDir = {self.projDir}", f"area = {self.area}", f"volume = {self.volume}", 
		f"C1 = {self.C1}", f"C1 = {self.C2}", f"C1 = {self.C3}")
		return

	def calcCs(self):
		self.C1 = f"{1/self.volume*self.a*self.Cd*(self.S1+self.S2):e}"
		self.C2 = f"{1/self.volume*self.b*self.Cd*(self.S2):e}"
		self.C3 = f"{1/self.volume*self.b*self.Cd*(self.S1):e}"
		return

class Geometrics():
	def __init__(self) -> None:
		self.prepareGeometrics()

	def prepareGeometrics(self):
		#with open(f"geometricCalc.log","wb") as out, open(f"geometricCalc.log","wb") as err:
		#	process = subprocess.Popen(['geometricCalc'],
		#								stdout=out, 
		#								stderr=err)
		process = subprocess.Popen(['geometricCalc'],
							stdout=subprocess.PIPE, 
							stderr=subprocess.PIPE)
		stdout, stderr = process.communicate()
		self.geomStr = str(stdout)
		return

	def getGeometrics(self, zoneName):
		area = self.getProcessOutput(self.geomStr, zoneName, f"projected area = ")
		volume = self.getProcessOutput(self.geomStr, zoneName, f"cellZoneVol = ")
		return area, volume

	def getProcessOutput(self, stringToSearch, lineStart, searchString):
		lines = stringToSearch.splitlines()
		for line in lines:
			if lineStart in line:
				id = line.index(searchString) + len(searchString)
				number = float(line[id:].split('\\')[0])
		return number

def addPanelTo(fileLoc, templateLoc, panel):
	with open(templateLoc, 'r') as templateFile:
		templateData = templateFile.read()
	
	templateData = templateData.replace('ZONENAME', panel.zoneName)
	templateData = templateData.replace('STLNAME', panel.stlName)
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

stlNames = []

for root, dirs, files in os.walk(stlPath):
	for file in files:
		#append the file name to the list
		#stlList.append(os.path.join(root,file))
		stlNames.append(int(file.split(".")[0]))
		
stlNames = sorted(stlNames)

e1s = []
e2s = []
with open(normalPath, newline='') as csvfile:
	csvreader = csv.reader(csvfile, delimiter=',', quotechar='|')

	rows = []
	for row in csvreader:
		rows.append(row)

	for stl in stlNames:
		desiredrow = rows[int(stl) + 1] 
		try:
			e1s.append((float(desiredrow[0]),float(desiredrow[1]),float(desiredrow[2])))
			e2s.append((float(desiredrow[3]),float(desiredrow[4]),float(desiredrow[5])))
		except:
			print(f"Cloud not convert row: {row}")

panels = []
for index, stl in enumerate(stlNames):
#Add panels and their properties here: name must currently be the same as stl name
	zoneName = f"zone{stl}"
	stlName = f"{stl}"
	panels.append(Panel(zoneName, stlName, 0.128, 1.6, e1 = e1s[index], e2 = e2s[index]))


#Prepare topoSetDict
overwrite(topoSetDictLoc, topoTopLoc)

#Configure topoSetDictLoc for all panels
for panel in panels:
	addPanelTo(topoSetDictLoc, topoLoc, panel)

#finalize topoSetDict
addTo(topoSetDictLoc, topoBottomLoc)

topo = False
if topo:
	#run topoSetDict
	with open(f"topoSet.log","wb") as out, open(f"topoSetError.log","wb") as err:
		process = subprocess.Popen(['topoSet'],
										stdout=out, 
										stderr=err)

#Prepare geometricCalcDict
overwrite(geometricCalcDictLoc, geometricTopLoc)

#Configure geometricCalcDictLoc for all panels
for panel in panels:
	addPanelTo(geometricCalcDictLoc, geometricLoc, panel)

geometrics = Geometrics()

#Prepare fvOptions
overwrite(fvOptionsLoc, darcyTopLoc)

#Configure fvOptions for all panels
for panel in panels:
	panel.setupCoeff()
	addPanelTo(fvOptionsLoc, darcyLoc, panel)

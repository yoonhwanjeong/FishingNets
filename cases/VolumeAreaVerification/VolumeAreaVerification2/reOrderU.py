#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

lastTimeUPath = "500"

def importU(outputUfile, resolution):
	fU = open(outputUfile, 'r+')
	dataU = fU.readlines()
	for index, line in enumerate(dataU):
		if "STARTING" in line:
			startLineIndex = index + 2
		
	dataU = dataU[startLineIndex:(resolution[0] * resolution[1] * resolution[2] + startLineIndex)]
	num = 0
	dataList = []
	for i in range(len(dataU)):
		data = dataU[i].strip('\n').split(' ')
		if len(data) == 7: #and str.isdigit(vel[0][0]):
			num += 1
			dataList.append(data)
		else:
			print(data)
	print(num)

	Cx = np.zeros(num)
	Cy = np.zeros(num)
	Cz = np.zeros(num)
	Ux = np.zeros(num)
	Uy = np.zeros(num)
	Uz = np.zeros(num)
	p = np.zeros(num)
	for i in range(num):
		Ux[i] = np.float(dataList[i][0])
		Uy[i] = np.float(dataList[i][1])
		Uz[i] = np.float(dataList[i][2])
		Cx[i] = np.float(dataList[i][3])
		Cy[i] = np.float(dataList[i][4])
		Cz[i] = np.float(dataList[i][5])
		p[i] = np.float(dataList[i][6])
	Cx = Cx.reshape((num, 1)) #Transpose wasn't working for some reason
	Cy = Cy.reshape((num, 1))
	Cz = Cz.reshape((num, 1))
	Ux = Ux.reshape((num, 1))
	Uy = Uy.reshape((num, 1))
	Uz = Uz.reshape((num, 1))
	p[i] = p.reshape((num, 1))

	sortMatrix = np.hstack((Cx, Cy, Cz, Ux, Uy, Uz, p))

	#with open('preSortMatrix.txt', 'w') as f:
	#	for line in np.matrix(presortMatrix):
	#		np.savetxt(f, line)

	sortMatrix = sortMatrix[sortMatrix[:,0].argsort()] # First sort doesn't need to be stable.
	sortMatrix = sortMatrix[sortMatrix[:,1].argsort(kind='mergesort')]
	sortMatrix = sortMatrix[sortMatrix[:,2].argsort(kind='mergesort')]

	#with open('sortedMatrix.txt', 'w') as f:
	#	for line in np.matrix(sortMatrix):
	#		np.savetxt(f, line)

	UxSorted = sortMatrix[:,3]
	UySorted = sortMatrix[:,4]
	UzSorted = sortMatrix[:,5]
	pSorted = sortMatrix[:,6]

	#CxSorted = presortMatrix[:,0]
	#CySorted = sortMatrix[:,1]
	#CzSorted = sortMatrix[:,2]

	#Cx = CxSorted.reshape((resolution[2], resolution[1], resolution[0])).T
	#Cy = CySorted.reshape((resolution[2], resolution[1], resolution[0])).T
	#Cz = CzSorted.reshape((resolution[2], resolution[1], resolution[0])).T
	
	Ux = UxSorted.reshape((resolution[2], resolution[1], resolution[0])).T
	Uy = UySorted.reshape((resolution[2], resolution[1], resolution[0])).T
	Uz = UzSorted.reshape((resolution[2], resolution[1], resolution[0])).T
	p = pSorted.reshape((resolution[2], resolution[1], resolution[0])).T

	# x = np.linspace(bounds[0][0], bounds[0][1], resolution[2])
	# y = np.linspace(bounds[1][0], bounds[1][1], resolution[1])
	# z = np.linspace(bounds[2][0], bounds[2][1], resolution[0])
	#
	# ux = interpolate.RegularGridInterpolator((x, y, z), Ux, 'linear')
	# uy = interpolate.RegularGridInterpolator((x, y, z), Uy, 'linear')
	# uz = interpolate.RegularGridInterpolator((x, y, z), Uz, 'linear')

	# VelX = ux((self.X, self.Y, self.Z))
	# VelY = uy((self.X, self.Y, self.Z))
	# VelZ = uz((self.X, self.Y, self.Z))

	#plt.imshow((VelX**2 + VelY**2 + VelZ**2)**0.5)
	#plt.imshow(Cx[:, 10, :])

	#CoordU = np.dstack((Cx, Cy, Cz, newUx))
	#plt.imshow(CoordU[:,:,70])
	plt.imshow(Ux[:,:,10])
	plt.colorbar(orientation='horizontal')
	plt.show()

	# self.V_inf = np.array([VelX, VelY, VelZ])

importU("outputU.log", [150,100,20])#[150, 100, 20])
blockMesh 1>>blockMesh.log
createPatch -overwrite 1>>createPatch.log
./setupPanels.py 1>>setupPanels.log
simpleFoam 1>>simpleFoam.log
foamToVTK -cellSet box_1
cp ~/OpenFOAM/servaas-v2106/topoTest /mnt/c/Users/Vozze/openFOAM -rv
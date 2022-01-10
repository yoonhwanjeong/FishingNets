. ${WM_PROJECT_DIR:?}/bin/tools/RunFunctions #Source runfunctions

blockMesh 1>>blockMesh.log
changeDictionary 1>>changeDictionary.log
createPatch -overwrite 1>>createPatch.log
runApplication decomposePar
runParallel topoSet
runApplication reconstructParMesh -constant
./setupPanels.py 1>>setupPanels.log #This requires the geometricCalc application
simpleFoam 1>>simpleFoam.log

#cp . /mnt/c/Users/Servaas/openFOAM -rv
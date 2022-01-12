runTopoSet="0" #Decide to run toposet or not

chmod -R +x . #Give all files in this directory execute permission
rm *.log log.*  #Clean all logs

if [[runTopoSet != "0"]]; then
    echo "blockMesh"
    blockMesh 1>blockMesh.log 2>blockMeshError.log
    echo "changeDictionary"
    changeDictionary 1>changeDictionary.log 2>changeDictionaryError.log
    echo "createPatch"
    createPatch -overwrite 1>createPatch.log 2>createPatchError.log
else
    echo "skipped mesh generation"
fi
echo "setupPanels"
python3 ./setupPanels.py $runTopoSet 1>setupPanels.log 2>setupPanelsError.log #This requires the geometricCalc application
echo "simpleFoam"
simpleFoam 1>simpleFoam.log 2>simpleFoamError.log
#cp . /mnt/c/Users/Servaas/openFOAM -rv
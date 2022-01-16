newMesh="1" #Decide to run toposet or not
runParallel="1"  #Decide to run parallel toposet or not

chmod -R +x . #Give all files in this directory execute permission
rm *.log log.* processor* -rfv #Clean all logs
ls | grep -P "[1-9]" | xargs -d"\n" rm -rv #Remove all previous results

sed -i "s/^startFrom	latestTime;*/startFrom	startTime;/" $PWD/system/controlDict #edit controlDict for simpleFoam to run

if [[ $newMesh != "0" ]]; then
	echo "Deleting old mesh"
	rm $PWD/constant/polyMesh -rfv
	echo "Constructing new mesh"
    echo "blockMesh"
    blockMesh 1>blockMesh.log 2>blockMeshError.log
    echo "changeDictionary"
    changeDictionary 1>changeDictionary.log 2>changeDictionaryError.log
    echo "createPatch"
    createPatch -overwrite 1>createPatch.log 2>createPatchError.log
else
    echo "skipped mesh generation"
fi

if [[ $runParallel != "0" ]]; then
    echo "Running topoSet in PARALLEL"
else
    echo "Running topoSet in SERIAL"
fi

echo "setupPanels"
unbuffer python3 ./setupPanels.py $newMesh $runParallel |& tee setupPanels.log #This requires the geometricCalc application to be compiled on your machine
echo "simpleFoam"
unbuffer simpleFoam |& tee simpleFoam.log #Run simpleFoam

sed -i "s/^startFrom	startTime;.*/startFrom	latestTime;/" $PWD/system/controlDict #edit controlDict for outputLog to run

outputU 1>outputU.log

rm processor* -rfv #Clean all logs

baseName=basename $PWD
touch "${baseName}.foam" #Create .foam for paraView

cp $PWD /mnt/c/Users/serva/openFOAM -rv #Copy from my ubuntu machine to windows to use paraView
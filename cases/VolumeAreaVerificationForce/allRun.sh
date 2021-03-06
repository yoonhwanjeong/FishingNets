newShape="0" #Decide to include new STL's from directory
newMesh="1" #Decide to run toposet or not
runParallel="1"  #Decide to run parallel toposet or not

chmod -R +x . #Give all files in this directory execute permission
rm *.log log.* processor* -rfv #Clean all logs
ls | grep -P "[1-9]" | xargs -d"\n" rm -rv #Remove all previous results

sed -i "s/^startFrom	latestTime;*/startFrom	startTime;/" $PWD/system/controlDict #edit controlDict to startTime for OpenFOAM simulation
sed -i "s/^writeFormat	ascii;*/writeFormat	binary;/" $PWD/system/controlDict #set controlDict to binary for OpenFOAM simulation

if [[ $newShape != "0" ]]; then
	cp "/mnt/c/Users/serva/Downloads/Temporary_STL_storage/0/STL_Files" . -rv #Add directory to copy STL's from
else
	echo "continuing with old shape"
fi

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
	echo "renumberMesh"
	renumberMesh -overwrite 1>renumberMesh1.log 2>renumberMesh1.log
else
    echo "skipped mesh generation"
fi

if [[ $runParallel != "0" ]]; then
    echo "Running topoSet in PARALLEL"
else
    echo "Running topoSet in SERIAL"
fi

#In the following lines unbuffer is present to write to terminal and file simultaneously. This can be removed or installed through the linux expect package: apt-get install expect
echo "setupPanels"
unbuffer python3 ./setupPanels.py $newMesh $runParallel |& tee setupPanels.log #This requires the geometricCalc application to be compiled on your machine
echo "renumberMesh"
renumberMesh -overwrite 1>renumberMesh2.log 2>renumberMesh2.log
echo "simpleFoam"
unbuffer simpleFoam |& tee simpleFoam.log #Run simpleFoam

sed -i "s/^startFrom	startTime;.*/startFrom	latestTime;/" $PWD/system/controlDict #edit controlDict to latestTime for OutputU
sed -i "s/^writeFormat	binary;*/writeFormat	ascii;/" $PWD/system/controlDict #edit controlDict to ascii for OutputU

outputU 1>outputU.log

rm processor* -rfv #Clean all multiprocessor results

baseName=basename $PWD
touch "${baseName}.foam" #Create .foam for paraView

cp $PWD /mnt/c/Users/serva/openFOAM -rv #Copy from Ubuntu machine to Windows to use ParaView
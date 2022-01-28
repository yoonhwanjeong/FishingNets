. ${WM_PROJECT_DIR:?}/bin/tools/RunFunctions #Source runfunctions
echo "decomposePar"
runApplication decomposePar
echo "topoSet"
runParallel topoSet
echo "reconstructParMesh"
runApplication reconstructParMesh -constant
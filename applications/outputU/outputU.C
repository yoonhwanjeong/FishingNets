/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2021 AUTHOR,AFFILIATION
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    volumeCalc

Description
    Inspired by: https://www.cfd-online.com/Forums/openfoam-programming-development/69661-how-compute-cellzone-volume.html
    and https://www.cfd-online.com/Forums/openfoam-programming-development/85049-read-properties-per-component-input-file-dictionary.html

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "singlePhaseTransportModel.H"
#include "turbulentTransportModel.H"
#include "simpleControl.H"
#include "fvOptions.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "postProcess.H"

    //argList::validArgs.append("projDirX");
    //argList::validArgs.append("projDirY");
    //argList::validArgs.append("projDirZ");

    #include "addCheckCaseOptions.H"
    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"
    #include "createControl.H"
    #include "createFields.H"


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
    Info<< "STARTING OUTPUT AT Time = " << runTime.timeName() << nl << endl;

    //const volVectorField& U = U;

    forAll(U,Ui)
    {
       Info << U[Ui][0] << " " << U[Ui][1] << " " << U[Ui][2] << " "
       << mesh.C()[Ui][0] << " " << mesh.C()[Ui][1] << " " << mesh.C()[Ui][2] << " "
       << p[Ui] 
        <<  endl;
    }

    /*forAll(mesh.cellCentres(), cellZoneID)
    {  


    }*/

    Info<< nl << "End" << endl;

    return 0;
}


// ************************************************************************* //

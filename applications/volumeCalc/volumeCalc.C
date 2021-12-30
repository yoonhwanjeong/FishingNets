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

    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
    Info << "Calculating zone volumes:" << nl << endl;

    forAll(mesh.cellZones(), cellZoneID)
    {
        const cellZone &zone = mesh.cellZones()[cellZoneID];
        const word &zoneName = zone.name();
        const cellZoneMesh &zoneMesh = zone.zoneMesh();
        const labelList &cellsZone = zoneMesh[cellZoneID];

        scalar cellZoneVol(0);
        forAll(cellsZone, cI)
        {
            cellZoneVol += mesh.V()[cellsZone[cI]];
        }

        Info << "zoneName = " << zoneName << ": cellZoneVol = " << cellZoneVol << "\\" << endl;
    }

    Info<< nl << "End" << endl;

    return 0;
}


// ************************************************************************* //

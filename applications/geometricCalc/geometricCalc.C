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

    argList::validArgs.append("projDirX");
    argList::validArgs.append("projDirY");
    argList::validArgs.append("projDirZ");

    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
    const cellList& cells  = mesh.cells();
    const vectorField& faceAreas = mesh.faceAreas();

    List<bool> outsideFaces(faceAreas.size(), false);

    const scalar x(readScalar(IStringStream(args.args()[1])()));
    const scalar y(readScalar(IStringStream(args.args()[2])()));
    const scalar z(readScalar(IStringStream(args.args()[3])()));

    vector projDir = {x,y,z};
    projDir.normalise();
    Info << projDir << endl;

    Info << "Calculating zone volumes and areas:" << endl;
    Info << "Area projection direction along " << projDir << nl << endl;

    forAll(mesh.cellZones(), cellZoneID)
    {
        //Setup
        const cellZone &zone = mesh.cellZones()[cellZoneID];
        const word &zoneName = zone.name();
        const cellZoneMesh &zoneMesh = zone.zoneMesh();
        const labelList &cellsZone = zoneMesh[cellZoneID];

        scalar cellZoneVol(0);
        forAll(cellsZone, cI)
        {
            cellZoneVol += mesh.V()[cellsZone[cI]];
        }

        outsideFaces = false;

        // mark all faces that are NOT internal to the cellZone:
        forAll(zone, i)
        {
            const cell& c = cells[zone[i]];
            forAll(c, cFaceI)
            {
                const label faceI = c[cFaceI];

                // xor operation
                // internal faces get marked twice, outside faces get marked once
                if (outsideFaces[faceI])
                {
                    outsideFaces[faceI] = false;
                }
                else
                {
                    outsideFaces[faceI] = true;
                }
            }
        }

        // now calculate the area
        scalar zoneOutsideArea = 0;
        scalar zouneOutsideProjectedArea = 0;
        label  zoneOutsideNFaces = 0;

        forAll(outsideFaces, faceI)
        {
            if (outsideFaces[faceI])
            {
                vector faceNormal = mesh.Sf()[faceI]/mesh.magSf()[faceI]; //Normalized
                scalar projFace = faceNormal & projDir; //Inner product 
                zoneOutsideArea += mag(faceAreas[faceI]);
                zouneOutsideProjectedArea += mag(faceAreas[faceI])*projFace;
                zoneOutsideNFaces++;
            }
        }

        zouneOutsideProjectedArea *= 0.5; //Select only the windward side of the projected area

        Info << "zoneName = " << zoneName 
        << ": cellZoneVol = " << cellZoneVol 
        << "\\" 
        << " nFaces = " << zoneOutsideNFaces
        << "\\" 
        << " area = " << zoneOutsideArea
        << "\\" 
        << " projected area = " << zouneOutsideProjectedArea
        << "\\" 
        << endl;
    }

    Info<< nl << "End" << endl;

    return 0;
}


// ************************************************************************* //

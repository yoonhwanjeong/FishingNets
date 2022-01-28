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
    #include "postProcess.H" //This include my throw errors on conventional compilers but wmake should be able to handle it

    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"

    //This creates a dictionary to interface with the geometricCalcDict in the case files that contains the panel normals
    IOdictionary ioDictObj
    (
    IOobject
    (
        "geometricCalcDict", /// The dictionary file
        runTime.constant(), /// Relative path (from case root)
        runTime, /// The Time object
        IOobject::MUST_READ, /// Read for constructor
        IOobject::NO_WRITE /// Foam::Time writeControl
    )
    );

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
    const cellList& cells  = mesh.cells();
    const vectorField& faceAreas = mesh.faceAreas();

    List<bool> outsideFaces(faceAreas.size(), false);

    Info << "Calculating zone volumes and areas:" << endl;

    //loop over all cellzones (thus the porous zones)
    forAll(mesh.cellZones(), cellZoneID)
    {
        //Setup
        const cellZone &zone = mesh.cellZones()[cellZoneID];
        const word &zoneName = zone.name();
        const cellZoneMesh &zoneMesh = zone.zoneMesh();
        const labelList &cellsZone = zoneMesh[cellZoneID];

        //Read geometricCalcDict and get the projection direction for the current porous zone
        vector projDir (vector(ioDictObj.subDict(zoneName).lookup("e1")));
        projDir.normalise();

        //Calculate the volume of the zone by summing the volumes of all cells
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

        //Calculate the area by summing the areas of external faces multiplied with the dots between the panel normal and the face normal
        //A_proj = 1/2*SUM(A_i*DOT(FaceNormal_i, ProjectionDirection))
        scalar zoneOutsideArea = 0;
        scalar zouneOutsideProjectedArea = 0;
        label  zoneOutsideNFaces = 0;

        forAll(outsideFaces, faceI)
        {
            if (outsideFaces[faceI]) //If face is on the outside surface
            {
                vector faceNormal = mesh.Sf()[faceI]/mesh.magSf()[faceI]; //Normalized normal of the face
                scalar projFace = mag(faceNormal & projDir); //Inner product
                zoneOutsideArea += mag(faceAreas[faceI]);
                zouneOutsideProjectedArea += mag(faceAreas[faceI])*projFace;
                zoneOutsideNFaces++; //count how many external faces there are for info
            }
        }

        zouneOutsideProjectedArea *= 0.5; //Select only the windward side of the projected area by multiplying entire projected area by 2

        //Print results to console
        Info << "zoneName = " << zoneName 
        << ": cellZoneVol = " << cellZoneVol 
        << "\\" 
        << " nFaces = " << zoneOutsideNFaces
        << "\\" 
        << " area = " << zoneOutsideArea
        << "\\" 
        << " projected area = " << zouneOutsideProjectedArea
        << "\\" 
        << " proj Dir = " << projDir
        << "\\" 
        << endl;
    }

    Info<< nl << "End" << endl;

    return 0;
}


// ************************************************************************* //

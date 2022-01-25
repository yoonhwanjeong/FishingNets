# FishingNets

TU Delft TW3725TU Final Minor Project of group 10 on the simulation of fishing nets.

![Heading image](https://drive.google.com/uc?export=view&id=1lxpYfWofaIt7EmDJwkr9ZyjKfm6ogy9h)

## Features

- Python fishing net deformation simulation:
  - Using custom force model
  - Using pressure fields from OpenFOAM
- OpenFOAM porous simulation of flow around fishing nets
- Coupled simulation of fishing nets using OpenFOAM and Fishing net deformation
- C++ and Python Cartesian mesher for OpenFOAM 

## Contents

### Python fishing net deformation simulation

### OpenFOAM porous simulation

The simulation of the flow around the fishing net is performed by discretizing the fishing net into multiple net elements which are represented in the flow using porous media. The repository contains simulations of the flow around a single net element and of the flow around a complete net geometry.

#### Simulation of the flow around and force on fishing net elements


#### Simulation of the flow around fishing nets of arbitrary complexity

The folders [applications](applications) and [cases](cases) contain relevant files and further documentation: 
[applications](applications) contains the newly written OpenFOAM applications required to run the OpenFOAM [cases](cases).

## Resources used

[OpenFOAM User Guide](https://cfd.direct/openfoam/user-guide/)

[C++ Source Code Guide](https://cpp.openfoam.org/v9/)

[Coding Style Guide](https://openfoam.org/dev/coding-style-guide/)

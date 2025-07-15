# Background Study

This background study is part of a research project aimed at assessing the impact on background counts when utilizing different drift chambers and gas mixtures.

## Installation

Once REST-for-Physics is installed, there is no need to install any additional software. Simply download the contents of this GitHub repository, and it is ready for use.

## Use

To run the simulation, simply use `launchSim.sh`, where the user can choose between a 3 cm or 6 cm drift chamber, select the gas mixture (which must be defined in `AllMaterials.xml`), and specify the name of the output file.

For simulations involving different geometries, the geometry files can be modified accordingly. Additionally, the `cry_generator.rml` file should be adjusted to set the desired particle types.

The file `cosmics.root` contains the spectra of the different cosmic particle fluxes.

Once the simulations are completed, the results can be analyzed by executing `Neutron_Gamma_Counts.C` within restRoot. This script provides:

  -The number of simulated events required to reach the desired number of detections in the Micromegas detector
  
  -The number of detected neutrons and gammas
  
  -The distribution of particles across different energy ranges


  ## Example of use

To run a simulation, open `launchSim.sh` and edit it with the desired parameters. Example:

```bash

#!/bin/bash

export GEOMETRY=setup_6cm.gdml 

export GAS_VOLUME=Argon1%Isobutane1.2bar 

export NAME=Sim1.root

restG4 cry_generator.rml -j 4 -e 10

```
Then, execute the script.

```bash

./launchSim.sh

```


Explanation of parameters:

`GEOMETRY` specifies the geometry file (e.g., 6 cm drift chamber)

`GAS_VOLUME` defines the gas mixture used (must match an entry in `AllMaterials.xml`)

`NAME` is the name of the output .root file

`-j 4` tells Geant4 to use 4 CPU cores

`-e 10` sets the number of detected events in the Micromegas to 10 (not the number of generated particles)

---------------------------------------------------------

Once the simulation has finished, it can be analyzed executing `Neutron_Gamma_Counts.C` within restRoot. Example:

```bash

restRoot

```

```bash

.x Neutron_Gamma_Counts("Sim1.root")

```
  


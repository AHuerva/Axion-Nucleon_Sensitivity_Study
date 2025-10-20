# Efficiency Study

This efficiency study is part of a research project focused on evaluating the detector's performance using different drift chambers and gas mixtures.

## Installation

Once REST-for-Physics is installed, there is no need to install any additional software. Simply download the contents of this GitHub repository, and it is ready for use.


## Intrinsic Efficiency

The aim of these simulations is to determine the detector's intrinsic efficiency in the 0–16 keV energy range, with 0.25 keV intervals. To run these simulations, execute `launchSim2.sh`. This will generate several output files, which can be analyzed using `getEfficiency.C` located in the `simFiles_Henke` directory. The result will be a file containing the calculated efficiency for each energy.


  ### Example of use

To run a simulation, open `launchSim2.sh` and edit it with the desired parameters. Example:

```bash

#!/bin/bash
#set -x
for N in $(seq 0.25 0.25 16.0)
do
  export GEOMETRY=Chamber_6cm.gdml
  export GAS_VOLUME=XenonNeon2.3%Isobutane2.0bar 
  export PRESSURE=2.0
  export NAME=Henke
  export POSITION=33.5
  export ENERGY="$N"

  restG4 Isotopes_Xe_fluxes.rml -e 10000
done


```
Then, execute the script.

```bash

./launchSim2.sh

```


Explanation of parameters:

The line `for N in $(seq 0.25 0.25 16.0)` defines a loop that starts at 0.25 keV and increments by 0.25 keV, ending at 16 keV.

`GEOMETRY` specifies the geometry file (e.g., 6 cm drift chamber)

`GAS_VOLUME` defines the gas mixture used (must match an entry in `Chamber_6cm.gdml`)

`PRESSURE` must match the pressure in `GAS_VOLUME`

Setting `NAME` = Henke directs the output files to the corresponding directory.

`POSITION` is optional to change, but `ENERGY` must remain fixed.

`-e 10000` sets the number of detected events in the Micromegas to 10000.

`Isotopes_Xe_fluxes.rml` corresponds to the detector filled with the xenon mixture. To use the argon mixture instead, it should be replaced with `Isotopes_Ar_fluxes.rml`

---------------------------------------------------------

Once the simulations have finished, they can be analyzed by executing `getEfficiency.C` within restRoot. Example:

```bash

restRoot

```

```bash

.x getEfficiency.C("RestG4_run*_2.0bar_Henke_XeNeISO_6cmDrift_v2.4.3")

```

This command will analyze all files whose names match the given pattern, where * acts as a wildcard.

## Nominal fluxes

The purpose of this study is to determine the detector's efficiency to different axion spectra: axion-photon, axion-electron and axion-nucleon, which can be found in the `Histograms` directory.

### Example of use

## Analyzed fluxes

### Example of use

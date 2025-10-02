#!/bin/bash
#set -x
for N in $(seq 0.25 0.25 16.0) # start increment end
do
  export GEOMETRY=Chamber_6cm.gdml
  export GAS_VOLUME=XenonNeon2.3%Isobutane2.0bar # This is in the gdml file  Argon1%Isobutane2.0bar | XenonNeon2.3%Isobutane1.05bar
  export PRESSURE=2.0
  export NAME=Henke
  export POSITION=33.5
  export ENERGY="$N"
  ENERGY=$(echo "$ENERGY" | sed 's/,/./g')
  echo "$ENERGY"
  echo "GAS_VOLUME=$GAS_VOLUME"
  echo "PRESSURE=$PRESSURE"

  restG4 Isotopes_Xe_fluxes.rml -e 10000
done

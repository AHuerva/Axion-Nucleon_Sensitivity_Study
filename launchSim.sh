#!/bin/bash

# Simulation 1

export GEOMETRY=setup_6cm.gdml # setup_6cm.gdml setup_3cm.gdml
export GAS_VOLUME=Argon1%Isobutane1.2bar # XenonNeon2.3%Isobutane1.05bar Argon1%Isobutane1.2bar
export NAME=Sim1988.79

restG4 cry_generator.rml -j 1 -e 3



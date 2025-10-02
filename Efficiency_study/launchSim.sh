export NAME=aN #aN | Primakoff | ABC
export RANGE="(0,16)"
export POSITION=33.5   #for L=l: (0,0,17.5)mm for L = 2l: (0, 0, 33.5)mm
export GEOMETRY=Chamber_3cm.gdml # Chamber_3cm.gdml Chamber_6cm.gdml
export GAS_VOLUME=XenonNeon2.3%Isobutane2.0bar # This is in the gdml file  Argon1%Isobutane2.0bar | XenonNeon2.3%Isobutane1.05bar
export PRESSURE=2.0

restG4 -j 4 Isotopes_Xe_fluxes.rml -e 200000 # Isotopes_Ar_fluxes.rml


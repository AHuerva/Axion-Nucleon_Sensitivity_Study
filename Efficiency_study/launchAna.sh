port#!/bin/bash

# This script sets up an environment variable and then launches restManager for a range of files.


export NAME=aN #aN Primakoff ABC
export PRESSURE=2.0
export DRIFT=6
export MIXTURE=Xenon-Neon 48.85Pct-Iso 2.3Pct 10-10E3Vcm #Argon-Isobutane 1Pct 10-10E3Vcm \ Xenon-Neon 48.85Pct-Iso 2.3Pct 10-10E3Vcm
filename="simFiles_${NAME}/RestG4_run00029_2.0bar_aN-histogram_XeNeISO_6cmDrift_v2.4.3.root" 
# Launch restManager with the constructed filename
echo "NAME=$NAME"
echo "PRESSURE=$PRESSURE"
echo "DRIFT=$DRIFT"
echo "filename=$filename"
ls -l "$filename"  

if [ -f "$filename" ]; then
        # Launch restManager with the constructed filename. 
        restManager --c Ar_Analysis.rml --f "$filename" 
    else
        echo "File $filename does not exist."
    fi

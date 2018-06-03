#!/bin/bash

declare -a cases=("Coord_Data_Ratio_Sync_Fix"
                  "Wide_Angle_Coord_Data_Interval"
                  "Low_Speed_Coord_Data_Interval"
                  "High_Speed_Coord_Data_Interval"
                  "Coordinator_Dynamic_Speed"
                  "Coordinator_Stationary"
                )

for j in "${cases[@]}"
do
    echo $j
done

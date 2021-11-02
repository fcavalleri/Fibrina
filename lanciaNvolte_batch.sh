#/bin/bash

clo_thres=$1
n_part=$2
max=$3
act_thres=$4

mkdir NewResults
cd NewResults
mkdir 'N_'$n_part'_cl_'$clo_thres'_act_'$act_thres
cd 'N_'$n_part'_cl_'$clo_thres'_act_'$act_thres

#for run in $(seq -f "%04g" 1 $max) ; do ./main $label'_'$run ; done
for run in $(seq -f "%04g" 1 $max) ; do ../../cmake-build-debug/main $clo_thres $n_part $run $act_thres ; done

mkdir RawData
mkdir FinalCoord
mkdir nYlnDL
mkdir RawDataParameters

mv RawData_* RawData
mv FinalCoord_* FinalCoord
mv nYlnDL_* nYlnDL
mv RawDataParameters_* RawDataParameters

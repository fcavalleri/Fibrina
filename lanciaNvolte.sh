#/bin/bash
echo Closing_threshold: 
read clo_thres
echo Initial_Particle_Number: 
read n_part
echo Iterazioni:
read max
mkdir NewResults
cd NewResults
mkdir 'N_'$n_part'_cl_'$clo_thres
cd 'N_'$n_part'_cl_'$clo_thres

#for run in $(seq -f "%04g" 1 $max) ; do ./main $label'_'$run ; done

ulimit -s unlimited;
for run in $(seq -f "%04g" 1 $max) ; do ../../cmake-build-debug/main $clo_thres $n_part $run ; done

mkdir RawData
mkdir FinalCoord
mkdir nYlnDL
mkdir RawDataParameters

mv RawData_* RawData
mv FinalCoord_* FinalCoord
mv nYlnDL_* nYlnDL
mv RawDataParameters_* RawDataParameters

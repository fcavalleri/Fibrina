#/bin/bash

export LC_NUMERIC=C
ulimit -s unlimited;

for thr in `seq 0.1 0.1 0.6` 
	do for n_part in `seq 500 100 1000` 
		do ./lanciaNvolte_batch.sh $thr $n_part 2 
	done
done


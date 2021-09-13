#/bin/bash

export LC_NUMERIC=C
ulimit -s unlimited;

for thr in `seq 0.1 0.1 0.6` 
	do for n_part in `seq 26500 6625 53000` 
		do ./lanciaNvolte_batch.sh $thr $n_part 15 
	done
done


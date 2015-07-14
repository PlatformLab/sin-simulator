#\bin\bash
NUM_TRIALS=100
DIE_SIZE=10
echo "#num users | average delay" > average_delay
echo "#num users | ratio of srtf matching scenarios" > srtf_ratio
for i in `seq 2 10`;
        do
        ../sin --num-users=$i --num-trials=$NUM_TRIALS --die-size=$DIE_SIZE > temp
        echo -n $i >> average_delay
        cat temp | grep "average delay" | sed s/'average delay ratio'/' '/g >> average_delay
        echo -n $i >> srtf_ratio
        cat temp | grep "scenarios matched" | cut -d "(" -f2 | cut -d ")" -f1 >> srtf_ratio
        done

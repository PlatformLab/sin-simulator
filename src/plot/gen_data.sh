SIN_LOCATION=../sin
MEAN_DELAY_FILE=mean_delay.dat

NUM_TRIALS=320
DIE_SIZE=40

echo '"num flows" "SIN" "SRTF" "Rate Sharing" ' > $MEAN_DELAY_FILE
for i in 2 3 4 6 8 16 32;
    do
        echo -n "$i " >> $MEAN_DELAY_FILE
        TIMES_TO_RUN=$(($NUM_TRIALS*32/$i))
        $SIN_LOCATION --num-flows=$i --num-trials=$TIMES_TO_RUN --die-size=$DIE_SIZE >> $MEAN_DELAY_FILE
    done

TIMES=5
WIDTH=(256 512 1024)
PROJECTS=(
    opencl1 
    opencl2 
    opencl3
    opencl4
)

make

echo "" > result.out

for proj in ${PROJECTS[*]}
do  
    for wid in ${WIDTH[*]}
    do
        ./$proj $wid $TIMES | tee -a result.out
    done 
done


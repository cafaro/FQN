#!/bin/bash

#
# This script executes the FQN algorithm
# cycling through all the distributions...
#
# In this version, parameters are passed to the script to specify:
# - half-window-size (-k)
# - max number of items to process from the input stream (-n)
#
#####################################################################
#

echo "Starting tests for FQN..."
echo

ITEMS=""

while getopts "k:n:" option
do
case "${option}"
in
k) K=${OPTARG};;
n) N=${OPTARG};;
esac
done


if [ ! -z $N ]
then
    ITEMS="-n ${N}"
fi


if [ ! -z $K ]
then
    echo "Working with window of ${K}"
    echo
    : # $K was given

    #distributions files..
    FILES=./BenchPDF/*.csv

    #log dir
    LOGS=TestLogs/
    ERRS=TestLogs/ErrLogs/
    LOGDIR=${LOGS}/Win${K}/
    if [ ! -d ${LOGDIR} ]; then
        mkdir -p ${LOGDIR};
    fi;

    i=0
    for f in $FILES; do

        ((++i))
        [ -e "$f" ] || continue

        bname=`basename $f`
        name=${bname%.csv}
        L="-l $(wc -l < $f| tr -d ' ')"

        cname=$(echo "${name}" | awk '{print toupper($0)}')
        echo "$i) Processing $cname file..."
        echo

        q=Q5
        echo
        echo "FQN -f $f -k $K ${L} ${ITEMS}"
        (time ./FQN -f $f -k $K ${L} ${ITEMS}) >${LOGDIR}${name}${q}.txt 2>&1

            echo "$cname processing ended!!"
            echo
        done

        echo "Script Done!"
        echo

    else
        # $K was not given
        echo "You must set the half-window size:"
        echo "example: -k 100"
        echo
        exit
fi

#!/bin/bash

#
#####################################################################
#
# This script executes the OnlineQnEstimator
# cycling through all the distributions...
#
#####################################################################
#

echo "Starting tests for FQN..."
echo

WINS="100 200 300 400 500"
REPETITIONS=3
SLEN=100000


    #log dir
    LOGS=TestLogs/
    LOGDIR=${LOGS}
    if [ ! -d ${LOGDIR} ]; then
        mkdir -p ${LOGDIR};
    fi;

    echo "Distribution,n_items_tested,K,time,upds,outliers,inliers" > ${LOGDIR}QnEstimatorLog.csv

    for K in ${WINS}
    do

    echo "Working with window of ${K}"
    echo
    
    #distributions files..
    FILES=./BenchPDF/*.csv

        i=0
        for f in $FILES; do

            ((++i)) 
            [ -e "$f" ] || continue

            bname=`basename $f`
            name=${bname%.csv}
            L="-l $(wc -l < $f| tr -d ' ')"
            cname=$(echo "${name}" | awk '{print toupper($0)}')
            
            Ncount=$((SLEN + (2*K) + 1))
            ITEMS="-n ${Ncount}"

            for r in `seq 1 ${REPETITIONS}`
            do
                echo "$i, $r) Processing $cname file..."
                echo "./FQN -f $f -k $K ${ITEMS}"
                ./FQN -f $f -k $K ${L} ${ITEMS} > /dev/null 2>> ${LOGDIR}QnEstimatorLog.csv
                echo
            done #for r

        done #for f
    done #for k

    echo "Script Done!"
    echo

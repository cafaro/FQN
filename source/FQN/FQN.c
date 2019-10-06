/**
 * @file    FQN.c
 * @author  CM
 * @brief   FQn for online Qn Estimator over a sliding window
 *
 * Given an input stream and a window of fixed size (of W=2K+1 items, W is odd),
 * – following Rousseeuw and Croux paper on Qn scale estimator (1992) –
 * – comparing against the Nunkesser and Fried version of Online Qn (2007) –
 * - testing (Johnson and Mizoguchi)/(Croux and Rousseeuw) vs Arjomandi and Mirzaian (1985) –
 * as items arrive from stream, the item x_{t-k} is evaluated to check if it's outlier
 * by means of a threshold taking into account the Qn of the current time window
 * and the median of the time current window.
 * The incremental insertion sort procedure is responsible to update the values window,
 * by removing values and inserting new values as they arrive from stream.
 *
 *
 * The following code uses 2 arrays of values:
 * - one array updates a time window, (storing items in sequential order, as they arrive from stream),
 * - the other is used with an incremental insertion sort procedure to manage a window of size W
 *      of sorted items corresponding to the values in the current time window
 *
 * Finally, an array of sequence numbers, to uniquely identify each item in the time window can be used (for logging purposes only).
 *
 * @note the time window could be defined as:
 *
 * \code{.c}
 *
 *      struct Item {
 *          ITEM_VALUE item;
 *          ITEMS_COUNT seqNo;
 *      };
 *
 *      struct Item timeWindow[W];
 *
 * \endcode
 *
 * and the updates will be:
 *
 * \code{.c}
 *
 *      timeWindow[current%W].item = newItem;
 *      timeWindow[current%W].seqNo = current+1;
 *
 * \endcode
 */

#include "Options.h"
#include "Outlierness.h"
#include "Sorter.h"
#include "XXSelect.h"
#include "Timing.h"
#include "Debugger.h"



int main(int argc, char *argv[]){

    //command line parsing is mandatory (also options)
    struct LaunchOptions opt;
    if (parseCommandLine(&opt, argc, argv) != PARSING_SUCCESS){
        printUsage(argv[0]);
        return PARSING_ERROR;
    }
    
#ifndef CSV
    printLaunchOptions(&opt);
#endif
    
    char version[] = "1.0";

    ITEMS_COUNT W = opt.window_size;
    ITEMS_COUNT K = opt.half_window_size;

    //management of max lines of the input file to process:
    ITEMS_COUNT maxItemsCount = (unsigned long)~0>>1;
    if (opt.maxItemsCount > 1){
        maxItemsCount = opt.maxItemsCount;
    }
    //fprintf(stderr, "max items to process: %ld vs max items count indicated: %ld\n", maxItemsCount,  opt.maxItemsCount);

    //Qn metric
    ITEMS_COUNT h = W/2+1;
    ITEMS_COUNT kth = h*(h-1)/2;
    ITEMS_COUNT knew = kth+opt.Offset;

    //Windows
    ITEM_VALUE byTime[W];
    ITEMS_COUNT bySequence[W];
    ITEM_VALUE byValue[W];

    memset(byTime, 0, W * sizeof(ITEM_VALUE));
    memset(bySequence, 0, W * sizeof(ITEMS_COUNT));
    memset(byValue, 0, W * sizeof(ITEM_VALUE));

    //save outputs to files...
    struct OutputsStats output;
    initStats(W, &output, K, opt.filename, opt.lines);

#ifndef TESTMODE
    //open the input file
    FILE *fp = fopen(opt.filename, "r");
    if (fp == NULL) {
        fprintf(stdout, "Error opening %s\n", opt.filename);
        return -1;
    }
#else
    //or buffer the input file (TESTMODE only)
    ITEMS_COUNT streamLen = 0;
    ITEM_VALUE *streamBuffer = bufferStreamFromFile(&opt, &streamLen);
#endif

#ifndef CSV
    fprintf(stderr, "FQN for online Qn estimator .. version %s\n", version);
#endif
    
    ITEMS_COUNT oldest = 0, current = 0, seqNo = 0;
    ITEM_VALUE newItem = 0;

    //from 0 to W-1:
#ifndef TESTMODE

    char *line = NULL;
    size_t dim = 0;
    while ( (current<W) && (getline(&line, &dim, fp) != -1) && (current <= maxItemsCount) ) {
        newItem = strtod(line, NULL);

#else

    while (current<W && (current <= maxItemsCount) ) {
        //fprintf(stderr, "buf[%ld]=%.0f\n", current, streamBuffer[current]);
        newItem = streamBuffer[current];

#endif
        seqNo = current+1;

        byTime[current%W] = newItem;
        bySequence[current%W] = seqNo;

        isort_v5(byValue, current, newItem);

        #ifdef CHECKS
        debugWindows(byTime, byValue, seqNo);
        #endif

    ++current;
    }//wend 1

    ITEM_VALUE kth_value;
        
    //kth_value = selectStatisticFrom(W, byValue, knew);
    //checkForOutlier(byTime[K], bySequence[K], byValue[K], kth_value, &output);

        
    #ifdef CHECKS
    //first check
    /*
    fprintf(stderr, "\tFirst median = %f\n", byValue[K]);
    fprintf(stderr, "\tFirst middle = %f (%ld)\n", byTime[K], bySequence[K]);
    fprintf(stderr, "\tCurrent = %ld\n", current);
    CheckForArjomandi(knew, kth_value, byTime, W);
     */
    #endif

#ifndef CSV
    fprintf(stderr, "\nStarting streaming...  \n");
#endif
        
        
    //from W to the end...
    StartTheClock();
#ifndef TESTMODE
    while ( (getline(&line, &dim, fp) != -1) && (current < maxItemsCount) ) {
        newItem = strtod(line, NULL);
#else
    while (current<streamLen && (current < maxItemsCount)) {
        //fprintf(stderr, "buf[%ld]=%.0f\n", current, streamBuffer[current]);
        newItem = streamBuffer[current];

#endif
        seqNo = current+1;

        //get the oldest item before updating arrays
        ITEM_VALUE oldItem = byTime[oldest];
        oldest = (oldest+1)%W;

        byTime[current%W] = newItem;
        bySequence[current%W] = seqNo;

        updateValues_v5(byValue, W, newItem, oldItem);

        #ifdef CHECKS
        debugWindows(byTime, byValue, W);
        #endif

        kth_value = selectStatisticFrom(W, byValue, knew);

        #ifdef CHECKS
        CheckForArjomandi(knew, kth_value, byTime, W);
        #endif

        ITEMS_COUNT mpos = (oldest+K)%W;
        checkForOutlier(byTime[mpos], bySequence[mpos], byValue[K], kth_value, &output);

    ++current;
    }//wend 2
    double updtime = StopTheClock();
    
    ITEMS_COUNT countchecks = current - 2 * K - 1; // countchecks is the number of outlierness tests actually done inside the while loop
        


#ifndef TESTMODE
    if (line)
        free(line);
    fclose(fp);
    
#ifdef CSV
    fprintf(stderr,"%s,%ld,%ld,%.5f,%.5f,%ld,%ld\n", opt.filename, countchecks, K, updtime / 1000, ((double)(countchecks)/(updtime/1000)), output.outliers, output.inliers);
#else

    fprintf(stdout, "Processing ended!\nFound: \n\tOutliers: %ld,\n\tInliers: %ld,\n\tOutlierness tests (P-2*K-1): %ld\n\n", output.outliers, output.inliers, countchecks);
    fprintf(stdout, "\tTotal update time (seconds): %f\n\tUpdates per second: %f\n", updtime / 1000, ((double)(countchecks)/(updtime/1000)));
#endif
#else
    //fprintf(stderr, "...Freeing resources..\n");
    if(streamBuffer)
        free(streamBuffer);

#ifdef CSV
        fprintf(stderr,"%s,%ld,%ld,%.5f,%.5f,%ld,%ld\n", opt.filename, countchecks, K, updtime / 1000, ((double)(countchecks)/(updtime/1000)), output.outliers, output.inliers);
#else
        
        fprintf(stdout, "Processing ended!\nFound: \n\tOutliers: %ld,\n\tInliers: %ld,\n\tOutlierness tests (P-2*K-1): %ld\n\n", output.outliers, output.inliers, countchecks);
        fprintf(stdout, "\tTotal update time (seconds): %f\n\tUpdates per second: %f\n", updtime / 1000, ((double)(countchecks)/(updtime/1000)));
#endif

#endif

    
    //close stats output files..
    closeStats(&output);
    
    return 0;
}


/**
 * @file    Options.h
 * @author  CM
 * @brief   Command lines arguments
 * 
 * To launch the program, on the command line these options need to be passed:
 * - (-f) the pathfile specifying the input stream
 * - (-k) the half-window size (window will be of size=2K+1, odd)
 * - [-l], optional, the legth of the stream - for benchmark tests purposes only - 
 *          corresponding to the line count of the filename
 * 
 * 
 */ 

#ifndef __OPTIONS_H__
#define __OPTIONS_H__
#define _GNU_SOURCE

#include "Header.h"

#define BSIZE           256     //pathfile max len             
#define PARSING_ERROR   130     //CODE returned processing input parameters if ERROR
#define PARSING_SUCCESS 15      //CODE returned processing input parameters if OK

/**
 * @brief Used to collect and store input parameters
 */
struct LaunchOptions {
    char filename[BSIZE];           //stream origin
    ITEMS_COUNT lines;              //number of lines in the input stream file...
    
    ITEMS_COUNT maxItemsCount;       //max number of items to process...

    ITEMS_COUNT half_window_size;   //K
    ITEMS_COUNT window_size;        //W = 2K+1, always odd
    ITEMS_COUNT Inset;              //W*(W-1)/2
    ITEMS_COUNT Offset;             //W*(W+1)/2
};


void printUsage(char *msg);
void printLaunchOptions(struct LaunchOptions *options);
int parseCommandLine(struct LaunchOptions *options, int argc, char *argv[]);


/**
 * @brief The function buffers all the input file in the heap
 * @param options, the input parameters
 * @param itemscount, inout param on return it contains the number of elements found
 * 
 * @return the array of values processed from file
 */
ITEM_VALUE *bufferStreamFromFile(struct LaunchOptions *options, ITEMS_COUNT *itemscount);


#endif //__OPTIONS_H__

/**
 * @file Options.c
 * @author CM 
 */

#include "Options.h"


/**
 * @brief Given a file, it counts the number of lines in file, 
 * corresponding to the number of items in the input stream.
 */
ITEMS_COUNT getFileLines(char *filename){

    FILE *fp = fopen(filename, "r");
    if (fp == NULL){
        fprintf(stdout, "Error opening %s\n", filename);
        return -1;
    }
    
    ITEMS_COUNT lcount = 0;
    char* line = NULL;
    size_t dim = 0;
    while( (getline(&line, &dim, fp)) != -1) {
        ++lcount;
    }
    fclose(fp);
    if (line)
        free(line);

return lcount;   
}


ITEM_VALUE *bufferStreamFromFile(struct LaunchOptions *options, ITEMS_COUNT *itemscount){

    if (options) { 
        if (options->lines <=0) {
            options->lines = getFileLines(options->filename);
        } 

        FILE *fp = fopen(options->filename, "r");
        if (fp == NULL) {
            fprintf(stdout, "Error opening input file: '%s'\n", options->filename);
            return NULL;
        }

        ITEM_VALUE *values = malloc(sizeof(ITEM_VALUE)*options->lines);
        
        char *line = NULL;
        size_t dim = 0;
        ITEMS_COUNT idx = 0;
        while( (getline(&line, &dim, fp)) != -1) {
            values[idx] = strtod(line, NULL);
            ++idx;
        }//wend
        if (line)
            free(line);
        fclose(fp);

        *itemscount = idx;
        //free(values);

        return values;
    }//fi options

    fprintf(stderr, "ERROR: cannot buffer input file...\n");
    return NULL;
}





void printUsage(char *msg){
    fprintf(stdout, "Usage: %s -f path-to-file -k half-window-size [-l lines-in-file (: wc - l)] [-n max-num-items-to-process]\n", msg);
}


void printLaunchOptions(struct LaunchOptions *options){
    fprintf(stdout, "\nPoint Anomaly Detection over sliding window\n");
    fprintf(stdout, "Parameters:\n\tStream origin: '%s'\n", options->filename);
    
    #ifdef TESTMODE
    fprintf(stdout, "\tNumber of entries in the file (stream length)(L): %ld\n", options->lines);
    #endif

    if (options->maxItemsCount>1){
        fprintf(stdout, "\tMax number of items that will be processed from input stream: %ld\n", options->maxItemsCount);
    }

    fprintf(stdout, "\tHalf-window size (K): %ld\n", options->half_window_size);
    fprintf(stdout, "\tWindow size (W=2K+1): %ld\n", options->window_size);
    fprintf(stdout, "\tW*W: %ld, Offset: %ld, Inset: %ld\n", (options->window_size*options->window_size), options->Offset, options->Inset);
    
    #ifdef TESTMODE
    fprintf(stdout, "\t -----> TESTING MODE <-----\n\n");
    #elif CHECKS
    fprintf(stdout, "\t ++++++ CHECK MODE ++++++\n\n");
    #else
    fprintf(stdout, "\t ***** NORMAL MODE *****\n\n");
    #endif    
}



int parseCommandLine(struct LaunchOptions *options, int argc, char *argv[]){
    
    if (argc == 1) {
        return PARSING_ERROR;
    }

    memset(options, 0, sizeof(struct LaunchOptions));

    bool flag1 = false, flag2= false; //-f and -k are mandatory!!!

    int c=0;
    while ( (c = getopt(argc, argv, "f:k:l:n:")) != -1) {
        
        switch (c) {

            case 'f':
                if (strlen(optarg)<BSIZE){
                    strncpy(options->filename, optarg, strlen(optarg));
                    flag1 = true;
                } else {
                    fprintf(stderr, "Error: filename too long\n");
                    return PARSING_ERROR;
                }
                break;
        

            case 'k':
                options->half_window_size = (ITEMS_COUNT)strtol(optarg, NULL, 10);

                if (options->half_window_size > 0){
                    flag2 = true;
                    options->window_size =(ITEMS_COUNT)(2*options->half_window_size + 1); 
                    options->Inset = (ITEMS_COUNT)(options->window_size * (options->window_size-1) / 2);
                    options->Offset = (ITEMS_COUNT)(options->window_size * (options->window_size+1) / 2);
                } else {
                    fprintf(stderr, "Error: invalid half-window-size '%s'\n", optarg);
                    return PARSING_ERROR;
                }
                break;
            

            case 'l':
                options->lines = (ITEMS_COUNT)strtol(optarg, NULL, 10);

                if (options->lines < 1){
                    fprintf(stderr, "Error: invalid stream length '%ld'\n", options->lines);
                    return PARSING_ERROR;
                }
                break;

            case 'n':
                options->maxItemsCount = (ITEMS_COUNT)strtol(optarg, NULL, 10);

                if (options->maxItemsCount < 1){
                    fprintf(stderr, "Error: invalid number of items to process '%ld'\n", options->maxItemsCount);
                    return PARSING_ERROR;
                }
                break;


            default:
                fprintf(stderr, "?? getopt returned character code 0%o ??\n", c);
                return PARSING_ERROR; 
                break;
        }
    }

    if (flag1 && flag2){
        #ifdef TESTMODE
        if (options->lines <=0){
            options->lines = getFileLines(options->filename);
        } 
        if (options->lines < 2*options->half_window_size + 1){
            fprintf(stderr, "\tAttention: input file too short for the window specified\n");
        }
        #endif

        return PARSING_SUCCESS;
    } else {
        return PARSING_ERROR;
    }    
}

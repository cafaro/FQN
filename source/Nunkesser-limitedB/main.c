/**
 * @file main.c
 * @note Modified version to use Arjomandi instead Croux and to limit the size of the buffer B
 * 
 */
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "qnonline.h"

#include <string.h>

long secs, usecs;
struct timeval tt;

void StartTheClock()
{
    gettimeofday(&tt,NULL);
    secs=tt.tv_sec;
    usecs=tt.tv_usec;
}

double StopTheClock()
{
    gettimeofday(&tt,NULL);
    secs=tt.tv_sec-secs;
    usecs=tt.tv_usec-usecs;
    return 1000*secs+((double)usecs/1000.0);
}

static double *data;
static double *window_buffer;
static int window_length;






void init_window_buffer(FILE *data_file, int num_items, int win_len) {
    data = (double*)malloc(sizeof(double)*num_items);
    for (int i = 0; i < num_items; i++) {
        double item;
        if (fscanf(data_file, "%lf", &item) > 0) {
            data[i] = item;
        } else {
            printf("Not enough items!\n");
            exit(1);
        }
    }
    window_length = win_len;
    window_buffer = (double *)malloc(sizeof(double)*win_len);
    for (int i = 0; i < win_len; i++) {
        window_buffer[i] = data[i];
    }
}

static int oldest_item_idx = 0;


/**
 * @brief read from data buffer and puts into window_buffer
 * @param next_item_idx is the index of the next data to be read from data stream...
 * @param new_item is the incoming value
 * @param oldest_item is the item to be removed
 */
void update_window_buffer(int next_item_idx, double *new_item, double *oldest_item) {
    *new_item = data[next_item_idx];
    *oldest_item = window_buffer[oldest_item_idx];
    window_buffer[oldest_item_idx] = *new_item;
    oldest_item_idx = (oldest_item_idx + 1) % window_length;
}

void get_window_buffer(double *window) {
    for (int i = 0; i < window_length; i++) {
        window[i] = window_buffer[(oldest_item_idx + i) % window_length];
    }
}

double getQnScaleFactor(long n, double scalingFactor){
    double dn;
    if (n <= 9)
    {
        if (n == 2) {
            dn = .399;
        } else if (n == 3) {
            dn = .994;
        } else if (n == 4) {
            dn = .512;
        } else if (n == 5) {
            dn = .844;
        } else if (n == 6) {
            dn = .611;
        } else if (n == 7) {
            dn = .857;
        } else if (n == 8) {
            dn = .669;
        } else {
            dn = .872;
        }
    }
    else
    {
        if (n % 2 == 1) {
            dn = n / (n + 1.4);
        } else {
            dn = n / (n + 3.8);
        }
    }//fi (n<9)
    return  (dn * scalingFactor);
}

typedef struct _log_point {
    double item;
    long seq;
} log_point;




int checkForOutlier(double middle, long seqNo, double median, double kth_value, double QnScaleFactor, log_point *outliers, log_point *inliers){

    static int num_outliers = 0;
    static int num_inliers = 0;

    //check if the central point in window is outlier:
    if ( (fabs(middle - median) - 3.0*QnScaleFactor*kth_value) > 0) {

        outliers[num_outliers].item = middle;
		outliers[num_outliers].seq = seqNo;

        ++num_outliers;

    } else {
        //not outlier
        inliers[num_inliers].item = middle;
		inliers[num_inliers].seq = seqNo;

        ++num_inliers;

    }
    //fi check test
    return num_outliers;
}






//************************************************************   MAIN FUNCTION

int main (int argc, char *argv[]) {
    BOSTree *X;
    BOSTree *Y;
    BOSTree *B;

#if defined ENABLE_MIN_TEST || defined ENABLE_TEST
    BOSTree *X_test;
    BOSTree *Y_test;
    BOSTree *B_test;
#endif
    
    BOSNode *qn0_node;
    BOSNode *qn0_node_offline;
    double qn0_exact;

    if (argc < 4) {
        printf("Usage: %s filename number_of_items half_window_length\n", argv[0]);
        exit(1);
    }

    FILE *data_file = fopen(argv[1], "r");
    if (data_file == NULL) {
        printf("\n\nError on opening input file\n\n");
        exit(1);
    }

    int num_items = atoi(argv[2]);
    int k = atoi(argv[3]); // window length / 2
    int w = (k*2)+1; // window length

    log_point outliers[num_items];
    log_point inliers[num_items];
    double qnvalue = 0;

    double QnFactor= 2.2219; //as defined by Rousseeuw and Croux (paper 1992)
    double QnScaleMultiplier = getQnScaleFactor(w, QnFactor);

    init_window_buffer(data_file, num_items, w);
    fclose(data_file);

    double current_window[w];
    get_window_buffer(current_window);

    init_trees(current_window, w, &X, &Y, &B);
    qn0_node = compute_qn0_offline(X, Y, B, w, &qnvalue);

#ifdef ENABLE_TEST
    assert(!test_verify_pointers(X, Y, B, NULL, false));
#endif

    // if (!qn0_node ) {
    //     printf("Test failed at first offline computation\n\n");
    //     exit(1);
    // }

#ifdef ENABLE_TEST
    printf("***** Precomputation:\n\nQN0 with Nunkesser offline algoritm: %f = x[%d] + y[%d]\n\n", ((BNode *) qn0_node->key)->value,
           bostree_rank(((BNode *) qn0_node->key)->toX),
           bostree_rank(((BNode *) qn0_node->key)->toY));
#endif


    double new_item;
    double oldest_item;
    int seq_no = w;
    double upd_time = 0.0;

    // Check for outliers:

    //middle by time in the window: the first time is the center point of the win_buffer
    double middle_value = current_window[k];
    
    //middle by value in the window:
    BOSNode *x_median = bostree_select(X, k);
    double median_value = *(double *)(x_median->key);
    
    int seq = k;
    double qn0 = 0;
    if (qn0_node){
        qn0 = ((BNode *)(qn0_node)->key)->value;
    } else {
        qn0 = qnvalue;
    }
    
    
    checkForOutlier(middle_value, seq, median_value, qn0, QnScaleMultiplier, outliers, inliers);
    
    // *****************
    int num_outliers = 0;
    
    //int num_recalc = 0;
    //long total_B_size = 0;
    //long max_B_size = 0;
    
    

    StartTheClock();
	//int oversizes = 0;

    for (int iter = 0; iter < (num_items-window_length); iter++ ) {

        update_window_buffer(window_length+iter, &new_item, &oldest_item);
#ifdef ENABLE_TEST
        printf("Iteration %d, processing item %d\n\n", iter, window_length + iter + 1);
        printf("\n\n***** Iteration %d\n\nItem to be inserted = %f, item to be deleted = %f\n\n", iter, new_item, oldest_item);
#endif
        //fprintf(stderr,"\r%d", iter);
        
    if (qn0_node){
        qn0_node = update_window(new_item, seq_no, oldest_item, X, Y, B, qn0_node);
        
        int Blen = bostree_node_count(B);
	if (qn0_node){
            qnvalue = ((BNode *)(qn0_node)->key)->value;
	}
        if (Blen > 2*w){
            qn0_node = NULL;
        }

    } else {
        //++oversizes;

        double *x_node_key = (double *)malloc(sizeof(double));
        NodeData *x_node_data = (NodeData *)malloc(sizeof(NodeData));
        *x_node_key = new_item;
        x_node_data->bs = NULL;
        x_node_data->bg = NULL;
        x_node_data->seqNo = seq_no;
        BOSNode *x_node = bostree_insert(X, (void *)x_node_key, (void *)x_node_data);


        double *y_node_key = (double *)malloc(sizeof(double));
        NodeData *y_node_data = (NodeData *)malloc(sizeof(NodeData));
        *y_node_key = -new_item;
        y_node_data->bs = NULL;
        y_node_data->bg = NULL;
        y_node_data->seqNo = seq_no;
        BOSNode *y_node = bostree_insert(Y, (void *)y_node_key, (void *)y_node_data);

        BOSNode *oldx = bostree_lookup(X, &oldest_item);
        bostree_remove(X,oldx);
        double old_y = -oldest_item;
        BOSNode *oldy = bostree_lookup(Y, &old_y);
        bostree_remove(Y,oldy);

    }

        //limit B to O(2s):
        if (!qn0_node) {
           //fprintf(stderr, "item %d\n", iter);
	#ifdef ENABLE_TEST
                printf("\nQN0 not in buffer at online iteration %d\n\n", iter);
            #endif
		
            
            reset_B(&B);
            qn0_node = compute_qn0_offline(X, Y, B, w, &qnvalue);
	        
            //num_recalc++;

            #ifdef ENABLE_TEST
                print_X(X);
                print_Y(Y);
                print_B(B);
                print_X_plus_Y(X, Y);
                printf("\n\n");
            #endif
        }//fi Blen


        // check for outliers
        get_window_buffer(current_window);

        //middle by time in the window: the first time is the center point of the win_buffer
        middle_value = current_window[k];

        //middle by value in the window:
        x_median = bostree_select(X, k);
        median_value = *(double *)(x_median->key);

        seq = seq_no-k;
        if (qn0_node){
            qn0 = ((BNode *)(qn0_node)->key)->value;
        } else{
            qn0 = qnvalue;
        }

        num_outliers = checkForOutlier(middle_value, seq, median_value, qn0, QnScaleMultiplier, outliers, inliers);
        
        // ****************
#ifdef ENABLE_TEST
        printf("\nQN0 with Nunkesser after iteration %d: %f = x[%d] + y[%d]\n\n", iter, ((BNode *) qn0_node->key)->value,
               bostree_rank(((BNode *) qn0_node->key)->toX),
               bostree_rank(((BNode *) qn0_node->key)->toY));
#endif

#if defined ENABLE_MIN_TEST || defined ENABLE_TEST
        init_trees(current_window, w, &X_test, &Y_test, &B_test);
        qn0_node_offline = compute_qn0_offline(X_test, Y_test, B_test, w);
#ifdef ENABLE_TEST
        printf("QN0 with Nunkesser offline algoritm at iteration %d: %f = x[%d] + y[%d]\n\n", iter,
               ((BNode *) qn0_node_offline->key)->value,
               bostree_rank(((BNode *) qn0_node_offline->key)->toX),
               bostree_rank(((BNode *) qn0_node_offline->key)->toY));
#endif
        // Exact computation of qn0;
        int m = w * (w - 1) / 2;

        double b[m];
        int c = 0;
        for (int i = 0; i < w; i++) {
            for (int j = i + 1; j < w; j++) {
                b[c] = (current_window[i] - current_window[j]) > 0 ? (current_window[i] - current_window[j]) : (current_window[j] - current_window[i]);
                c++;
            }
        }

        if (m != c) {
            printf("Something wrong with exact computation\n\n");
            exit(1);
        };

        int h = w / 2 + 1;
        int k = h * (h - 1) / 2;

        qn0_exact = qselect(b, m, k - 1); /* k-1: 0-indexing */
#ifdef ENABLE_TEST
        printf("QN0 with exact computation at iteration %d: %f\n\n", iter, qn0_exact);
#endif
        assert(qn0_exact == ((BNode *) qn0_node->key)->value);
        assert(((BNode *) qn0_node_offline->key)->value == ((BNode *) qn0_node->key)->value);
        assert(bostree_rank(((BNode *) qn0_node_offline->key)->toX) == bostree_rank(((BNode *) qn0_node->key)->toX));
        assert(bostree_rank(((BNode *) qn0_node_offline->key)->toY) == bostree_rank(((BNode *) qn0_node->key)->toY));
        delete_trees(&X_test, &Y_test, &B_test);
#endif
        seq_no++;
    }
    upd_time = StopTheClock();

#ifndef CSV
    printf("\n\nTest completed successfully! Time for processing %d items: %.3f seconds. Updates/second: %.3f. Number of recalc: %d\n\n", (seq_no-w), upd_time/1000, (double)(seq_no-w)/(upd_time/1000), num_recalc);
#else
    //fprintf(stderr, "%s,%d,%d,%.5f,%.5f,%d,%d,%d,%d,%f\n",argv[1], seq_no-w, k, upd_time/1000, (seq_no-w)/(upd_time/1000), num_outliers, (seq_no-w)-num_outliers, oversizes, 0, 0); //num_recalc, max_B_size, (double)total_B_size/(seq_no-w));
    fprintf(stderr, "%s,%d,%d,%.5f,%.5f,%d,%d\n",argv[1], seq_no-w, k, upd_time/1000, (seq_no-w)/(upd_time/1000), num_outliers, (seq_no-w)-num_outliers); //num_recalc, max_B_size, (double)total_B_size/(seq_no-w));
#endif
    

    bool quit=false;
    char *path= malloc(sizeof(char)*(strlen(argv[1])+1)); 
    memcpy(path, argv[1], sizeof(char)*(strlen(argv[1])+1));

    int u = strlen(path);
    while(!quit && u>=0) {
        if(path[u]=='/'){
			quit = true;
            break;
        }
        --u;
    }//wend

	char distrName[32] = {0};
	snprintf(distrName, 31, "%s", &path[u+1]);
    distrName[31]='\0';
	if (path)
		free(path);

    int len = strlen(distrName) + 13;
    char fname[len];

    sprintf(fname, "data/%s_out_%d.csv", distrName, k);
    

    FILE *fp_log_out = fopen(fname, "w");
    if (fp_log_out == NULL){
        fprintf(stderr, "Error opening %s\n", fname);
        exit(1);
    }
    
    for(long u = 0; u<(num_outliers); ++u){
        fprintf(fp_log_out, "%ld,%.3f\n", outliers[u].seq, outliers[u].item);
    }
    fclose(fp_log_out);
    
    sprintf(fname, "data/%s_in_%d.csv", distrName, k);

    fp_log_out = fopen(fname, "w");
    if (fp_log_out == NULL){
        fprintf(stderr, "Error opening %s\n", fname);
        exit(1);
    }
    
    long num_inliers = (seq_no-w)-num_outliers;
    
    for(long u = 0; u<(num_inliers); ++u){
        fprintf(fp_log_out, "%ld,%.3f\n", inliers[u].seq, inliers[u].item);
    }
    fclose(fp_log_out);
    
    
    return 0;
}

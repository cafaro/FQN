/**
 * @file Debugger.c
 * @author CM
 */

#include "Debugger.h"
#include "QuickSelect.h"

int openDebugFiles(char *file1, char *file2, char *file3, struct debugFiles *df){

    df->fp_in = fopen(file1, "w");
    if (df->fp_in == NULL){
        fprintf(stdout, "Error opening %s\n", file1);
        return -1;
    }

    df->fp_twin = fopen(file2, "w");
    if (df->fp_twin == NULL){
        fprintf(stdout, "Error opening %s\n", file2);
        return -1;
    }

    df->fp_vwin = fopen(file3, "w");
    if (df->fp_vwin == NULL){
        fprintf(stdout, "Error opening %s\n", file3);
        return -1;
    }

    return 0;
}



void closeDebugFiles(struct debugFiles *df){
    if (df!=NULL){
        //if (df->fp_in)
        fclose(df->fp_in);
        fclose(df->fp_twin);
        fclose(df->fp_vwin);
    }
}


void debugTimeW(ITEM_VALUE *t, ITEMS_COUNT *s, ITEMS_COUNT n, ITEMS_COUNT oldest, FILE *fp) {

    for(ITEMS_COUNT u = 0; u < n; ++u) {
        ITEMS_COUNT p = (oldest+u)%n;
        fprintf(fp, "%.0f (%ld), ", t[p], s[p]);
    }
    fprintf(fp, "\n");
}

void debugValueW(ITEM_VALUE *v, ITEMS_COUNT n, FILE *fp){
    for(ITEMS_COUNT u = 0; u < n; ++u) {
        fprintf(fp, "%.0f, ", v[u]);
    }
    fprintf(fp, "\n");
}

//************************** Exact windows

/**
 * @brief Comparator function used by qsort() to manage ascending order
 */
int asc_values(const void *p, const void *q){
    ITEM_VALUE a = *(ITEM_VALUE *)p;
    ITEM_VALUE b = *(ITEM_VALUE *)q;
    
    if (a<b)
        return -1; //ascending order
    
    if (a>b)
        return 1;

return 0;
}


void debugWindows(ITEM_VALUE *t, ITEM_VALUE *v, ITEMS_COUNT n){

    ITEM_VALUE *a = malloc(n*sizeof(ITEM_VALUE));
    memcpy(a, t, sizeof(ITEM_VALUE)*n);

    qsort(a, n, sizeof(ITEM_VALUE), &asc_values );

    for(ITEMS_COUNT u = 0; u<n; ++u){
        if (a[u] != v[u]){
            fprintf(stderr, "\tIncorrect order in byValue @%ld\t Should be %.1f, it's %.1f\n", u, a[u], v[u]);
        }
    }

    free(a);
}


void CheckArjomandi(ITEMS_COUNT knew, ITEM_VALUE kth_value, ITEM_VALUE *t, ITEMS_COUNT n){
       
    //ITEM_VALUE diffs[n*n];
    ITEM_VALUE *diffs = malloc(sizeof(ITEM_VALUE)*n*n); 
    
    ITEMS_COUNT idx = 0;
    for(ITEMS_COUNT i=0; i<n; ++i){
        for(ITEMS_COUNT j = 0; j<n; ++j){
            diffs[idx] = t[i]-t[j];
            ++idx;
        }
    }

    qsort(diffs, n*n, sizeof(ITEM_VALUE), &asc_values);

    ITEM_VALUE exact_k = diffs[knew-1];

    if (exact_k != kth_value){
        fprintf(stderr, "ERROR: selected kth: %f, exact kth: %f\n", kth_value, exact_k);
    }

    free(diffs);
}

void CheckForArjomandi(ITEMS_COUNT knew, ITEM_VALUE kth_value, ITEM_VALUE *t, ITEMS_COUNT n){
       
    //ITEM_VALUE diffs[n*n];
    ITEM_VALUE *diffs = malloc(sizeof(ITEM_VALUE)*n*n); 
    
    ITEMS_COUNT idx = 0;
    for(ITEMS_COUNT i=0; i<n; ++i){
        for(ITEMS_COUNT j = 0; j<n; ++j){
            diffs[idx] = t[i]-t[j];
            ++idx;
        }
    }

    // qsort(diffs, n*n, sizeof(ITEM_VALUE), &asc_values);
    // ITEM_VALUE exact_k = diffs[knew-1];

    ITEM_VALUE exact_k = quickselect(diffs, n*n, knew-1);

    if (exact_k != kth_value){
        fprintf(stderr, "ERROR: selected kth: %f, exact kth: %f\n", kth_value, exact_k);
    }

    free(diffs);
}

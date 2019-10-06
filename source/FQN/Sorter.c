/**
 * @file Sorter.c
 * @author CM
 */


#include "Sorter.h"
#include "QuickSelect.h"


void isort_v3(ITEM_VALUE *V, ITEMS_COUNT len, ITEM_VALUE new) {
    
    if (len == 0){
        V[0] = new;
        return;
    }

    ITEMS_COUNT P = -1;
    
    if (new <= V[0])  {
        P = 0;
        //fprintf(stderr, "%f is new min, P = %ld\n", new, P);
    } else if(new >= V[len-1]){
        P = len;
        //fprintf(stderr, "%f is new max, P = count: %ld\n", new, P);
    } else {

        ITEMS_COUNT min, max, med;
        bool found = false;
        
        min = 1; max=len-1;
        while ( (max>=min) && !found) {
        
            med = (min+max)/2;

            if (new < V[med]){
                max = med-1;
            } else if (new > V[med]) {
                min = med+1;
            } else {
                found = true;
                P = med;
            }
        }//wend

        if (!found){
            P=min;
        }
    }//fi

    ITEMS_COUNT m = len+1;
    ITEM_VALUE swap[m];
    memcpy(swap, V, sizeof(ITEM_VALUE)*P);
    swap[P] = new;
    memcpy(&swap[P+1], &V[P], sizeof(ITEM_VALUE)*(m-1-P));

    
    memcpy(V, swap, sizeof(ITEM_VALUE)*m);
}



void updateValues_v3(ITEM_VALUE *V, ITEMS_COUNT n, ITEM_VALUE new, ITEM_VALUE old){
    
    if (old == new){
        //fprintf(stderr, "deleting: %.1f, adding %.1f=> no change to array of values\n", old, new);
        return;
    }

    ITEMS_COUNT p1 = -1, p2=-1;
    
    ITEMS_COUNT min, max, med;
    bool found = false;
        
    min = 0; max=n-1;
    while ( (max>=min) && !found) {
        med = (min+max)/2;

        if (old < V[med]){
            max = med-1;
        } else if (old > V[med]) {
            min = med+1;
        } else {
            found = true;
            p1 = med;
        }
    }//wend

    if (!found){
        fprintf(stderr, "ERROR: item %.1f not found in array of sorted values\n", old);
        exit(1);
    }
    
    ITEMS_COUNT m = n-1;
    ITEM_VALUE *swap = malloc(sizeof(ITEM_VALUE)*(m+1));
    memcpy(swap, V, sizeof(ITEM_VALUE)*p1);
    memcpy(&swap[p1], &V[p1+1], sizeof(ITEM_VALUE)*(m-p1));

    //swap contains n items, the last is not updated yet: it will be after isort_v3()
    isort_v3(swap, m, new);
    memcpy(V, swap, sizeof(ITEM_VALUE)*n);
    free(swap);
}



void isort_v4(ITEM_VALUE *V, ITEMS_COUNT len, ITEM_VALUE new) {
    
    if (len == 0){
        V[0] = new;
        return;
    }

    //@todo: binary search
    ITEMS_COUNT p = 0;
    for(p=0; p<len; ++p){
        if (V[p]>= new){
            break;
        }
    }

    
    // if (new <= V[0])  {
    //     P = 0;
    //     //fprintf(stderr, "%f is new min, P = %ld\n", new, P);
    // } else if(new >= V[len-1]){
    //     P = len;
    //     //fprintf(stderr, "%f is new max, P = count: %ld\n", new, P);
    // } else {

    //     ITEMS_COUNT min, max, med;
    //     bool found = false;
        
    //     min = 1; max=len-1;
    //     while ( (max>=min) && !found) {
        
    //         med = (min+max)/2;

    //         if (new < V[med]){
    //             max = med-1;
    //         } else if (new > V[med]) {
    //             min = med+1;
    //         } else {
    //             found = true;
    //             P = med;
    //         }
    //     }//wend

    //     if (!found){
    //         P=max;
    //     }
    // }//fi

    ITEMS_COUNT blocksize = len-p+1;
    ITEM_VALUE tmp[blocksize];
    tmp[0]=new;
    memcpy(&tmp[1], &V[p], sizeof(ITEM_VALUE)*(blocksize-1));
    
    memcpy(&V[p], tmp, sizeof(ITEM_VALUE)*blocksize);
}


void updateValues_v4(ITEM_VALUE *V, ITEMS_COUNT n, ITEM_VALUE new, ITEM_VALUE old){
    
    if (new==old){
        return;
    }

    ITEMS_COUNT p1=-1, p2=-1;
    
    ITEMS_COUNT min, max, med;
    bool found = false;
        
    min = 0; max=n-1;
    while ( (max>=min) && !found) {
        med = (min+max)/2;

        if (old < V[med]){
            max = med-1;
        } else if (old > V[med]) {
            min = med+1;
        } else {
            found = true;
            p1 = med;
        }
    }//wend
    if (!found){
        fprintf(stderr, "ERROR: item %.1f not found in array of sorted values\n", old);
        exit(1);
    }

    //got p1, looking for p2...
    found = false;    
    min = 0; max=n-1;
    while ( (max>=min) && !found) {
        med = (min+max)/2;

        if (new < V[med]){
            max = med-1;
        } else if (new > V[med]) {
            min = med+1;
        } else {
            found = true;
            p2 = med;
        }
    }//wend
    if (!found){
        p2 = min;
    }
    

    if (p1==p2){
        V[p1] = new;
        return;
    }

    if (p1<p2){

        ITEMS_COUNT block = (p2-p1);
        ITEM_VALUE tmp[block];

        memcpy(tmp, &V[p1+1], sizeof(ITEM_VALUE)*(block-1));
        tmp[block-1]= new;

        memcpy(&V[p1], tmp, sizeof(ITEM_VALUE)*block);

    } else {

        ITEMS_COUNT block = (p1-p2)+1;
        ITEM_VALUE tmp[block];
        tmp[0] = new;

        memcpy(&tmp[1], &V[p2], sizeof(ITEM_VALUE)*(block-1));

        memcpy(&V[p2], tmp, sizeof(ITEM_VALUE)*block);
    }
}


//********************************************************************************** INSERTION SORT V5

void isort_v5(ITEM_VALUE *V, ITEMS_COUNT len, ITEM_VALUE new){

    if (len == 0){
        V[0] = new;
        return; 
    }

    ITEM_VALUE tmp;
    ITEMS_COUNT k = 0;
    
    //window not full
    for(k = 0; k < len; ++k){

        if (new <= V[k]){
            tmp = V[k];  
            V[k] = new;
            new = tmp;          
        }

    }//for
    V[k] = new;

}



void updateValues_v5(ITEM_VALUE *V, ITEMS_COUNT n, ITEM_VALUE new, ITEM_VALUE old){
    
    if (old == new)
    {
        //window is full but new == old: nothing changes
        return;
    } 

    //@todo debug from here
    //@note new<old OR new>old

    if (new < old) {
        //p2<=p1: get first of old clones

        ITEM_VALUE tmp = old;        
        for(ITEMS_COUNT k = 0; k < n; ++k) {

            if (new <= V[k]) {
                // fprintf(stderr, "insert %.1f at %ld\n", new, k);
                tmp = V[k];  
                V[k] = new;
                new = tmp;

                if (new==old){
                    break;
                }
            }//fi

        }//for k

    } else {
        //p1 <= p2: get last of old clones

        ITEM_VALUE tmp;
        for(ITEMS_COUNT k = n-1; k >= 0; --k){
            if (new >= V[k]) {
                tmp = V[k];
                V[k] = new;
                new = tmp;
            
                if (new == old){
                    break;
                }
            }//fi
        }//for k
    }//fi old
}


//********************************************************************* ISORT V5.1

ITEMS_COUNT getFirstOldestPos(ITEM_VALUE *V, ITEMS_COUNT n, ITEM_VALUE old){

    ITEMS_COUNT min, max, med, p1=-1;
    bool found = false;
        
    min = 0; max=n-1;
    while ( (max>=min) && !found) {
        med = (min+max)/2;

        if (old < V[med]){
            max = med-1;
        } else if (old > V[med]) {
            min = med+1;
        } else {
            found = true;
            p1 = med;
        }
    }//wend

    if (!found){
        fprintf(stderr, "ERROR: item %.1f not found in array of sorted values\n", old);
        exit(1);
    }

    //get the first of "old" clone values...if any 
    ITEMS_COUNT succ = p1;
    while(V[succ]==V[succ-1]){
        succ=succ-1;
    }
    return succ;
}



ITEMS_COUNT getLastOldestPos(ITEM_VALUE *V, ITEMS_COUNT n, ITEM_VALUE old){

    ITEMS_COUNT min, max, med, p1=-1;
    bool found = false;
        
    min = 0; max=n-1;
    while ( (max>=min) && !found) {
        med = (min+max)/2;

        if (old < V[med]){
            max = med-1;
        } else if (old > V[med]) {
            min = med+1;
        } else {
            found = true;
            p1 = med;
        }
    }//wend

    if (!found){
        fprintf(stderr, "ERROR: item %.1f not found in array of sorted values\n", old);
        exit(1);
    }

    //get the last of "old" clone values...if any 
    ITEMS_COUNT succ = p1;
    while(V[succ]==V[succ+1]){
        succ=succ+1;
    }
    return succ;
}

void updateValues_v51(ITEM_VALUE *V, ITEMS_COUNT n, ITEM_VALUE new, ITEM_VALUE old){
    
    if (old == new)
    {
        //window is full but new == old: nothing changes
        return;
    } 

    //@todo debug from here
    //@note new<old OR new>old

    if (new < old) 
    {
        //p2<=p1: get first of old clones
        ITEMS_COUNT P1 = getFirstOldestPos(V, n, old);

        ITEM_VALUE tmp = old;        
        for(ITEMS_COUNT k = 0; k <= P1; ++k) {

            if (new <= V[k]) {
                // fprintf(stderr, "insert %.1f at %ld\n", new, k);
                tmp = V[k];  
                V[k] = new;
                new = tmp;

                if (new==old){
                    break;
                }
            }//fi

        }//for k

    }//fi new<old
    else 
    {
        //p1 <= p2: get last of old clones
        ITEMS_COUNT P1 = getLastOldestPos(V, n, old);

        ITEM_VALUE tmp;
        for(ITEMS_COUNT k = n-1; k >= P1; --k){
            if (new >= V[k]) {
                tmp = V[k];
                V[k] = new;
                new = tmp;
            
                if (new == old){
                    break;
                }
            }//fi
        }//for k

    }//fi old<new
}


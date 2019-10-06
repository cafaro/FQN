/*
 *  Copyright (C) 2019	Marco Pulimeno -- University of Salento
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*  The qn* and whimed routines are modified versions of the original C code
 *  in files qn_sn.c by Martin Maechler and wgt_himed_templ.h,
 *  which are contained in the robustbase R package.
 *  The qselect routine and relative support functions are cleaned and adapted versions
 *  from https://github.com/swenson/sort code, distributed by Christopher Swenson under
 *  the MIT License (MIT).
 */

#include "qn.h"
#include "common.h"

/* ----------------- sorting routines ----------------- */

#ifndef SORT_SWAP
#define SORT_SWAP(x,y) {double _sort_swap_temp = (x); (x) = (y); (y) = _sort_swap_temp;}
#endif

#ifndef SORT_CMP
#define SORT_CMP(x, y)  ((x) < (y) ? -1 : ((x) == (y) ? 0 : 1))
#endif

/* Function used to do a binary search for binary insertion sort */
static __inline size_t binary_insertion_find(double *dst, const double x,
                                             const size_t size) {
    size_t l, c, r;
    double cx;
    l = 0;
    r = size - 1;
    c = r >> 1;

    /* check for out of bounds at the beginning. */
    if (SORT_CMP(x, dst[0]) < 0) {
        return 0;
    } else if (SORT_CMP(x, dst[r]) > 0) {
        return r;
    }

    cx = dst[c];

    while (1) {
        const int val = SORT_CMP(x, cx);

        if (val < 0) {
            if (c - l <= 1) {
                return c;
            }
            r = c;
        } else { /* allow = for stability. The binary search favors the right. */
            if (r - c <= 1) {
                return c + 1;
            }

            l = c;
        }

        c = l + ((r - l) >> 1);
        cx = dst[c];
    }
}

/* Binary insertion sort, but knowing that the first "start" entries are sorted.  Used in timsort. */
static void binary_insertion_sort_start(double *dst, const size_t start, const size_t size) {
    size_t i;

    for (i = start; i < size; i++) {
        size_t j;
        double x;
        size_t location;

        /* If this entry is already correct, just move along */
        if (SORT_CMP(dst[i - 1], dst[i]) <= 0) {
            continue;
        }

        /* Else we need to find the right place, shift everything over, and squeeze in */
        x = dst[i];
        location = binary_insertion_find(dst, x, i);

        for (j = i - 1; j >= location; j--) {
            dst[j + 1] = dst[j];

            if (j == 0) { /* check edge case because j is unsigned */
                break;
            }
        }

        dst[location] = x;
    }
}

/* Binary insertion sort */
void binary_insertion_sort(double *dst, const size_t size) {
    /* don't bother sorting an array of size <= 1 */
    if (size <= 1) {
        return;
    }

    binary_insertion_sort_start(dst, 1, size);
}

static __inline size_t quick_sort_partition(double *dst, const size_t left,
                                            const size_t right, const size_t pivot) {
    double value = dst[pivot];
    size_t index = left;
    size_t i;
    int not_all_same = 0;
    /* move the pivot to the right */
    SORT_SWAP(dst[pivot], dst[right]);

    for (i = left; i < right; i++) {
        int cmp = SORT_CMP(dst[i], value);
        /* check if everything is all the same */
        not_all_same |= cmp;

        if (cmp < 0) {
            SORT_SWAP(dst[i], dst[index]);
            index++;
        }
    }

    SORT_SWAP(dst[right], dst[index]);

    /* avoid degenerate case */
    if (not_all_same == 0) {
        return SIZE_MAX;
    }

    return index;
}

static void quick_sort_recursive(double *dst, const size_t left, const size_t right) {
    size_t pivot;
    size_t new_pivot;

    if (right <= left) {
        return;
    }

    if ((right - left + 1U) < 16U) {
        binary_insertion_sort(&dst[left], right - left + 1U);
        return;
    }

    pivot = left + ((right - left) >> 1);
    /* this seems to perform worse by a small amount... ? */
    /* pivot = MEDIAN(dst, left, pivot, right); */
    new_pivot = quick_sort_partition(dst, left, right, pivot);

    /* check for partition all equal */
    if (new_pivot == SIZE_MAX) {
        return;
    }

    quick_sort_recursive(dst, left, new_pivot - 1U);
    quick_sort_recursive(dst, new_pivot + 1U, right);
}

void quick_sort(double *dst, const size_t size) {
    /* don't bother sorting an array of size 1 */
    if (size <= 1) {
        return;
    }

    quick_sort_recursive(dst, 0U, size - 1U);
}

/* ----------------- support routines ----------------- */

#define SWAP(a,b) temp=(a);(a)=(b);(b)=temp;

double qselect(double *arr, int n, int k) {
    int i,ir,j,l,mid,idx;
    double a,temp;

    l=0;
    ir=n-1;

    for(;;) {
        if (ir <= l+1) {
            if (ir == l+1 && arr[ir] < arr[l]) {
                SWAP(arr[l],arr[ir]);
            }
            return arr[k];
        }
        else {
            mid=(l+ir) >> 1;
            SWAP(arr[mid],arr[l+1]);
            if (arr[l] > arr[ir]) {
                SWAP(arr[l],arr[ir]);
            }
            if (arr[l+1] > arr[ir]) {
                SWAP(arr[l+1],arr[ir]);
            }
            if (arr[l] > arr[l+1]) {
                SWAP(arr[l],arr[l+1]);
            }
            i=l+1;
            j=ir;
            a=arr[l+1];
            for (;;) {
                do i++; while (arr[i] < a);
                do j--; while (arr[j] > a);
                if (j < i) break;
                SWAP(arr[i],arr[j]);
            }
            arr[l+1]=arr[j];
            arr[j]=a;
            if (j >= k) ir=j-1;
            if (j <= k) l=i;
        }
    }
}

double whimed(double *a, int *w, int n,
              double* a_cand, int* w_cand)
{

/*
  Algorithm to compute the weighted high median in O(n) time.

  The whimed is defined as the smallest a[j] such that the sum
  of the weights of all a[i] <= a[j] is strictly greater than
  half of the total weight.

  Arguments:

  a: double array containing the observations
  n: number of observations
  w: array of (int/double) weights of the observations.
*/

    int n2, i, kcand;
    /* sum of weights: `int' do overflow when  n ~>= 1e5 */
    int64_t wleft, wmid, wright, w_tot, wrest;

    double trial;

    w_tot = 0;
    for (i = 0; i < n; ++i)
        w_tot += w[i];
    wrest = 0;

/* REPEAT : */
    do {
//        for (i = 0; i < n; ++i)
//            a_srt[i] = a_cpy[i];
        n2 = n/2;/* =^= n/2 + 1 with 0-indexing */
        trial =  qselect(a, n, n2);

        wleft = 0;    wmid  = 0;    wright= 0;
        for (i = 0; i < n; ++i) {
            if (a[i] < trial)
                wleft += w[i];
            else if (a[i] > trial)
                wright += w[i];
            else
                wmid += w[i];
        }
        /* wleft = sum_{i; a_cpy[i]	 < trial}  w[i]
         * wmid	 = sum_{i; a_cpy[i] == trial}  w[i] at least one 'i' since trial is one a_cpy[]!
         * wright= sum_{i; a_cpy[i]	 > trial}  w[i]
         */
        kcand = 0;
        if (2 * (wrest + wleft) > w_tot) {
            for (i = 0; i < n; ++i) {
                if (a[i] < trial) {
                    a_cand[kcand] = a[i];
                    w_cand[kcand] = w[i];
                    ++kcand;
                }
            }
        }
        else if (2 * (wrest + wleft + wmid) <= w_tot) {
            for (i = 0; i < n; ++i) {
                if (a[i] > trial) {
                    a_cand[kcand] = a[i];
                    w_cand[kcand] = w[i];
                    ++kcand;
                }
            }
            wrest += wleft + wmid;
        }
        else {
            return trial;
        }
        n = kcand;
        for (i = 0; i < n; ++i) {
            a[i] = a_cand[i];
            w[i] = w_cand[i];
        }
    } while(1);

} /* _WHIMED_ */

int mpoints_cmp_fn(const void *a, const void *b) {
    M_point *p1 = (M_point*)a;
    M_point *p2 = (M_point*)b;
    if (p1->value < p2->value) return -1;
    if (p1->value > p2->value) return 1;
    if (p1->value == p2->value) {
        if (p1->x_idx < p2->x_idx) return -1;
        if (p1->x_idx > p2->x_idx) return 1;
        if (p2->x_idx == p2->x_idx) {
            if (p1->y_idx < p2->y_idx) return -1;
            else return 1;
        }
    }
}

BOSNode* compute_qn0_offline(BOSTree *X, BOSTree *Y, BOSTree *B, int n, double *qnvalue)
{
/*----------------------------------------------------------------

   Efficient algorithm for the scale estimator:

       Q*_n = { |x_i - x_j|; i<j }_(k)	[= Qn without scaling ]

		i.e. the k-th order statistic of the |x_i - x_j|

   Parameters of the function Qn :
       X  : AVL tree containing the observations
       Y  : AVL tree containing the observations with negated value
       B  : AVL tree containing n elements of set M=X+Y centered in the qn statistics
       n  : number of observations (n >=2)
*/

    double *work   = (double *)calloc(n, sizeof(double));
    double *a_cand = (double *)calloc(n, sizeof(double));

    int *left	  = (int *)calloc(n, sizeof(int));
    int *right	  = (int *)calloc(n, sizeof(int));
    int *p	  = (int *)calloc(n, sizeof(int));
    int *q	  = (int *)calloc(n, sizeof(int));
    int *weight	  = (int *)calloc(n, sizeof(int));

    double x[n];
    double y[n];

    M_point *r = (M_point*)malloc(3*sizeof(M_point));
    BOSNode *qn0_node = NULL;

    double trial;/* -Wall */
    bool found;

    int h, i, j,jj,jh;
    /* Following should be `long long int' : they can be of order n^2 */
    int64_t knew, nl,nr, sump,sumq;

    for (int i = 0; i < n; i++) {
        BOSNode *xi_node = bostree_select(X, i);
        BOSNode *yi_node = bostree_select(Y, i);
        x[i] = *((double *) xi_node->key);
        y[i] = *((double *) yi_node->key);
    }

    int64_t k0;
    int64_t kv[3];

    h = n / 2 + 1;
    k0 = (int64_t) h * (h - 1) / 2;

    kv[0] = k0;
    kv[1] = k0 - (n - 1) / 2;
    kv[2] = k0 + n / 2;

    for (int c = 0; c < 3; c++) {
        int64_t k = kv[c];

        for (i = 0; i < n; ++i) {
            left[i] = n - i + 1;
            right[i] = (i <= h) ? n : n - (i - h);
            /* the n - (i-h) is from the paper; original code had `n' */
        }
        nl = (int64_t) n * (n + 1) / 2;
        nr = (int64_t) n * n;
        knew = k + nl;/* = k + (n+1 \over 2) */
        found = false;

        while (!found && nr - nl > n) {
            j = 0;
            /* Truncation to float :
               try to make sure that the same values are got later (guard bits !) */

            for (i = 1; i < n; ++i) { /* i = 1: first row of X+Y has elements with negative or zero value */
                if (left[i] <= right[i]) {
                    weight[j] = right[i] - left[i] + 1;
                    jh = left[i] + weight[j] / 2; /* left and right contain y index in 1-indexing form */
                    work[j] =  (x[i] + y[jh - 1]);  /* jh-1: x and y are 0-indexed */
                    ++j;
                }
            }
            trial = whimed(work, weight, j, a_cand, /*iw_cand*/ p);

            j = 0;
            for (i = n - 1; i >= 0; --i) {
                while (j < n && ( (x[i] + y[j])) < trial)
                    ++j;
                p[i] = j; /* j == number of elements < trial on row i and index (0-indexing) of first element >= trial on row i */
            }

            j = n + 1;
            for (i = 0; i < n; ++i) {
                while (j > 1 &&  (x[i] + y[j - 2]) > trial)
                    --j;
                q[i] = j;   /* j-1 == number of elements <= trial on row i and index (0-indexing) of first element > trial in row i*/
            }
            sump = 0;
            sumq = 0;
            for (i = 0; i < n; ++i) {
                sump += p[i]; /* total number of elements < trial in X+Y */
                sumq += q[i] - 1; /* total number of elements <= trial in X+Y */
            }

            if (knew <= sump) {
                for (i = 0; i < n; ++i)
                    right[i] = p[i]; /* index (1-indexing) of last element < trial on row i */
                nr = sump;

            } else if (knew > sumq) {
                for (i = 0; i < n; ++i)
                    left[i] = q[i]; /* index (1-indexing) of first element > trial in row i */
                nl = sumq;

            } else { /* sump < knew <= sumq */
                found = true;
            }
        } /* while */

        if (found) {
            knew -= sump;
            j = 0;
            for (i = 0; i < n; i++) {
                left[i] = p[i];
                right[i] = q[i]-1;
                if (left[i] < right[i]) {
                    int l = right[i] - left[i];
                    if (j+l >= knew) {
                        r[c].x_idx = i;
                        r[c].y_idx = left[i] + knew - j - 1;
                        break;
                    } else {
                        j = j + l;
                    }
                }
            }
            r[c].value = trial;
        } else {
            M_point work_points[n];
            j = 0;
            for (i = 0; i < n; ++i) {
                for (jj = left[i]; jj <= right[i]; ++jj) {
                    work_points[j].value = (x[i] + y[jj - 1]);
                    work_points[j].x_idx = i;
                    work_points[j].y_idx = jj-1;
                    j++;
                }/* j will be = sum_{i=2}^n (right[i] - left[i] + 1)_{+}  */
            }
            knew -= (nl + 1); /* -1: 0-indexing */
            qsort(work_points, j, sizeof(M_point), mpoints_cmp_fn);
            r[c] = work_points[knew];
        }
    }

    // determine the elements of X+Y to be added to B
    j = 0;
    for (i = n - 1; i >= 0; --i) {
        while (j < n && ((x[i] + y[j])) < r[1].value)
            ++j;
        p[i] = j; /* j == number of elements < kth[1] on row i and index (0-indexing) of first element >= kth[1] on row i */
    }

    j = n + 1;
    for (i = 0; i < n; ++i) {
        while (j > 1 &&  (x[i] + y[j - 2]) > r[2].value)
               --j;
        q[i] = j-1; /* j-1 == number of elements <= kth[2] on row i and index (0-indexing) of first element > kth[2] in row i */
    }

    int Bsize = 0;
    for(int i= 0; i< n; ++i){
        Bsize += q[i] - p[i];
    }//for
    
    if (Bsize > 2*n){
        *qnvalue = r[0].value;
        return NULL;
    }


    // fill B avl tree

    BOSNode *xbs[n];
    BOSNode *xbg[n];
    BOSNode *ybs[n];
    BOSNode *ybg[n];
    BNode *bnode;

    for (int i = 0; i < n; i++) {
        if (p[i] <= q[i]-1) {
            for (int j = p[i]; j < q[i]; j++) {
                bnode = (BNode *) malloc(sizeof(BNode));
                bnode->value = (x[i] + y[j]);
                bnode->toX = bostree_select(X, i);
                bnode->toY = bostree_select(Y, j);
                BOSNode *b = bostree_insert(B, (void *)bnode, NULL);
                
                if (r[0].value == bnode->value && r[0].x_idx == i && r[0].y_idx == j) { // store pointer to qn0 in B
                    qn0_node = b;
                }
                if (j == p[i]) xbs[i] = b;
                if (j == q[i] - 1) xbg[i] = b;
            }
        } else {
            xbs[i] = NULL;
            xbg[i] = NULL;
        }
    }

    for (int j = 0; j < n; j++) {
        int imax = -1;
        int imin = n+1;
        for (int i = n-1; i >= 0; i--) {
            if ( j >= p[i] && j < q[i]) {
                if (i < imin) imin = i;
                if (i > imax) imax = i;
            }
        }
        if (imin <= imax) {
            BNode key;
            key.value = x[imin]+y[j];
            key.toX = bostree_select(X, imin);
            key.toY = bostree_select(Y, j);
            ybs[j] = bostree_lookup(B, (void *) &key);
#ifdef ENABLE_TEST
            assert(ybs[j]);
#endif
            key.value = x[imax] + y[j];
            key.toX = bostree_select(X, imax);
            key.toY = bostree_select(Y, j);
            ybg[j] = bostree_lookup(B, (void *) &key);
#ifdef ENABLE_TEST
            assert(ybg[j]);
#endif
        } else {
            ybs[j] = NULL;
            ybg[j] = NULL;
        }
    }

    for (int ij = 0; ij < n; ij++) {
        BOSNode *x = bostree_select(X, ij);
        BOSNode *y = bostree_select(Y, ij);
        NodeData *x_data = (NodeData *)x->data;
        NodeData *y_data = (NodeData *)y->data;
        x_data->bs = xbs[ij];
        x_data->bg = xbg[ij];
        y_data->bs = ybs[ij];
        y_data->bg = ybg[ij];
    }

    free(work);
    free(a_cand);

    free(left);
    free(right);
    free(p);
    free(q);
    free(weight);

    return qn0_node;
} /* qn0c */




double qn0(double *x, int n)
{
/*----------------------------------------------------------------

   Efficient algorithm for the scale estimator:

       Q*_n = { |x_i - x_j|; i<j }_(k)	[= Qn without scaling ]

		i.e. the k-th order statistic of the |x_i - x_j|

   Parameters of the function Qn :
       x  : double array containing the observations
       n  : number of observations (n >=2)
 */

    double *y	   = (double *)calloc(n, sizeof(double));
    double *work   = (double *)calloc(n, sizeof(double));
    double *a_srt  = (double *)calloc(n, sizeof(double));
    double *a_cand = (double *)calloc(n, sizeof(double));

    int *left	  = (int *)calloc(n, sizeof(int));
    int *right	  = (int *)calloc(n, sizeof(int));
    int *p	  = (int *)calloc(n, sizeof(int));
    int *q	  = (int *)calloc(n, sizeof(int));
    int *weight	  = (int *)calloc(n, sizeof(int));

    double trial;/* -Wall */
    double qn0;
    bool found;

    int h, i, j,jj,jh;
    /* Following should be `long long int' : they can be of order n^2 */
    int64_t k, knew, nl,nr, sump,sumq;

    h = n / 2 + 1;
    k = (int64_t)h * (h - 1) / 2;

    for (i = 0; i < n; ++i) {
        y[i] = x[i];
        left [i] = n - i + 1;
        right[i] = (i <= h) ? n : n - (i - h);
        /* the n - (i-h) is from the paper; original code had `n' */
    }
    quick_sort(y, n); /* y := sort(x) */
    nl = (int64_t)n * (n + 1) / 2;
    nr = (int64_t)n * n;
    knew = k + nl;/* = k + (n+1 \over 2) */
    found = false;

    while(!found && nr - nl > n) {
        j = 0;
        /* Truncation to float :
           try to make sure that the same values are got later (guard bits !) */
        for (i = 1; i < n; ++i) {
            if (left[i] <= right[i]) {
                weight[j] = right[i] - left[i] + 1;
                jh = left[i] + weight[j] / 2;
                work[j] = (y[i] - y[n - jh]);
                ++j;
            }
        }
        trial = whimed(work, weight, j, a_cand, /*iw_cand*/ p);

        j = 0;
        for (i = n - 1; i >= 0; --i) {
            while (j < n && ((y[i] - y[n - j - 1])) < trial)
                ++j;
            p[i] = j;
        }

        j = n + 1;
        for (i = 0; i < n; ++i) {
            while ((y[i] - y[n - j + 1]) > trial)
                --j;
            q[i] = j;
        }
        sump = 0;
        sumq = 0;
        for (i = 0; i < n; ++i) {
            sump += p[i];
            sumq += q[i] - 1;
        }

        if (knew <= sump) {
            for (i = 0; i < n; ++i)
                right[i] = p[i];
            nr = sump;
        } else if (knew > sumq) {
            for (i = 0; i < n; ++i)
                left[i] = q[i];
            nl = sumq;
        } else { /* sump < knew <= sumq */
            found = true;
        }
    } /* while */

    if (found)
        qn0 = trial;
    else {
        j = 0;
        for (i = 1; i < n; ++i) {
            for (jj = left[i]; jj <= right[i]; ++jj) {
                work[j] = y[i] - y[n - jj];
                j++;
            }/* j will be = sum_{i=2}^n (right[i] - left[i] + 1)_{+}  */
        }
        /* return pull(work, j - 1, knew - nl)	: */
        knew -= (nl + 1); /* -1: 0-indexing */
        qn0 = qselect(work, j, knew);
    }

    free(y);
    free(work);
    free(a_srt);
    free(a_cand);

    free(left);
    free(right);
    free(p);
    free(q);
    free(weight);

    return qn0;
} /* qn0 */

double qn(double *x, int n, int finite_corr)
{
/* Efficient algorithm for the scale estimator:

	Qn = dn * 2.2219 * {|x_i-x_j|; i<j}_(k)
*/
    double r, dn = 1./*-Wall*/;

    r = 2.2219 * qn0(x, n); /* asymptotic consistency for sigma^2 */
    /*		 === */

    if (finite_corr) {
        if (n <= 9) {
            if	    (n == 2)	dn = .399;
            else if (n == 3)	dn = .994;
            else if (n == 4)	dn = .512;
            else if (n == 5)	dn = .844;
            else if (n == 6)	dn = .611;
            else if (n == 7)	dn = .857;
            else if (n == 8)	dn = .669;
            else if (n == 9)	dn = .872;
        } else {
            if (n % 2 == 1)
                dn = n / (n + 1.4);
            else /* (n % 2 == 0) */
                dn = n / (n + 3.8);
        }
        return dn * r;
    }
    else return r;
} /* qn */

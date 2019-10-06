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

#include "qnonline.h"
#include <math.h>

#ifdef ENABLE_TEST
void print_X_plus_Y (BOSTree *X, BOSTree *Y){
    int nx = bostree_node_count(X);
    int ny = bostree_node_count(Y);
    printf("\nX+Y:");
    for (int i = 0; i < nx; i++) {
        printf("\n");
        BOSNode *x_node = bostree_select(X, i);
        for (int j = 0; j < ny; j++) {
            BOSNode *y_node = bostree_select(Y, j);
            printf("%f\t", *(double*)x_node->key + *(double*)y_node->key);
        }
    }
    printf("\n");
}

void print_X (BOSTree *X){
    int nx = bostree_node_count(X);
    printf("\nX: ");
    for (int i = 0; i < nx; i++) {
        BOSNode *x_node = bostree_select(X, i);
        printf("%f\t", *(double*)x_node->key);
    }
    printf("\n");
}

void print_Y (BOSTree *X){
    int nx = bostree_node_count(X);
    printf("\nY: ");
    for (int i = 0; i < nx; i++) {
        BOSNode *x_node = bostree_select(X, i);
        printf("%f\t", *(double*)x_node->key);
    }
    printf("\n");
}

void print_B (BOSTree *B){
    int nx = bostree_node_count(B);
    printf("\nB: ");
    for (int i = 0; i < nx; i++) {
        BOSNode *b_node = bostree_select(B, i);
        printf("%f (%d, %d)\t", ((BNode*)b_node->key)->value, bostree_rank(((BNode*)b_node->key)->toX), bostree_rank(((BNode*)b_node->key)->toY));
    }
    printf("\n");
}

int test_verify_pointers(BOSTree *X, BOSTree *Y, BOSTree *B, BOSNode *qn0, bool delete_qn) {
    int nx = bostree_node_count(X);
    int ny = bostree_node_count(Y);
    int nb = bostree_node_count(B);

    BOSNode *x_nodes_in_B[nb];
    BOSNode *y_nodes_in_B[nb];

    for (int i = 0; i < nb; i++) {
        BOSNode *bnode = bostree_select(B, i);
        if (delete_qn && bnode == qn0) {
            x_nodes_in_B[i] = NULL;
            y_nodes_in_B[i] = NULL;
        } else {
            BOSNode *xnode = ((BNode *) bnode->key)->toX;
            BOSNode *ynode = ((BNode *) bnode->key)->toY;
            assert(xnode != NULL);
            assert(ynode != NULL);
            x_nodes_in_B[i] = xnode;
            y_nodes_in_B[i] = ynode;
        }
    }

    for (int i = 0; i < nx; i++) {
        BOSNode *xnode = bostree_select(X, i);
        BOSNode *bsnode = ((NodeData*)xnode->data)->bs;
        BOSNode *bgnode = ((NodeData*)xnode->data)->bg;

        bool found = false;
        for (int j = 0; j < nb; j++) {
            if (x_nodes_in_B[j] == xnode) {
                assert(bsnode != NULL);
                assert(bgnode != NULL);
                found = true;
                break;
            }
        }
        if (!found) {
            assert(bsnode == NULL);
            assert(bgnode == NULL);
        }
    }

    for (int i = 0; i < ny; i++) {
        BOSNode *ynode = bostree_select(Y, i);
        BOSNode *bsnode = ((NodeData*)ynode->data)->bs;
        BOSNode *bgnode = ((NodeData*)ynode->data)->bg;

        bool found = false;
        for (int j = 0; j < nb; j++) {
            if (y_nodes_in_B[j] == ynode) {
                assert(bsnode != NULL);
                assert(bgnode != NULL);
                found = true;
                break;
            }
        }
        if (!found) {
            assert(bsnode == NULL);
            assert(bgnode == NULL);
        }
    }

    return 0;
}
#endif

int cmp_fn_B(const void *a, const void *b) {
    const BNode *anode = (const BNode *) a;
    const BNode *bnode = (const BNode *) b;

    if (anode->value == bnode->value) {
        if (bostree_rank(anode->toX) < bostree_rank(bnode->toX)) return -1;
        if (bostree_rank(anode->toX) > bostree_rank(bnode->toX)) return 1;
        if (bostree_rank(anode->toX) == bostree_rank(bnode->toX)) {
            if (bostree_rank(anode->toY) < bostree_rank(bnode->toY)) return -1;
            if (bostree_rank(anode->toY) > bostree_rank(bnode->toY)) return 1;
            if (bostree_rank(anode->toY) == bostree_rank(bnode->toY)) return 0;
        }
    }
    else if (anode->value < bnode->value) return -1;
    else if (anode->value > bnode->value) return 1;
}

int insert_x_item(double xnew, long seq_no, BOSTree *X, BOSTree *Y, BOSTree *B){
    double bs, bg, search_key;

    double *xnode_key = (double *)malloc(sizeof(double));
    NodeData *xnode_data = (NodeData *)malloc(sizeof(NodeData));
    *xnode_key = xnew;
    xnode_data->bs = NULL;
    xnode_data->bg = NULL;
    xnode_data->seqNo = seq_no;

    BOSNode *x_node = bostree_insert(X, (void *)xnode_key, (void *)xnode_data);

    int count = bostree_node_count(B);
    if (count == 0) return 0; // B is empty

    BOSNode *bs_node = bostree_select(B, 0);
    BOSNode *bg_node = bostree_select(B, count-1);
    bs = ((BNode *)bs_node->key)->value;
    bg = ((BNode *)bg_node->key)->value;


    int x_rank = bostree_rank(x_node);

    search_key = bs - xnew;
    int j = search_for_j(Y, (void *)&search_key);
    search_key = bg - xnew;
    int l = search_for_l(Y, (void *)&search_key);

#ifdef ENABLE_TEST
    printf("\nInserting x = %f with bs = %f and bg = %f: j = %d, l = %d\n", xnew, bs, bg, j, l);
#endif

    for (int jj = j; jj <=l; jj++) {
        BNode *b_key = (BNode*)malloc(sizeof(BNode));
        BOSNode *y_node = bostree_select(Y, jj);
        b_key->value = xnew + *(double*)y_node->key;
        b_key->toX = x_node;
        b_key->toY = y_node;
        BOSNode *b_node = bostree_insert(B, (void*)b_key, NULL);
        // update x_node->data
        if (jj == j) xnode_data->bs = b_node;
        if (jj == l) xnode_data->bg = b_node;

        // update y_node->data
        BOSNode *b_col_min = ((NodeData *)y_node->data)->bs;
        BOSNode *b_col_max = ((NodeData *)y_node->data)->bg;
        if (b_col_min == NULL) { // y_jj did not contribute to any value in B
            ((NodeData *)y_node->data)->bs = b_node;
            ((NodeData *)y_node->data)->bg = b_node;
        } else {
            int col_min = bostree_rank((((BNode *) b_col_min->key)->toX));
            int col_max = bostree_rank((((BNode *) b_col_max->key)->toX));
            if (x_rank < col_min) { // new row inserted before the first row in column jj
                ((NodeData *)y_node->data)->bs = b_node;
            }
            if (x_rank > col_max) { //new row inserted after the last row in column jj
                ((NodeData *)y_node->data)->bg = b_node;
            }
        }
    }

    int num_items_before_bs = j;
    int num_left_shifts = num_items_before_bs;// + num_items_before_qn0;
    return num_left_shifts;
}

int delete_x_item(double xdel, BOSTree *X, BOSTree *Y, BOSTree *B){

    int n = bostree_node_count(Y);
    BOSNode *xdel_node = bostree_lookup(X, (void *)&xdel);
    int xdel_rank = bostree_rank(xdel_node);

    int Bcount = bostree_node_count(B);
    if (Bcount == 0) {
        bostree_remove(X, xdel_node);
        return 0;
    }

    BOSNode *bs_node = ((NodeData*)xdel_node->data)->bs;
    BOSNode *bg_node = ((NodeData*)xdel_node->data)->bg;
    int num_items_before_bs = 0;

    if (bs_node != NULL && bg_node != NULL) {
        BOSNode *y_j_node = ((BNode *) bs_node->key)->toY;
        BOSNode *y_l_node = ((BNode *) bg_node->key)->toY;
        int j = bostree_rank(y_j_node);
        int l = bostree_rank(y_l_node);

#ifdef ENABLE_TEST
        printf("\nDeleting x = %f with j = %d, l = %d\n", xdel, j, l);
#endif
        for (int jj = j; jj<=l; jj++) {
            BOSNode *y_jj_node = bostree_select(Y, jj);
            BNode b_del;
            b_del.value = xdel + *(double*)y_jj_node->key;
            b_del.toX = xdel_node;
            b_del.toY = y_jj_node;
            BOSNode *bdel_node = bostree_lookup(B, (const void *)&b_del);
#ifdef ENABLE_TEST
            assert(bdel_node);
#endif
            BOSNode *y_bs = ((NodeData*)y_jj_node->data)->bs;
            BOSNode *y_bg = ((NodeData*)y_jj_node->data)->bg;

            int bs_x_rank = bostree_rank(((BNode*)y_bs->key)->toX);
            int bg_x_rank = bostree_rank(((BNode*)y_bg->key)->toX);

            if (bs_x_rank == bg_x_rank && bs_x_rank == xdel_rank) {
                ((NodeData*)y_jj_node->data)->bs = NULL;
                ((NodeData*)y_jj_node->data)->bg = NULL;
            }
            if (bs_x_rank < bg_x_rank) {
                if (bs_x_rank == xdel_rank) {
                    BNode bs_new;
                    BOSNode *bs_new_node = NULL;
                    int rank = bs_x_rank;
                    while(bs_new_node == NULL && rank <= bg_x_rank) {
                        rank++;
                        BOSNode *bs_new_xnode = bostree_select(X, rank);
                        bs_new.value = *(double *) bs_new_xnode->key + *(double *) y_jj_node->key; // verify
                        bs_new.toX = bs_new_xnode;
                        bs_new.toY = y_jj_node;
                        bs_new_node = bostree_lookup(B, (void const *) &bs_new);
                    }
                    ((NodeData*)y_jj_node->data)->bs = bs_new_node;
                }
                if (bg_x_rank == xdel_rank) {
                    BNode bg_new;
                    BOSNode *bg_new_node = NULL;
                    int rank = bg_x_rank;
                    while(bg_new_node == NULL && rank >= bs_x_rank) {
                        rank--;
                        BOSNode *bg_new_xnode = bostree_select(X, rank);
                        bg_new.value = *(double *) bg_new_xnode->key + *(double *) y_jj_node->key; // verify
                        bg_new.toX = bg_new_xnode;
                        bg_new.toY = y_jj_node;
                        bg_new_node = bostree_lookup(B, (void const *) &bg_new);
                    }
                    ((NodeData*)y_jj_node->data)->bg = bg_new_node;
                }
            }

            bostree_remove(B, bdel_node);
        }
        num_items_before_bs = j;
    } else {
        bs_node = bostree_select(B, 0);
        //double bs_value = ((BNode*)bs_node->key)->value;
        BNode row_right_bnode;
        //double right_value = xdel + *(double*)bostree_select(Y, n-1)->key;
        row_right_bnode.toX = xdel_node;
        row_right_bnode.toY = bostree_select(Y, n-1);
        row_right_bnode.value = xdel + *(double*)((row_right_bnode.toY)->key);
        if (cmp_fn_B(bs_node->key, &row_right_bnode) >= 0) { //(right_value <= bs_value) {
            num_items_before_bs = n;
        } else {
            int jj = n - xdel_rank; // index of first item >= 0 in the xdel row
            //double curr_value = xdel + *(double *) bostree_select(Y, jj)->key;
            BNode curr_bnode;
            curr_bnode.toX = xdel_node;
            curr_bnode.toY = bostree_select(Y, jj);
            curr_bnode.value = xdel + *(double*)((curr_bnode.toY)->key);
            while ((cmp_fn_B(bs_node->key, &curr_bnode) >= 0) && (jj < n-1)) { //curr_value <= bs_value
                jj++;
                curr_bnode.toY =  bostree_select(Y, jj);
                curr_bnode.value = xdel + *(double*)((curr_bnode.toY)->key);
            }
            num_items_before_bs = jj;
        }
    }
    bostree_remove(X, xdel_node);

    int num_right_shifts = num_items_before_bs;
    return num_right_shifts;
}

int insert_y_item(double ynew, long seq_no, BOSTree *X, BOSTree *Y, BOSTree *B) {
    double bs, bg, search_key;

    double *y_node_key = (double *)malloc(sizeof(double));
    NodeData *y_node_data = (NodeData *)malloc(sizeof(NodeData));
    *y_node_key = ynew;
    y_node_data->bs = NULL;
    y_node_data->bg = NULL;
    y_node_data->seqNo = seq_no;

    BOSNode *y_node = bostree_insert(Y, (void *)y_node_key, (void *)y_node_data);

    int count = bostree_node_count(B);
    if (count == 0) return 0; // B is empty

    int y_rank = bostree_rank(y_node);

    BOSNode *bs_node = bostree_select(B, 0);
    BOSNode *bg_node = bostree_select(B, count-1);
    bs = ((BNode *)bs_node->key)->value;
    bg = ((BNode *)bg_node->key)->value;

    search_key = bs - ynew;
    int j = search_for_j(X, (void *)&search_key);
    search_key = bg - ynew;
    int l = search_for_l(X, (void *)&search_key);

#ifdef ENABLE_TEST
    printf("\nInserting y = %f with bs = %f and bg = %f: j = %d, l = %d\n", ynew, bs, bg, j, l);
#endif

    for (int i = j; i <=l; i++) {
        BNode *b_key = (BNode*)malloc(sizeof(BNode));
        BOSNode *x_node = bostree_select(X, i);
        b_key->value = *(double*)x_node->key + ynew;
        b_key->toX = x_node;
        b_key->toY = y_node;
        BOSNode *b_node = bostree_insert(B, (void*)b_key, NULL);

        // update y_node->data
        if (i == j) ((NodeData*)y_node->data)->bs = b_node;
        if (i == l) ((NodeData*)y_node->data)->bg = b_node;

        // update x_node->data
        BOSNode *x_bs_node = ((NodeData *)x_node->data)->bs;
        BOSNode *x_bg_node = ((NodeData *)x_node->data)->bg;
        if (x_bs_node == NULL) { // x_i did not contribute to any value in B
            ((NodeData *)x_node->data)->bs = b_node;
            ((NodeData *)x_node->data)->bg = b_node;
        } else {
            int x_bs_yrank = bostree_rank((((BNode *) x_bs_node->key)->toY));
            int x_bg_yrank = bostree_rank((((BNode *) x_bg_node->key)->toY));
            if (y_rank < x_bs_yrank) { // new column inserted before the first column in row i
                ((NodeData *)x_node->data)->bs = b_node;
            }
            if (y_rank > x_bg_yrank) { //new column inserted after the last column in column i
                ((NodeData *)x_node->data)->bg = b_node;
            }
        }
    }

    int num_items_before_bs = j;
    int num_left_shifts = num_items_before_bs;

    return num_left_shifts;
}

int delete_y_item(double ydel, BOSTree *X, BOSTree *Y, BOSTree *B){

    int n = bostree_node_count(X);
    int num_items_before_bs = 0;

    BOSNode *y_del_node = bostree_lookup(Y, (void *)&ydel);
    int Bcount = bostree_node_count(B);
    if (Bcount == 0) {
        bostree_remove(Y, y_del_node);
        return 0;
    }

    int y_del_rank = bostree_rank(y_del_node);

    BOSNode *bs_node = ((NodeData*)y_del_node->data)->bs;
    BOSNode *bg_node = ((NodeData*)y_del_node->data)->bg;

    if (bs_node != NULL && bg_node != NULL) {
        BOSNode *x_j_node = ((BNode*)bs_node->key)->toX;
        BOSNode *x_l_node = ((BNode*)bg_node->key)->toX;
        int j = bostree_rank(x_j_node);
        int l = bostree_rank(x_l_node);

#ifdef ENABLE_TEST
        printf("\nDeleting y = %f with j = %d, l = %d\n", ydel, j, l);
#endif

        for (int i = j; i <= l; i++) {
            BOSNode *x_i_node = bostree_select(X, i);
            BNode b_del;
            b_del.value = *(double *) x_i_node->key + ydel;
            b_del.toX = x_i_node;
            b_del.toY = y_del_node;
            BOSNode *b_del_node = bostree_lookup(B, (const void *) &b_del);

#ifdef ENABLE_TEST
            assert(b_del_node);
#endif

            BOSNode *x_bs_node = ((NodeData *) x_i_node->data)->bs;
            BOSNode *x_bg_node = ((NodeData *) x_i_node->data)->bg;

            int x_bs_y_rank = bostree_rank(((BNode *) x_bs_node->key)->toY);
            int x_bg_y_rank = bostree_rank(((BNode *) x_bg_node->key)->toY);

            if (x_bs_y_rank == x_bg_y_rank && x_bs_y_rank == y_del_rank) {
                ((NodeData *) x_i_node->data)->bs = NULL;
                ((NodeData *) x_i_node->data)->bg = NULL;
            }

            if (x_bs_y_rank < x_bg_y_rank) {
                if (x_bs_y_rank == y_del_rank) {
                    BNode bs_new;
                    BOSNode *bs_new_node = NULL;
                    int rank = x_bs_y_rank;
                    while(bs_new_node == NULL && rank <= x_bg_y_rank) {
                        rank++;
                        BOSNode *new_bs_y_node = bostree_select(Y, rank);
                        bs_new.value = *(double *) x_i_node->key + *(double *) new_bs_y_node->key;
                        bs_new.toX = x_i_node;
                        bs_new.toY = new_bs_y_node;
                        bs_new_node = bostree_lookup(B, (void const *) &bs_new);
                    }
                    ((NodeData *) x_i_node->data)->bs = bs_new_node;
                }
                if (x_bg_y_rank == y_del_rank) {
                    BNode bg_new;
                    BOSNode *bg_new_node = NULL;
                    int rank = x_bg_y_rank;
                    while(bg_new_node == NULL && rank >= x_bs_y_rank) {
                        rank--;
                        BOSNode *new_bg_y_node = bostree_select(Y, rank);
                        bg_new.value = *(double *) x_i_node->key + *(double *) new_bg_y_node->key;
                        bg_new.toX = x_i_node;
                        bg_new.toY = new_bg_y_node;
                        bg_new_node = bostree_lookup(B, (void const *) &bg_new);
                    }
                    ((NodeData *) x_i_node->data)->bg = bg_new_node;
                }
            }

            bostree_remove(B, b_del_node);
        }
        num_items_before_bs = j;
    } else {
        bs_node = bostree_select(B, 0);
        //double bs_value = ((BNode*)bs_node->key)->value;
        BNode max_col_bnode;
        max_col_bnode.toY = y_del_node;
        max_col_bnode.toX = bostree_select(X, n-1);
        max_col_bnode.value = ydel + *(double*)(max_col_bnode.toX)->key;
        //double max_col_value = ydel + *(double*)bostree_select(X, n-1)->key;
        if (cmp_fn_B(bs_node->key, &max_col_bnode)>=0) {//(max_col_value <= bs_value) {
            num_items_before_bs = n;
        } else {
            int ii = n - y_del_rank; // row index of first item >= 0 in the ydel col
            //double curr_value = ydel + *(double *) bostree_select(X, ii)->key;
            BNode curr_bnode;
            curr_bnode.toX = bostree_select(X, ii);
            curr_bnode.toY = y_del_node;
            curr_bnode.value = ydel + *(double*)(curr_bnode.toX)->key;

            while ((cmp_fn_B(bs_node->key, &curr_bnode)>=0) && (ii < n-1)) { //curr_value <= bs_value
                ii++;
                curr_bnode.toX = bostree_select(X, ii);
                curr_bnode.value = ydel + *(double*)(curr_bnode.toX)->key;
            }
            num_items_before_bs = ii;
        }
    }

    bostree_remove(Y, y_del_node);
    int num_right_shifts = num_items_before_bs;
    return num_right_shifts;
}

BOSNode *update_window(double xnew, int seqno, double xdel, BOSTree *X, BOSTree *Y, BOSTree *B, BOSNode* qn0_node) {
    int num_shifts = 0;
    int num_left_shifts = 0;
    int num_right_shifts = 0;

    int qn0_rank = bostree_rank(qn0_node);

#ifdef ENABLE_TEST
    assert(!test_verify_pointers(X, Y, B, NULL, false));
    print_X(X);
    print_Y(Y);
    print_B(B);
    print_X_plus_Y(X, Y);
#endif

    num_left_shifts = insert_x_item(xnew, seqno, X, Y, B);
    num_shifts = num_shifts - num_left_shifts;

#ifdef ENABLE_TEST
    printf("\n\nInsert x shifts: %d\n\n", num_left_shifts);
    assert(!test_verify_pointers(X, Y, B, NULL, false));
    print_X(X);
    print_Y(Y);
    print_B(B);
    print_X_plus_Y(X, Y);
#endif

    num_right_shifts = delete_x_item(xdel, X, Y, B);
    num_shifts = num_shifts + num_right_shifts;

#ifdef ENABLE_TEST
    printf("\n\nDelete x shifts: %d\n\n", num_right_shifts);
    print_X(X);
    print_Y(Y);
    print_B(B);
    print_X_plus_Y(X, Y);
    if (bostree_node_count(B) > 0)
        assert(!test_verify_pointers(X, Y, B, NULL, false));
#endif

    num_left_shifts = insert_y_item(-xnew, seqno, X, Y, B);
    num_shifts = num_shifts - num_left_shifts;

#ifdef ENABLE_TEST
    printf("\n\nInsert y shifts: %d\n\n", num_left_shifts);
    if (bostree_node_count(B) > 0)
        assert(!test_verify_pointers(X, Y, B, NULL, false));
    print_X(X);
    print_Y(Y);
    print_B(B);
    print_X_plus_Y(X, Y);
#endif

    num_right_shifts = delete_y_item(-xdel, X, Y, B);
    num_shifts = num_shifts + num_right_shifts;

#ifdef ENABLE_TEST
    printf("\n\nDelete y shifts: %d\n\n", num_right_shifts);
    print_Y(X);
    print_Y(Y);
    print_B(B);
    print_X_plus_Y(X, Y);
    printf("\n\nTotal number of shifts: %d\n\n", num_shifts);
#endif

    int qn0_new_rank =  qn0_rank + num_shifts;

#ifdef ENABLE_TEST
    printf("Old rank = %d, New rank = %d\n\n", qn0_rank, qn0_new_rank);
    print_Y(X);
    print_Y(Y);
    print_B(B);
    print_X_plus_Y(X, Y);
    if (bostree_node_count(B) > 0)
        assert(!test_verify_pointers(X, Y, B, NULL, false));
#endif

    int n = bostree_node_count(B);
    if (qn0_new_rank < 0 || qn0_new_rank > n-1) { // qn0 is not in the buffer
        return NULL;
    }
    else {
        BOSNode *qn0_new = bostree_select(B, qn0_new_rank);
        return qn0_new;
    }
}

int cmp_fn_XY(const void* a, const void* b) {
    const double *avalue = (const double *)a;
    const double *bvalue = (const double *)b;
    if (*avalue == *bvalue) return 0;
    else if (*avalue < *bvalue) return -1;
    else if (*avalue > *bvalue) return 1;
}

void Bfree_fn(BOSNode *node) {
    free((BNode*)node->key);
}

void XYfree_fn(BOSNode *node) {
    free((double*)node->key);
    free((NodeData*)node->data);
}

void init_trees(double *items, long w, BOSTree **X, BOSTree **Y, BOSTree **B) {
    *X = bostree_new(cmp_fn_XY, XYfree_fn);
    *Y = bostree_new(cmp_fn_XY, XYfree_fn);
    *B = bostree_new(cmp_fn_B, Bfree_fn);
    for (int i = 0; i < w; i++) {
        NodeData *x_node_data = (NodeData*)malloc(sizeof(NodeData));
        double *x_node_key = (double *)malloc(sizeof(double));
        *x_node_key = items[i];
        x_node_data->seqNo = i;
        x_node_data->bs = NULL;
        x_node_data->bg = NULL;
        bostree_insert(*X, x_node_key, x_node_data);

        NodeData *y_node_data = (NodeData *)malloc(sizeof(NodeData));
        double *y_node_key = (double *)malloc(sizeof(double));
        *y_node_key = -items[i];
        y_node_data->seqNo = i;
        y_node_data->bs = NULL;
        y_node_data->bg = NULL;
        bostree_insert(*Y, y_node_key, y_node_data);
    }
}

void delete_trees(BOSTree **X, BOSTree **Y, BOSTree **B) {
    bostree_destroy(*B);
    bostree_destroy(*X);
    bostree_destroy(*Y);
    *B=NULL;
    *X=NULL;
    *Y=NULL;
}

void reset_B (BOSTree **B) {
    bostree_destroy(*B);
    *B=NULL;
    *B = bostree_new(cmp_fn_B, Bfree_fn);
}
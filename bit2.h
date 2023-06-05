/*
 *     bit2.h
 *     by Kabir Pamnani and Isaac Monheit, 02/06/2023
 *     HW2: Interfaces, Implementations and Images (iii)
 *
 *     Summary: Interface for 2D Bit Arrays 
 */

#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED
#define T Bit2_T
typedef struct T *T;


extern T Bit2_new(int width, int height);
extern void Bit2_free(T *bit2_array);
extern int Bit2_width(T bit2_array);
extern int Bit2_height(T bit2_array);
extern int Bit2_get(T bit2_array, int col, int row);
extern int Bit2_put(T bit2_array, int col, int row, int bit);
extern void Bit2_map_row_major(T bit2_array, void apply(int col, int row, 
                            T bit2_array, int bit, void *cl), void *cl);
extern void Bit2_map_col_major(T bit2_array, void apply(int col, int row, 
                            T bit2_array, int bit, void *cl), void *cl);


#undef T
#endif






/*
 *     bit2.c
 *     by Kabir Pamnani and Isaac Monheit, 02/06/2023
 *     HW2: Interfaces, Implementations and Images (iii)
 *
 *     Summary: Implementation of 2D Bit Arrays 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "bit2.h"
#include "bit.h"

#define T Bit2_T

struct T {
        Bit_T bit_array;
        int width;
        int height;
};

/**********Bit2_new********
 *
 * Creates a new vector of width x height bits and sets all the bits to zero 
 * Inputs:
 *              int width: integer storing the number of columns contained in 
 *                         the bit2 array
 *              int height: integer storing the number of rows contained in the
                            bit2 array
 * Return: A new bit2 array with width x height number of elements
 * Expects:
 *      width and height to be nonnegative
 * Notes:
 *      Checked runtime error if width or height is negative
 *      Bit2 can raise Mem_Failed if Bit2_new cannot allocate the memory
 *      requested
 ************************/
T Bit2_new(int width, int height) 
{
        T bit2_array = malloc(sizeof(*bit2_array));
        assert(bit2_array != NULL);
    
        assert(width >= 0); 
        assert(height >= 0);

        bit2_array->width = width;
        bit2_array->height = height;

        bit2_array->bit_array = Bit_new(width * height);

        return bit2_array;
}

/**********Bit2_free********
 *
 * Deallocates and clears the *bit2_array
 * Inputs:
 *              T *bit2_array: A pointer to a pointer to the bit2_array that 
 *                             will be deallocated and cleared
 * Return: A new bit2_array with width x height number of elements
 * Expects:
 *      bit2_array or *bit2_array to be nonnull
 * Notes:
 *      Checked runtime error if bit2_array / *bit2_array to be null 
 ************************/
void Bit2_free(T *bit2_array)
{
        Bit_free(&((*bit2_array)->bit_array));
        free(*bit2_array);
}

/**********Bit2_width********
 *
 * Returns the number of columns in the bit2_array
 * Inputs:
 *              T bit2_array: A pointer to the bit2_array in which the width is
 *                            to be retrieved
 * Return: the number of columns in the bit2_array (width of the bit2_array)
 * Expects:
 *       bit2_array or *bit2_array to be nonnull
 * Notes:
 *      Checked runtime error if bit2_array / *bit2_array is null
 ************************/
int Bit2_width(T bit2_array) 
{
        assert(bit2_array != NULL);
        assert(bit2_array->bit_array != NULL);
        return bit2_array->width;
}

/**********Bit2_height********
 *
 * Returns the number of rows in the bit2_array
 * Inputs:
 *              T bit2_array: A pointer to the bit2_array in which the height 
 *                            is to be retrieved
 * Return: the number of rows in the bit2_array (the height of the bit2_array)
 * Expects:
 *       Pointer to bit2_array to be nonnull
 * Notes:
 *      Checked runtime error if bit2_array is null
 ************************/
int Bit2_height(T bit2_array)
{
        assert(bit2_array != NULL);
        assert(bit2_array->bit_array != NULL);
        return bit2_array->height;
}

/**********Bit2_get********
 *
 * Returns the bit at (row, col) in bit2_array
 * Inputs:
 *              T bit2_array: A pointer to a bit2 array in which the bit is to 
 *                            be returned
 * Return: the bit at (row, col) in bit2_array
 * Expects:
 *      * bit2_array to be nonnull
 *      * The row value is positive and less than the height of the bit2_array 
 *      * The col value is positive and less than the width of the bit2_array
 * Notes:
 *      * Checked runtime error if:
 *              * bit2_array is null 
 *              * row value >= height
 *              * col value >= width
 ************************/
int Bit2_get(T bit2_array, int col, int row)
{
        assert(bit2_array != NULL);
        assert(col >= 0 && col < bit2_array->width);
        assert(row >= 0 && row < bit2_array->height);
        return Bit_get(bit2_array->bit_array, row * bit2_array->width + col);
}

/**********Bit2_put********
 *
 * Sets the bit at (row, col) in the bit2_array to the value of bit
 * Inputs:
 *              T bit2_array: A pointer to a bit2 array in which the bit is to 
 *                            be returned
 *              int row: The row index of the element to be set within 
 *                       the bit2_array
 *              int col: The column index of the element to be set within the 
 *                       bit2_array
 * Return: the previous bit value at (row, col) in bit2_array before it is set
 * Expects:
 *      * bit2_array to be nonnull
 *      * The row value is positive and less than the height of the bit2_array 
 *      * The col value is positive and less than the width of the bit2_array
 *      * bit to be zero or one
 * Notes:
 *      * Checked runtime error if:
 *              * bit2_array is null 
 *              * row value >= height
 *              * col value >= width
 *              * bit is not zero or one
 ************************/
int Bit2_put(T bit2_array, int col, int row, int bit)
{
        assert(bit2_array != NULL);
        assert(col >= 0 && col < bit2_array->width);
        assert(row >= 0 && row < bit2_array->height);
        assert(bit == 0 || bit == 1);
        return Bit_put(bit2_array->bit_array, row * bit2_array->width + col, 
                                                                        bit);
}

/**********Bit2_map_row_major********
 *
 * Calls an apply function for each element in bit2_array, in order from low to
 * high indices, with column indices varying more rapidly than row indices
 * Inputs:
 *              T bit2_array: A pointer to the bit2_array that the apply 
 *                            function will be called on 
 *                         
 *              void apply: The function that will be applied to each element 
 *                          in bit2_array      * parameters detailed below *
 *                  int row: the current row index
 *                  int col: the current column index
 *                  T bit2_array: A pointer to the same bit2_array passed into 
 *                                the outside function
 *                  int bit: the bit value at (row, col)
 *                  void *cl: A closure passed in by the client to be used in 
 *                            the apply function  
 *
 *              void *cl: A closure passed in by the client to be used in the
 *                        apply function               
 *                       
 *          
 * Return: N/A
 * Expects: 
 *      * bit2_array to be nonnull
 *      * The row value is positive and < the height of the bit2_array 
 *      * The col value is positive and < than the width of the bit2_array
 *      * bit to be zero or one
 * Notes:
 *      * Checked runtime error if:
 *              * bit2_array is null 
 *              * row value >= height
 *              * col value >= width
 *              * bit is not zero or one
 *      Updates the bit2_array entered in as the first parameter 
 ************************/
void Bit2_map_row_major(T bit2_array, void apply(int col, int row, 
                                T bit2_array, int bit, void *cl), void *cl)
{
        assert(bit2_array != NULL);
        for (int r = 0; r < bit2_array->height; r++) {
                for (int c = 0; c < bit2_array->width; c++) {
                        apply(c, r, bit2_array, Bit2_get(bit2_array, c, r), 
                                                                        cl);
                }
        }
}

/**********Bit2_map_col_major********
 *
 * Calls an apply function for each element in bit2_array, in order from low to
 * high indices, with row indices varying more rapidly than column indices
 * Inputs:
 *              T bit2_array: A pointer to the bit2_array that the apply 
 *                            function will be called on 
 *                         
 *              void apply: The function that will be applied to each element 
 *                          in bit2_array      * parameters detailed below *
 *                  int row: the current row index
 *                  int col: the current column index
 *                  T bit2_array: A pointer to the same bit2_array passed into 
 *                                the outside function
 *                  int bit: the bit value at (row, col)
 *                  void *cl: A closure passed in by the client to be used in 
 *                            the apply function  
 *
 *              void *cl: A closure passed in by the client to be used in the
 *                        apply function               
 *                        
 *          
 * Return: N/A
 * Expects: 
 *      * bit2_array to be nonnull
 *      * The row value is positive and < the height of the bit2_array 
 *      * The col value is positive and < the width of the bit2_array
 *      * bit to be zero or one
 * Notes:
 *      * Checked runtime error if:
 *              * bit2_array is null 
 *              * row value >= height
 *              * col value >= width
 *              * bit is not zero or one
 *      Updates the bit2_array entered in as the first parameter 
 ************************/
void Bit2_map_col_major(T bit2_array, void apply(int col, int row, 
                                T bit2_array, int bit, void *cl), void *cl)
{
        assert(bit2_array != NULL);
        for (int c = 0; c < bit2_array->width; c++) {
                for (int r = 0; r < bit2_array->height; r++) {
                        apply(c, r, bit2_array, Bit2_get(bit2_array, c, r), 
                                                                        cl);
                }
        }
}
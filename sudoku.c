/*
 *     sudoku.c
 *     by Kabir Pamnani and Isaac Monheit, 02/06/2023
 *     HW2: Interfaces, Implementations and Images (iii)
 *
 *     Summary: Uses uarray2.h interface to identify Sudoku puzzle solutions
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <pnmrdr.h>
#include "uarray2.h"
#include "set.h"
#include "atom.h"

const int LINE_WIDTH = 9;
const int LINE_HEIGHT = 1;

void check_pgm_format(Pnmrdr_mapdata input_data);
UArray2_T sudoku_puzzle(Pnmrdr_T input, Pnmrdr_mapdata input_data);
void insert_pgm_into_array(int col, int row, UArray2_T sudoku, 
                                        void *element_at, void *input);
void validate_lines(int col, int row, UArray2_T sudoku, void *element_at, 
                                                        void *one_line);
void validate_3x3s(UArray2_T sudoku, Set_T one_line);


int main(int argc, char *argv[]) 
{
        assert(argc == 1 || argc == 2);
        FILE *input_file;
        
        if (argc == 1) {
                input_file = stdin;
        } else { 
                input_file = fopen(argv[1], "r");
                assert(input_file != NULL);
        }

        /* check format of pgm */
        Pnmrdr_T input = Pnmrdr_new(input_file);
        Pnmrdr_mapdata input_data = Pnmrdr_data(input);
        check_pgm_format(input_data);

        /* turn pgm into a 2D UArray */
        UArray2_T test = sudoku_puzzle(input, input_data);
        
        /* validate rows and columns */
        Set_T one_line = Set_new(9, NULL, NULL);
        UArray2_map_row_major(test, validate_lines, &one_line);
        UArray2_map_col_major(test, validate_lines, &one_line);

        /* validate smaller 3x3 grids */
        validate_3x3s(test, one_line);

        /* free up memory */
        UArray2_free(&test);
        Pnmrdr_free(&input);
        fclose(input_file);

        exit(EXIT_SUCCESS);
}


/**********check_pgm_format********
 *
 * Checks that the filename provided is a correct portable graymap file
 * Inputs:
 *              Pnmrdr_mapdata input_data: input_data is an instance of a 
 *                                         struct of type Pnmrdr_mapdata, 
 *                                         which is used in the function to
 *                                         validate that the correct values 
 *                                         associated with the pgm file are 
 *                                         being inputted.
 * Return: N/A
 * Expects:
 *      * type of input_data to be a pgm (value = 2)
 *      * width, height and denominator to be 9
 * Notes:
 *      * Checked runtime error if type of input_data != 2 (not of type pgm)
 *      * Program exits if width, height, denominator != 9
 ************************/
void check_pgm_format(Pnmrdr_mapdata input_data)
{
        /* check if it's a pgm */
        assert(input_data.type == 2);
        
        if (input_data.width != 9 || input_data.height != 9 
                                        || input_data.denominator != 9) {
                exit(EXIT_FAILURE);
        }
}

/**********sudoku_puzzle********
 *
 * Creates, allocates and populates a new UArray2 with values from the pgm 
 * input file. The UArray2 that holds these values represents the sudoku board
 * Inputs:
 *              Pnmrdr_T input: input will be passed as a closure to a map 
 *                              function in order to read in the values from an
 *                              input file
 *              Pnmrdr_mapdata input_data: input_data is an instance of a 
 *                                         struct of type Pnmrdr_mapdata, 
 *                                         which is used in the function to
 *                                         access the values associated with 
 *                                         the pgm file that is inputted
 * Return: A UArray2 that is populated with the values in the pgm file
 * Expects:
 *      * width and height to be nonnegative
 *      * size to be positive
 * Notes:
 *      * UArray2 sudoku_array is allocated memory in this function. 
 *      * The client must use Uarray2_free once the memory is no longer needed
 ************************/
UArray2_T sudoku_puzzle(Pnmrdr_T input, Pnmrdr_mapdata input_data) 
{
        UArray2_T sudoku_array = UArray2_new(input_data.width, 
                                        input_data.height, sizeof(int));
        UArray2_map_row_major(sudoku_array, insert_pgm_into_array, &input);
        return sudoku_array;
}

/**********insert_pgm_into_array********
 *
 * Inserts a value in the pgm file into the UArray2 at a given position 
 * defined by (col, row)
 * Inputs:
 *              int col: column value where element will be inserted
 *              int row: row value where element will be inserted
 *              UArray2_T sudoku: Pointer to the UArray2 where value will be 
 *                                inserted
 *              void *element_at: pointer to element at a specific position
 *                                (not used)
 *              void *input: a closure value, in this case it is a pointer to 
 *                           a struct of type Pnmrdr_T, which is used to enable
 *                           access to each value in the pgm file
 * Return: N/A
 * Expects: 
 *      * An existing UArray2 is entered in as the first parameter
 *      * The row value is positive and is less than the height of the UArray2
 *      * The col value is positive and is less than the width of the UArray2
 * Notes:
 *      * Program exits if element = 0
 *      * Used as an apply function for map_row_major, so that it will iterate
 *      and insert every value from the pgm file into the UArray2
 ************************/
void insert_pgm_into_array(int col, int row, UArray2_T sudoku, 
                                        void *element_at, void *input)
{
        (void) element_at;
        *(int *)UArray2_at(sudoku, col, row) = Pnmrdr_get(*(Pnmrdr_T *)input);
        
        if (*(int *)UArray2_at(sudoku, col, row) == 0) {
                exit(EXIT_FAILURE);
        }
}

/**********validate_lines********
 *
 * Checks that every value in a line (row or column) is unique
 * Inputs:
 *              int col: column value (not used in function)
 *              int row: row value (not used in function)
 *              UArray2_T sudoku: Pointer to the UArray2 (not used in function)
 *              void *element_at: pointer to element at a specific position
 *              void *one_line: a closure value. In this function it is a 
 *                              set that is used to check if each element in a
 *                              line is unique
 * Return: None
 * Expects:
 *      None
 * Notes:
 *      * Each new set created is allocated memory in this function, and then
 *      freed in this function once the set reaches max capacity (length = 9)
 *      * However, in the final instance (when validating the last line), a new
 *      set is allocated memory within this function. The client must free the
 *      memory using Set_free once it is no longer needed.
 ************************/
void validate_lines(int col, int row, UArray2_T sudoku, void *element_at, 
                                                                void *one_line)
{        
        (void)col;
        (void)row;
        (void)sudoku;

        int elem = *(int *)element_at;

        /* turns elem into an Atom to be able to be used in the Set */
        const char *curr_elem = Atom_int(elem);

        /* if there are any repeat numbers in one line */
        if (Set_member(*(Set_T *)one_line, curr_elem)) {
                exit(EXIT_FAILURE);
        }
        
        Set_put(*(Set_T *)one_line, curr_elem);
        
        /* reset the Set every 9 elems */
        if (Set_length(*(Set_T *)one_line) == 9) {
                Set_free(&(*(Set_T *)one_line));
                *(Set_T *)one_line = Set_new(9, NULL, NULL);
        }
}

/**********validate_3x3s********
 *
 * Checks that every value in each 3x3 grid is unique
 * Inputs:
 *              UArray2_T sudoku: Pointer to the UArray2 (not used in function)
 *              Set_T one_line: Pointer to a set that will be populated with
 *                              each value in each 3x3 grid
 * Return: None
 * Expects:
 *      * width and height to be nonnegative
 *      * size to be positive
 * Notes:
 *      A new UArray2 box_to_row is allocated memory to hold each grid, and 
 *      freed after the check is done
 *      This function also frees the set that contains the last line in 
 *      validate_lines()
 ************************/
void validate_3x3s(UArray2_T sudoku, Set_T one_line) 
{
        for (int row = 0; row < 9; row += 3) {
                for (int col = 0; col < 9; col += 3) {
                        /* new 9 x 1 UArray2 for each small box */
                        UArray2_T box_to_row = UArray2_new
                                                (LINE_WIDTH, 
                                                 LINE_HEIGHT, 
                                                 sizeof(int));
                        int count = 0;

                        for (int i = row; i < row + 3; i++) {
                                for (int j = col; j < col + 3; j++) {
                                        *(int *)UArray2_at
                                        (box_to_row, count, 0)
                                         = *(int *)UArray2_at(sudoku, j, i);
                                        count++;
                                }        
                        }
                        UArray2_map_row_major(box_to_row, validate_lines, 
                                                                &one_line);
                        UArray2_free(&box_to_row);
                }
        }
        Set_free(&one_line);
}

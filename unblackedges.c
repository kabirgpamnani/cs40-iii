/*
 *     unblackedges.c
 *     by Kabir Pamnani and Isaac Monheit, 02/06/2023
 *     HW2: Interfaces, Implementations and Images (iii)
 *
 *     Summary: Uses bit2.h interface to implement a program that removes black
 *              edges
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <pnmrdr.h>
#include "bit2.h"
#include "stack.h"
#include <stdbool.h>

const int MARKED = 1;
const int WHITE = 0; 

struct bit_position {
        int col;
        int row;
};

typedef struct bit_position *bp;

void check_pbm_format(Pnmrdr_mapdata input_data);
Bit2_T image_2D_array(Pnmrdr_T input, Pnmrdr_mapdata input_data);
void insert_pbm_into_array(int col, int row, Bit2_T image, int bit, 
                                                            void *input);

void remove_black_edges(Bit2_T image);
void connected_black_edge(int col, int row, Bit2_T image, int bit, 
                                                            void *black_edges);

bp make_bp(int col, int row);
bool is_black_edge(int col, int row, Bit2_T image);
void free_structs_in_stack(Stack_T stack);
bool valid_black_bit(int col, int row, Bit2_T image, Bit2_T visited_bits);
bool visited(Bit2_T visited_bits, int col, int row);

void print_one_bit(int col, int row, Bit2_T image, int bit, void *input);


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

        /* check format of pbm */
	Pnmrdr_T input = Pnmrdr_new(input_file);
        Pnmrdr_mapdata input_data = Pnmrdr_data(input);
        check_pbm_format(input_data);

        /* turn pbm into a 2D bit array */
        Bit2_T image = image_2D_array(input, input_data);

        remove_black_edges(image);

        /* printing output */
        printf("P1\n# file without black edges\n%i %i\n", 
                        Bit2_width(image), Bit2_height(image));
        Bit2_map_row_major(image, print_one_bit, NULL);

        /* freeing memory */
        Pnmrdr_free(&input);
        Bit2_free(&image);
	fclose(input_file);

        exit(EXIT_SUCCESS);
}

/**********check_pbm_format********
 *
 * Checks that the filename provided is a correct portable bitmap file
 * Inputs:
 *              Pnmrdr_mapdata input_data: input_data is an instance of a 
 *                                         struct of type Pnmrdr_mapdata, 
 *                                         which is used in the function to
 *                                         validate that the correct values 
 *                                         associated with the pbm file are 
 *                                         being inputted.
 * Return: N/A
 * Expects:
 *      * type of input_data to be a pbm (value = 1)
 *      * width and height to be 0
 * Notes:
 *      Checked runtime error if:
 *              * type of input_data != 1 (not of type pbm)
 *      Exits with EXIT_FAILURE if width or height are nonzero  
 ************************/
void check_pbm_format(Pnmrdr_mapdata input_data) 
{
        /* check if it's a pbm */
	assert(input_data.type == 1);

	if (input_data.width == 0 || input_data.height == 0) {
                exit(EXIT_FAILURE);
	}
}

/**********image_2D_array********
 *
 * Creates, allocates and populates a new Bit2_array with values from the pbm 
 * input file. The Bit2_array that holds these values represents the bitmap to
 * be converted.
 * Inputs:
 *              Pnmrdr_T input: input will be passed as a closure to the map 
 *                              function in order to read in the bits from an
 *                              input file
 *              Pnmrdr_mapdata input_data: input_data is an instance of a 
 *                                         struct of type Pnmrdr_mapdata, 
 *                                         which is used in the function to
 *                                         access the values associated with 
 *                                         the pbm file that is inputted
 * Return: A Bit2_array that is populated with the values in the pbm file
 * Expects:
 *      * input_data.width and input_data.height to be nonnegative
 * Notes:
 *      * Bit2_array image_array is allocated memory in this function. 
 *      * The client must use Bit2_free once the memory is no longer needed
 ************************/
Bit2_T image_2D_array(Pnmrdr_T input, Pnmrdr_mapdata input_data) 
{
        Bit2_T image_array = Bit2_new(input_data.width, input_data.height);
        Bit2_map_row_major(image_array, insert_pbm_into_array, &input);
        return image_array;
}

/**********insert_pbm_into_array********
 *
 * Inserts a value in the pbm file into the Bit2_array at a given position 
 * defined by (col, row)
 * Inputs:
 *              int col: column value where element will be inserted
 *              int row: row value where element will be inserted
 *              Bit2_T image: Pointer to the Bit2_array where value will be 
 *                            inserted
 *              int bit: integer value of bit at given position (either 1 or 0)
 *                       (not used)
 *              void *input: a closure value, in this case it is a pointer to 
 *                           a struct of type Pnmrdr_T, which is used to enable
 *                           access to each value in the pbm file
 * Return: N/A
 * Expects: 
 *      None
 * Notes:
 *      * Used as an apply function for map_row_major, so it will iterate 
 *      through and insert every value from the pbm file into the Bit2_array
 ************************/
void insert_pbm_into_array(int col, int row, Bit2_T image, int bit, 
                                                             void *input)
{
        (void)bit;
        int prev_bit = Bit2_put(image, col, row, 
                                        Pnmrdr_get(*(Pnmrdr_T *)input));
        (void)prev_bit;
        
}

/**********remove_black_edges********
 *
 * Converts all black pixels that connect to a black edge pixel into white
 * Inputs:
 *              Bit2_T image: Pointer to the Bit2_array that stores the pixels
 *                            before they have been converted
 * Return: N/A
 * Expects: 
 *      image to be nonnull
 * Notes:
 *      * Memory is allocated for a new stack in this function
 *      * Each element popped from the stack is freed, as well as the stack 
 *      * itself
 ************************/
void remove_black_edges(Bit2_T image)
{
        Stack_T black_edges = Stack_new();
        Bit2_map_row_major(image, connected_black_edge, &black_edges);

        while (Stack_empty(black_edges) != 1) {
               bp popped = (bp)Stack_pop(black_edges);
               int prev_bit = Bit2_put(image, popped->col, popped->row, WHITE);
               (void)prev_bit;
               free(popped);
        }
        Stack_free(&black_edges);
}

/**********connected_black_edge********
 *
 * Pushes all the edge coordinates that are either a black edge themself, or 
 * are connected to a black edge onto a stack passed in as a closure
 * Inputs:
 *              int col: column value of the bit being checked
 *              int row: row value of the bit being checked
 *              Bit2_T image: Pointer to the Bit2_array that stores the pixels
 *                            before they have been converted
 *              int bit: bit value at given position (0 or 1)
 *              void *black_edges: black_edges is passed as a closure. 
 *                                 black_edges is a pointer to a stack that 
 *                                 stores the final location of all pixels that
 *                                 are connected to a black edge
 * Return: N/A
 * Expects: 
 *      * image to be nonnull
 *      * bit to be 0 or 1
 * Notes:
 *      * To be used in Bit2_map_row_major as an apply function
 *      * Memory is allocated for a new Bit2_array, and is then freed after 
 *      * going through every pixel
 *      * Memory is allocated for a new stack, and is then freed after all of 
 *      * its elements have been popped from the stack
 *      * Memory is allocated for one struct (original_bit) outside the 
 *      * while loop, and is then freed once the while loop concludes
 *      * Memory is allocated for a new struct everytime a new valid black 
 ************************/
void connected_black_edge(int col, int row, Bit2_T image, int bit, 
                                                            void *black_edges)
{
        if (bit == 0) {
                return;
        }
        
        /* save the original bit for if it's a valid black edge */
        bp original_bit = make_bp(col, row);
        
        Stack_T bits_to_check = Stack_new();
        bp first_bit = make_bp(col, row);
        Stack_push(bits_to_check, first_bit);

        Bit2_T visited_bits = Bit2_new(Bit2_width(image), Bit2_height(image));

        while (Stack_empty(bits_to_check) != 1) {
                bp curr_bit = (bp)Stack_pop(bits_to_check);

                /* push to stack once a black edge pixel is found */
                if (is_black_edge(curr_bit->col, curr_bit->row, image) 
                                                                  == true) {
                        Stack_push(*(Stack_T *)black_edges, original_bit);
                        free(curr_bit);
                        free_structs_in_stack(bits_to_check);
                        Bit2_free(&visited_bits);
                        return;
                }
                /* 
                * push all neighbouring, unvisited black bits (right, left, 
                * above and below) onto a stack to check if they are connected
                * to a black edge pixel
                */
                if (valid_black_bit(curr_bit->col + 1, curr_bit->row, 
                                        image, visited_bits) == true) {
                        bp pos = make_bp(curr_bit->col + 1, curr_bit->row);
                        Stack_push(bits_to_check, pos);
                }
                if (valid_black_bit(curr_bit->col - 1, curr_bit->row, 
                                        image, visited_bits) == true) {
                        bp pos = make_bp(curr_bit->col - 1, curr_bit->row);
                        Stack_push(bits_to_check, pos); 
                }
                if (valid_black_bit(curr_bit->col, curr_bit->row + 1, 
                                        image, visited_bits) == true) {
                        bp pos = make_bp(curr_bit->col, curr_bit->row + 1);
                        Stack_push(bits_to_check, pos); 
                }
                if (valid_black_bit(curr_bit->col, curr_bit->row - 1, 
                                        image, visited_bits) == true) {
                        bp pos = make_bp(curr_bit->col, curr_bit->row - 1);
                        Stack_push(bits_to_check, pos); 
                }
                Bit2_put(visited_bits, curr_bit->col, curr_bit->row, MARKED);
                free(curr_bit);
        }
        free(original_bit);
        free_structs_in_stack(bits_to_check);
        Bit2_free(&visited_bits);
}

/**********make_bp********
 *
 * Creates and allocates memeory for one bit_position struct
 * Inputs:
 *              int col: column coordinate
 *              int row: row coordinate
 * Return: A pointer to a bit_position struct with the col and row values given
 *         as parameters
 * Expects:
 *      None
 * Notes:
 *      allocates memory for a struct that the client needs to free when they
 *      no longer need the struct
 ************************/
bp make_bp(int col, int row) 
{
        bp pos = malloc(sizeof(*pos));
        assert(pos != NULL);
        pos->col = col;
        pos->row = row;
        return pos;
}

/**********is_black_edge********
 *
 * Checks if a pixel in an image is black and is on the edge of the image
 * Inputs:
 *              int col: column coordinate value to be checked
 *              int row: row coordinate value to be checked
 *              Bit2_T image: The image where the coordinates will be checked
 * Return: A bool that is true is the pixel in (col,row) is black and is on
 *         the edge of image
 * Expects: 
 *      image to be nonnull
 *      col is positive and less than the height of image
 *      row is positive and less than the width of image
 * Notes:
 *      None
 ************************/
bool is_black_edge(int col, int row, Bit2_T image)
{
        if (((col == 0) || (col == (Bit2_width(image) - 1)) || 
             (row == 0) || (row == (Bit2_height(image) - 1))) 
                        && (Bit2_get(image, col, row) == 1)) {
                return true;
        } else {
                return false;
        }
}

/**********free_structs_in_stack********
 *
 * Frees up memory from all the structs in an inputted stack and then frees up
 * memory allocated by that stack
 * Inputs:
 *              Stack_T stack: stack of structs that will be freed
 * Return: N/A
 * Expects: 
 *      stack is nonnull
 * Notes:
 *      None
 ************************/
void free_structs_in_stack(Stack_T stack) 
{
        assert(stack != NULL);
        while (Stack_empty(stack) != 1) {
                bp curr_struct = (bp)Stack_pop(stack);
                free(curr_struct);
        }
        Stack_free(&stack);
}

/**********valid_black_bit********
 *
 * Checks if the current bit being inputed at (col,row) is a black bit that is
 * within the range of the image and hasn't been visited before
 * Inputs:
 *              int col: column value of the bit being checked
 *              int row: row value of the bit being checked
 *              Bit2_T image: the image being checked
 *              Bit2_T visited_bits: a representation of all the visited bits
 *                                   to be used to check if the current one
 *                                   has already been visited
 * Return: A bool that is true if the current bit is a black bit that is within
 *         the range of the image and hasn't been visited before, false if
 *         any of those predicates aren't correct
 * Expects:
 *      visited_bits and image to be nonnull
 * Notes:
 *      None
 ************************/
bool valid_black_bit(int col, int row, Bit2_T image, Bit2_T visited_bits)
{
        if ((col < 0) || (col > Bit2_width(image) - 1) ||  
            (row < 0) || (row > Bit2_height(image) - 1)) {
                return false;
        }
        
        if ((Bit2_get(image, col, row) == 1) && 
            (visited(visited_bits, col, row) == false)) {
                return true;
        } else {
                return false;
        }
}

/**********visited********
 *
 * Checks if a location in a 2D bitmap has been visited before
 * Inputs:
 *              Bit2_T visited_bits: 2D bitmap that is being checked
 *              int col: column value of the location being checked
 *              int row: row value of the location being checked
 * Return: A bool that is true if the location has been visited before, false
 *         if not
 * Expects:
 *      visited_bits to be nonnull
 *      The row value is positive and less than the height of visited_bits
 *      The col value is positive and less than the width of visited_bits
 * Notes:
 *      Within the 2D bitmap, a 1 bit means the location has been visited, and
 *      a 0 bit means that the location hasn't been visited
 ************************/
bool visited(Bit2_T visited_bits, int col, int row)
{
        if (Bit2_get(visited_bits, col, row) == 1) {
                return true;
        } else {
                return false;
        }
}

/**********print_one_bit********
 *
 * Prints one bit value in a 2D image array, at the location (col,row), and
 * prints a white character after it (either a space or a newline)
 * Inputs:
 *              int col: column value of the bit (not used)
 *              int row: row value of the bit (not used)
 *              Bit2_T image: Pointer to the Bit2_array that is used to find
 *                            the width of the entire image to figure out where
 *                            to put newlines
 *              int bit: integer value of bit at given position (either 1 or 0)
 *                       being printed to output
 *              void *input: a closure value (not used)
 * Return: N/A
 * Expects: 
 *      None
 * Notes:
 *      * Used as an apply function for Bit2_map_row_major, so it will iterate 
 *      through and print out every bit in image
 *      * There is one space added between each bit, with the exception of a 
 *      newline instead after every row is completed, setting up the next row
 *      of the image on the next line
 ************************/
void print_one_bit(int col, int row, Bit2_T image, int bit, void *input)
{
        (void)col;
        (void)row;
        (void)input;

        printf("%i", bit);
        if (col == (Bit2_width(image) - 1)) {
                printf("\n");
        } else {
                printf(" ");
        }
}

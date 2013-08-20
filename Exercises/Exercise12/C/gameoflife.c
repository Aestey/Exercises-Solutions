//------------------------------------------------------------------------------
//
// Name:       gameoflife.c
// 
// Purpose:    Run a naive Conway's game of life
//
// HISTORY:    Written by Tom Deakin and Simon McIntosh-Smith, August 2013
//
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#define PARAMFILE      "input.params"
#define FINALSTATEFILE "final_state.dat"

// Define the state of the cell
#define DEAD  0
#define ALIVE 1

/*************************************************************************************
 * Forward declarations of utility functions
 ************************************************************************************/
void die(const char* message, const int line, const char *file);
void load_board(char* board, const char* file, const unsigned int nx, const unsigned int ny);
void print_board(const char* board, const unsigned int nx, const unsigned int ny);
void save_board(const char* board, const unsigned int nx, const unsigned int ny);
void load_params(unsigned int *nx, unsigned int *ny);


/*************************************************************************************
 * Game of Life worker method
 ************************************************************************************/

// Apply the rules of life to tick and save in tock
void accelerate_life(char* tick, char* tock, const int nx, const int ny)
{
    // The cell we work on in the loop
    unsigned int idx;

    // Indexes of rows/columns next to idx
    // wrapping around if required
    unsigned int x_l, x_r, y_u, y_d;

    for (unsigned int i = 0; i < ny; i++)
    {
        for (unsigned int j = 0; j < nx; j++)
        {
            // Calculate indexes
            idx = i * nx + j;
            x_r = (j + 1) % nx;
            x_l = (j == 0) ? ny - 1 : j - 1;
            y_u = (i + 1) % nx;
            y_d = (i == 0) ? nx - 1: i - 1;

            // Count alive neighbours (out of eight)
            int neighbours = 0;
            if (tick[i * nx + x_l] == ALIVE) neighbours++;
            if (tick[y_u * nx + x_l] == ALIVE) neighbours++;
            if (tick[y_d * nx + x_l] == ALIVE) neighbours++;
            
            if (tick[i * nx + x_r] == ALIVE) neighbours++;
            if (tick[y_u * nx + x_r] == ALIVE) neighbours++;
            if (tick[y_d * nx + x_r] == ALIVE) neighbours++;
            
            if (tick[y_u * nx + j] == ALIVE) neighbours++;
            if (tick[y_d * nx + j] == ALIVE) neighbours++;

            // Apply game of life rules
            if (tick[idx] == ALIVE)
            {
                if (neighbours == 2 || neighbours == 3)
                    // Cell lives on
                    tock[idx] = ALIVE;
                else
                    // Cell dies by over/under population
                    tock[idx] = DEAD;
            }
            else
            {
                if (neighbours == 3)
                    // Cell becomes alive through reproduction
                    tock[idx] = ALIVE;
                else
                    // Remains dead
                    tock[idx] = DEAD;
            }

        }
    }
}


/*************************************************************************************
 * Main function
 ************************************************************************************/

int main(int argc, void **argv)
{

    // Check we have a starting state file
    if (argc != 2)
    {
        printf("Usage:\n./gameoflife input.dat\n");
        return EXIT_FAILURE;
    }

    // Board dimensions
    unsigned int nx, ny;

    load_params(&nx, &ny);

    // Allocate memory for boards
    char* board_tick = (char *)calloc(nx * ny, sizeof(char));
    char* board_tock = (char *)calloc(nx * ny, sizeof(char));

    if (!board_tick || !board_tock)
        die("Could not allocate memory for board", __LINE__, __FILE__);

    // Load in the starting state to board_tick
    load_board(board_tick, argv[1], nx, ny);

    // Display the starting state
    printf("Starting state\n");
    print_board(board_tick, nx, ny);

    // Loop
    // TODO

    accelerate_life(board_tick, board_tock, nx, ny);
    printf("Then:\n");
    print_board(board_tock, nx, ny);
    accelerate_life(board_tock, board_tick, nx, ny);
    printf("Then\n");
    print_board(board_tick, nx, ny);
    accelerate_life(board_tick, board_tock, nx, ny);

    // Display the final state
    printf("Finishing state\n");
    print_board(board_tock, nx, ny);

    // Save the final state of the board
    save_board(board_tock, nx, ny);

    return EXIT_SUCCESS;
}


/*************************************************************************************
 * Utility functions
 ************************************************************************************/

// Function to load the params file and set up the X and Y dimensions
void load_params(unsigned int *nx, unsigned int *ny)
{
    FILE *fp = fopen(PARAMFILE, "r");
    if (!fp)
        die("Could not open params file.", __LINE__, __FILE__);

    int retval;
    retval = fscanf(fp, "%d\n", nx);
    if (retval != 1)
        die("Could not read params file: nx.", __LINE__, __FILE__);
    retval = fscanf(fp, "%d\n", ny);
    if (retval != 1)
        die("Could not read params file: ny", __LINE__, __FILE__);

    fclose(fp);
}

// Function to load in a file which lists the alive cells
// Each line of the file is expected to be: x y 1
void load_board(char* board, const char* file, const unsigned int nx, const unsigned int ny)
{
    FILE *fp = fopen(file, "r");
    if (!fp)
        die("Could not open input file.", __LINE__, __FILE__);

    int retval;
    unsigned int x, y, s;
    while ((retval = fscanf(fp, "%d %d %d\n", &x, &y, &s)) != EOF)
    {
        if (retval != 3)
            die("Expected 3 values per line in input file.", __LINE__, __FILE__);
        if (x < 0 || x > nx - 1)
            die("Input x-coord out of range.", __LINE__, __FILE__);
        if (y < 0 || y > ny - 1)
            die("Input y-coord out of range.", __LINE__, __FILE__);
        if (s != ALIVE)
            die("Alive value should be 1.", __LINE__, __FILE__);

        board[x + y * nx] = ALIVE;
    }

    fclose(fp);
}

// Function to print out the board to stdout
// Alive cells are displayed as O
// Dead cells are displayed as .
void print_board(const char* board, const unsigned int nx, const unsigned int ny)
{
    for (unsigned int i = 0; i < ny; i++)
    {
        for (unsigned int j = 0; j < nx; j++)
        {
            if (board[i * nx + j] == DEAD)
                printf(".");
            else
                printf("O");
        }
        printf("\n");
    }
}

void save_board(const char* board, const unsigned int nx, const unsigned int ny)
{
    FILE *fp = fopen(FINALSTATEFILE, "w");
    if (!fp)
        die("Could not open final state file.", __LINE__, __FILE__);

    for (unsigned int i = 0; i < ny; i++)
    {
        for (unsigned int j = 0; j < nx; j++)
        {
            if (board[i * nx + j] == ALIVE)
                fprintf(fp, "%d %d %d\n", j, i, ALIVE);
        }
    }
}

// Function to display error and exit nicely
void die(const char* message, const int line, const char *file)
{
  fprintf(stderr, "Error at line %d of file %s:\n", line, file);
  fprintf(stderr, "%s\n",message);
  fflush(stderr);
  exit(EXIT_FAILURE);
}
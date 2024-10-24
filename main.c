#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "machine.h"
#include "parser.h"

#define MAX_INPUT_LEN 100

void parse_input(char *input, char *arr);

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Usage: ./turing file \"input\" [delay ms] [animate 0/1]\n");
        return 0;
    }
    if (strlen(argv[2]) > MAX_INPUT_LEN + 2)
    {
        fprintf(stderr, "input too long\n");
        return 1;
    }

    char *filename = argv[1];
    char input[MAX_INPUT_LEN];
    parse_input(argv[2], input);

    int delay = 0;
    if (argc >= 4)
        delay = strtol(argv[3], NULL, 10);
    bool clear_screen = false;
    if (argc == 5 && !strcmp(argv[4], "1"))
        clear_screen = true;

    State *states = malloc(100 * sizeof(State));

    int num_states = parse(filename, states);
    if (num_states == 0)
        return 1;

    Machine machine;
    init_machine(&machine, input, states, num_states, "START");

    int result = run_machine(&machine, delay, clear_screen);

    if (result < 0)
        printf("Program halted: FAILURE (could not find valid instruction)\n");
    else
    {
        printf("Program halted: SUCCESS (reached halt state after %d steps)\n", result);
        printf("Final state: ");
        print_final_state(machine.tape, machine.pointer);
        printf("\n");
    }

    free(states);
}

void parse_input(char *input, char *arr)
{
    char *first = arr;

    while (*input != '\0')
        if (*input != '"')
            *arr++ = *input++;

    *arr++ = '_';
    *arr = '\0';
    arr = first;
}

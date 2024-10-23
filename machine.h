#pragma once
#include <limits.h>
#include <stdbool.h>

#define BLANK       '_'
#define WILDCARD    -1
#define TAPE_SIZE   10000
#define MAX_STATES  100
#define MAX_ID_LEN  50

typedef enum Shift { LEFT, RIGHT, NONE } Shift;

typedef struct Instruction {
    char read;
    char write;
    Shift shift;
    char next_state_name[MAX_ID_LEN];
    int next_state;
} Instruction;

typedef struct State {
    int id;
    char name[MAX_ID_LEN];
    int num_instructions;
    Instruction instructions[255];
} State;

typedef struct Machine {
   char tape[TAPE_SIZE];
   unsigned int pointer;
   State **states;
   char **state_ids;
   int initial_state;
   int num_states;
} Machine;

void init_machine(Machine *m, char *input, State states[], int num_states, char *initial_state);
int run_machine(Machine *m, int delay, bool clear_screen);

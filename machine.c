#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <unistd.h>
#endif

#include "machine.h"

Instruction* get_next_instruction(State* state, char value);
void print_state(char *tape, unsigned int start_pos, int num_cells, unsigned int pointer, char *state_name);
void wait_ms(int ms);

void init_machine(Machine *m, char *input, State states[], int num_states, char* initial_state)
{
    m->pointer = TAPE_SIZE / 2;

    // Initialize tape
    for (int i = 0; i < TAPE_SIZE; i++)
         m->tape[i]= BLANK;
         
    m->states = malloc(num_states * sizeof(State));
    m->state_ids = malloc(num_states * sizeof(char*));

    // Add states to machine
    for (int i = 0; i < num_states; i++)
    {
        *(m->states+i) = &(states[i]);
        *(m->state_ids + i) = states[i].name; // Assign an ID to the state (in this case just the array index)

        if (!strcmp(*(m->state_ids + i), initial_state))
            m->initial_state = i;
    }

    // Use state IDs instead of names for easy indexing
    for (int i = 0; i < num_states; i++)
    {
        for (int j = 0; j < m->states[i]->num_instructions; j++)
        {
            if (!strcmp(m->states[i]->instructions[j].next_state_name, "HALT")) // Special case
            {
                m->states[i]->instructions[j].next_state = -1;
                continue;
            }

            for (int k = 0; k < num_states; k++) // Search for the corresponding state ID
            {
                if (!strcmp(m->state_ids[k], m->states[i]->instructions[j].next_state_name))
                    m->states[i]->instructions[j].next_state = k;
            }
        }
    }

    // Copy input to tape
    int i = 0;
    while (*input != '\0')
         m->tape[m->pointer + i++] = *input++;
}

int run_machine(Machine* m, int delay, bool clear_screen)
{
    State* curr_state = m->states[m->initial_state];
    Instruction* instruction = get_next_instruction(curr_state, m->tape[m->pointer]);

    unsigned int init_pos = m->pointer;

    int num_iterations = 0;
    while (instruction != NULL && instruction->next_state != -1) // Halt state or invalid instruction
    {
        if (clear_screen)
        {
            #ifdef _WIN32
            clrscr();
            #else
            printf("\e[1;1H\e[2J");
            #endif
        }

        print_state(m->tape, init_pos, 40, m->pointer, curr_state->name);

        m->tape[m->pointer] = instruction->write;
        
        if (instruction->shift == LEFT)
        {
            if (m->pointer == 0)
            {
                fprintf(stderr, "pointer out of bounds\n");
                return 0;
            }
            m->pointer--;
        }
        else if (instruction->shift == RIGHT)
        {
            if (m->pointer == TAPE_SIZE-1)
            {
                fprintf(stderr, "pointer out of bounds\n");
                return 0;
            }
            m->pointer++;
        }

        curr_state = m->states[instruction->next_state];
        instruction = get_next_instruction(curr_state,  m->tape[m->pointer]);

        num_iterations++;
        if (delay)
            wait_ms(delay);
    }

    free(m->state_ids);
    free(m->states);

    if (instruction == NULL)
    {
        return -1;
    }

    return num_iterations;
}

Instruction* get_next_instruction(State* state, char value)
{
    Instruction* instruction = state->instructions;
    for (int i = 0; i < state->num_instructions; i++)
    {
        if (value == instruction->read)
            return instruction;
        instruction++;
    }

    return NULL;
}

void print_state(char* tape, unsigned int start_pos, int num_cells, unsigned int pointer, char* state_name)
{
    num_cells -= 10;
    for (int i = -10; i < num_cells; i++)
    {
        printf("%c ", *(tape + start_pos + i));
    }
    printf("%s\n", state_name);

    for (int i = -10; i < num_cells; i++)
    {
        if (pointer == start_pos + i)
            printf("^ ");
        else
            printf("  ");
    }
    printf("\n");
}

void wait_ms(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(1000 * ms);
#endif
}

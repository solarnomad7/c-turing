#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

#define MAX_TOKEN_LEN   50
#define RESERVED_CHARS  "{}:;<>=*\\\n"

typedef enum TokenType
{
    IDENTIFIER,
    VALUE,
    WILDCARD_VALUE,
    DIRECTION,
    SEPARATOR,
    COLON,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    NA,
} TokenType;

typedef struct Token
{
    TokenType type;
    char contents[MAX_TOKEN_LEN];
} Token;

Token* tokenize(char *s);
bool isreserved(char c);
char* read_file(char *name);

int parse(char *filename, State *states)
{
    State *first = states;

    int num_states = 0;

    char *raw = read_file(filename);
    Token *tokens = tokenize(raw);

    if (tokens == NULL)
        return 1;

    bool in_state = false;

    Token *t = tokens;
    while (t->type != NA)
    {
        if (t->type == IDENTIFIER)
        {
            if (in_state)
            {
                fprintf(stderr, "missing close bracket\n");
                return 0;
            }
            if (strlen(t->contents) > MAX_ID_LEN)
            {
                fprintf(stderr, "state identifier too long\n");
                return 0;
            }

            in_state = true;
            strcpy(states->name, t->contents);

            t++;
            if (t->type != OPEN_BRACKET)
            {
                fprintf(stderr, "missing open bracket\n");
                return 0;
            }
            t++;
        }
        else if (t->type == VALUE || t->type == WILDCARD_VALUE)
        {
            if (!in_state)
            {
                fprintf(stderr, "value found outside of state\n");
                return 0;
            }

            states->instructions[states->num_instructions].read = (t->type == VALUE) ? t->contents[0] : WILDCARD;
            t++;

            if (t->type != COLON)
            {
                fprintf(stderr, "instruction missing colon\n");
                return 0;
            }
            t++;

            if (t->type != VALUE && t->type != WILDCARD_VALUE)
            {
                fprintf(stderr, "instruction missing write value\n");
                return 0;
            }
            states->instructions[states->num_instructions].write = (t->type == VALUE) ? t->contents[0] : WILDCARD;
            t++;

            if (t->type != DIRECTION)
            {
                fprintf(stderr, "instruction missing shift direction\n");
                return 0;
            }
            switch (t->contents[0])
            {
                case '<':
                    states->instructions[states->num_instructions].shift = LEFT;
                    break;
                case '>':
                    states->instructions[states->num_instructions].shift = RIGHT;
                    break;
                case '=':
                    states->instructions[states->num_instructions].shift = NONE;
                    break;
            }
            t++;

            if (t->type != IDENTIFIER)
            {
                fprintf(stderr, "instruction missing next state\n");
                return 0;
            }
            if (strlen(t->contents) > MAX_ID_LEN)
            {
                fprintf(stderr, "state identifier too long\n");
                return 0;
            }
            strcpy(states->instructions[states->num_instructions++].next_state_name, t->contents);
            t++;

            if (t->type != SEPARATOR)
            {
                fprintf(stderr, "missing separator after instruction\n");
                return 0;
            }
            t++;
        }
        else if (t->type == CLOSE_BRACKET)
        {
            if (in_state)
            {
                in_state = false;
                states++;
                num_states++;
                t++;
            }
            else
            {
                fprintf(stderr, "bracket mismatch\n");
                return 0;
            }
        }
    }

    free(raw);
    free(tokens);

    states = first;
    return num_states;
}

Token* tokenize(char *s)
{
    int num_tokens = 0;
    int arr_used = 100;
    Token *tokens = malloc(arr_used * sizeof(Token));
    Token *first = tokens;

    bool escaping = false;
    bool comment = false;

    while (*s != '\0')
    {
        bool got_token = false;
        if (!escaping)
        {
            tokens->type = NA;
            if (*s == '<' || *s == '>' || *s == '=')
                tokens->type = DIRECTION;
            else if (*s == ';')
                tokens->type = SEPARATOR;
            else if (*s == ':')
                tokens->type = COLON;
            else if (*s == '{')
                tokens->type = OPEN_BRACKET;
            else if (*s == '}')
                tokens->type = CLOSE_BRACKET;
            else if (*s == '*')
                tokens->type = WILDCARD_VALUE;
            else if (*s == '\\')
            {
                escaping = true; // Treat the next character as a value
                got_token = true; // Not really, we just want to skip to the next character
            }

            if (tokens->type != NA)
            {
                tokens->contents[0] = *s;
                tokens->contents[1] = '\0';
                tokens++;
                num_tokens++;
                got_token = true; // Skip collecting identifier/value characters
            }
        }
        
        if (!got_token && !isspace(*s) && (escaping || !isreserved(*s)))
        {
            char id_contents[MAX_TOKEN_LEN];
            int i = 0;
            while (!isspace(*s) && (escaping || !isreserved(*s)))
            {
                id_contents[i++] = *s++;
                if (i == MAX_TOKEN_LEN)
                {
                    fprintf(stderr, "state identifier too long\n");
                    return NULL;
                }
                escaping = false;
            }

            id_contents[i] = '\0';
            s--;

            if (strlen(id_contents) == 1)
            {
                tokens->type = VALUE;
                tokens->contents[0] = *s;
                tokens->contents[1] = '\0';
                tokens++;
                num_tokens++;
            }
            else
            {
                tokens->type = IDENTIFIER;
                strcpy(tokens->contents, id_contents);
                tokens++;
                num_tokens++;
            }

            if (escaping)
                escaping = false;
        }

        s++;

        if (num_tokens == arr_used)
        {
            first = realloc(first, (arr_used + 100) * sizeof(Token));
            arr_used += 100;
        }
    }

    tokens->type = NA;
    return first;
}

bool isreserved(char c)
{
    char *r = RESERVED_CHARS;
    while (*r != '\0')
    {
        if (c == *r)
            return 1;
        r++;
    }

    return 0;
}

char* read_file(char *filename)
{
    char* buffer = NULL;
    long len;

    FILE *file;
    if ((file = fopen(filename, "rb")))
    {
        fseek(file, 0, SEEK_END);
        len = ftell(file);
        fseek(file, 0, SEEK_SET);

        buffer = malloc(len+1);
        if (buffer)
            fread(buffer, 1, len, file);
        
        fclose(file);
    }

    buffer[len] = '\0';

    return buffer;
}

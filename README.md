# c-turing

A fully programmable Turing machine simulator for Windows and Linux written in C.

# Usage

Use `make` to build the project.
Using the program: `turing file "input" [delay ms] [animate 0/1]`

# Turing Machine Language (TML)

C-Turing allows the programming of Turing machines using a very simple custom language called Turing Machine Language. TML code consists of one or more states containing a series of instructions. A small program that moves along a tape consisting of ones and zeroes until it reaches a one is shown below:

```
START {
  0: 0 > START;
  1: 1 > HALT;
}
```

Execution starts at the state `START` and ends once `HALT` is reached. State names must be at least two characters long and cannot contain any reserved characters: `{}<>=:*#`. Values can be reserved characters as long as they are preceded by the escape character `\`.

Instructions have the following syntax: `(read value): (write value) (pointer movement direction) (next state);`. Valid pointer movement directions are left (`<`), right (`>`), and none (`=`).

When used as a read value, the wildcard operator `*` functions as a kind of "else" statement, applying to all possible values excluding the ones already handled by other instructions. When used in place of the write value, the instruction does not replace the selected cell value, instead keeping it as is. Blank cells are defined as the underscore character `_`.

Comments begin with `#` and end at line breaks.

An example of a program that traverses a tape containing arbitrary characters, replacing each asterisk with an O, and returing back to the beginning after reaching the end:

```
START {
    *: * > START;   # Move right along the tape
    \*: O > START;  # Replace * with O
    _: _ < go_back;
}

go_back {
    *: * < go_back; # Move left along the tape
    _: _ > HALT;    # Halt upon reach a blank
}
```

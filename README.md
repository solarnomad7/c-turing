# c-turing

A fully programmable turing machine simulator for Windows and Linux written in C.

# Usage

Use `make` to build the project.
Using the program: `turing file "input" [delay ms] [animate 0/1]`

# Turing Machine Language (TML)

C-Turing allows the programming of turing machines using a very simple custom language called Turing Machine Language. TML code consists of one or more states containing a series of instructions. A small program that moves along a tape consisting of ones and zeroes until it reaches a one is shown below:

```
START {
  0: 0 > START;
  1: 1 > HALT;
}
```

Execution starts at the state `START` and ends once `HALT` is reached. State names must be at least two characters long and cannot (along with cell values) contain any reserved characters: `{}<>:`

Instructions have the following syntax: `(if cell value): (new value) (pointer movement direction) (next state);`

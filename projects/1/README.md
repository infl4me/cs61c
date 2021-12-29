# Part 1 — Assembler

In this part of the project, we will be writing an assembler that translates a subset of the MIPS instruction set to machine code. Our assembler is a two-pass assembler similar to the one described in lecture. However, we will only assemble the .text segment. At a high level, the functionality of our assembler can be divided as follows:

Pass 1: Reads the input (.s) file. Comments are stripped, pseudoinstructions are expanded, and the address of each label is recorded into the symbol table. Input validation of the labels and pseudoinstructions is performed here. The output is written to an intermediate (.int) file .

Pass 2: Reads the intermediate file and translates each instruction to machine code. Instruction syntax and arguments are validated at this step. The relocation table is generated, and the instructions, symbol table, and relocation table are written to an object (.out) file.

[full assignment](https://inst.eecs.berkeley.edu/~cs61c/sp15/projs/01/)


# Part 2 — Linker

In part 1 of this project, we wrote an assembler in C. Now, we will continue where we left off by implementing a linker in MIPS. The linker processes object files (which in our project are .out files) and generates an executable file. In the rest of this document, "input" will be used interchangeably with "object file", and "output" with "executable file".

The linker has two main tasks, combining code and relocating symbols. Code from each input file's .text segment is merged together to create an executable. This also determines the absolute address of each symbol (recall that the assembler outputs a symbol table containing the relative address of each symbol). Since the absolute address is known, instructions that rely on absolute addressing can have the addresses filled in.

[full assignment](https://inst.eecs.berkeley.edu//~cs61c/sp15/projs/01/part2.html)
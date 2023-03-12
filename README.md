CS 61C Solutions Repository
=======================

Current repository contains my solutions of Berkeley's [Computer Science 61c — machine structures course](https://hkn.eecs.berkeley.edu/courseguides/CS/61C). Assigments: [link](https://inst.eecs.berkeley.edu//~cs61c/sp15/)

## Overview:
The course focuses on concepts that drive computing architecture. Students will explore the software-hardware connection starting with high-level software with C and low-level assembly language with MIPS, as well as explore circuit and processor design. Students will then look at modern developments, such as different levels of parallelisms in computer processors, distributed computing, and warehouse scale computers (WSCs). Lastly, students will be introduced to basic operating system concepts. (Note: In some versions of the course OS concepts are covered before distributed computing and WSCs.)

## Completed tasks

### [Project 1.1: Assembler:](https://inst.eecs.berkeley.edu/~cs61c/sp15/projs/01/)
In this part of the project, we will be writing an assembler that translates a subset of the MIPS instruction set to machine code. Our assembler is a two-pass assembler similar to the one described in lecture. However, we will only assemble the .text segment. At a high level, the functionality of our assembler can be divided as follows:

Pass 1: Reads the input (.s) file. Comments are stripped, pseudoinstructions are expanded, and the address of each label is recorded into the symbol table. Input validation of the labels and pseudoinstructions is performed here. The output is written to an intermediate (.int) file .

Pass 2: Reads the intermediate file and translates each instruction to machine code. Instruction syntax and arguments are validated at this step. The relocation table is generated, and the instructions, symbol table, and relocation table are written to an object (.out) file.

### [Project 1.2: Linker:](https://inst.eecs.berkeley.edu//~cs61c/sp15/projs/01/part2.html)
The linker has two main tasks, combining code and relocating symbols. Code from each input file's .text segment is merged together to create an executable. This also determines the absolute address of each symbol (recall that the assembler outputs a symbol table containing the relative address of each symbol). Since the absolute address is known, instructions that rely on absolute addressing can have the addresses filled in.

### [Project 2: Processor Design:](https://inst.eecs.berkeley.edu//~cs61c/sp15/projs/02/index.html)
In this project you will be using Logisim to create a 32-bit two-cycle processor. It is similar to MIPS, except that memory addresses represent 32-bit words instead of 8-bit bytes (word-addressed instead of byte-addressed). Also, all addresses are 24-bits wide instead of 32-bits, due to limitations in Logisim. Throughout the implementation of this project, we'll be making design choices that make it compatible with machine code outputs from MARS and your Project 1!

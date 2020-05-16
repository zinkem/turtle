# turtle
A toy logo implementation for CS 355 at Washington State University

# Introduction

The turtle interpreter is designed to direct a 'turtle' to draw a picture.
Commands are read from stdin, and an image is drawn and output to stdout in pgm
format.

## Files included in this archive
```
Makefile        Makefile to build project
README.md       This file

interpreter.c   Code to perform ll(1) parse of code

scanner.c       Code to scan for next token, provided by Dr
scanner.h       Wayne Cochran

symtab.code     Symbol table provided by Dr Wayne Cochran
symtab.h

turtle.c        Code to control the turtle for drawing,
turtle.h        provided by Dr Wayne Cochran

code/	          Sample .turtle files containing turtle code
```
## Compiling and Executing

Make is required to compile the project.

In the root directory, type:

`make`

This will build an executable named `turtle` that can be used to interpret
turtle commands (code) and create images.

Turtle will take commands from `stdin` and output a pgm image to `stdout`

Turtle will accept commands directly, and output to file using the following
command:

`turtle > image.pgm`

This will take commands from `stdin` to draw with the turtle, `ctrl-D` stops the
interpretter and dumps a pgm to `image.pgm`

Source files can be piped into the interpretter on stdin.

`turtle < source.turtle > image.pgm`

The above command will generate an image `image.pgm` specified
by the commands in `source.turtle`


# The Turtle Language

### The Turtle

The turtle can be commanded to do different things, the commands are listed
below:

`HOME`          -- moves the turtle to the origin, facing east

`PENUP`	        -- tells the turtle to put it's 'pen' down, drawing where it moves 	 

`PENDOWN`	      -- tells turtle to stop drawing

`FORWARD expr`  -- move forward a distance specified by expr

`LEFT expr`     -- rotate expr degrees left

`RIGHT expr`    -- rotate expr degrees right

`PUSHSTATE`     -- record the turtles current state, and save it on the stack

`POPSTATE`      -- recall the top state on the stack

### Variables

All variables are floats, initialized to 0.0 the first time
they show up in code. They must begin with a letter and can contain
letters and numbers, but none of the reserved words.

`:=` Assignment operator

`FOO := 5` assigns a value of `5` to `FOO`

### Expressions

All expressions are infix. The `+`, `-`, `/`, and `*` symbols are all valid
operators that behave as expected. Parentheses may be used to specify
precedence. All expressions deal with Variables and literal values.

Examples:
```
6 + 7
FOO * 5
(5 + 8) * BAR
```

### Boolean expressions

Boolean expressions are a boolean operator with an Expression on
either side. Operators are: `=`, `NE`, `<` , `LE`, `>`, `GE` and the Unary
operator `NOT`.

Examples:
```
5 > FOO
4 NE 1
NOT (5 > 4)
```
If statements:
```
IF <bool>
THEN <stmt list>
{ ELSIF <bool> THEN <stmt list> }
[ ELSE <stmt list> ]
FI
```
For example:
```
IF <condition> THEN
...
FI
```
This is the simplest if statement, and performs ... if condition is true.

An ELSE can be added:
```
IF <condition> THEN
...
ELSE
...
FI
```

ELSE-IF chains:
```
IF <condition> THEN
ELSIF <condition> THEN ...
ELSIF <condition> THEN ...
```
While statements:
```
WHILE <bool> DO <stmt list> OD
```
As long as the bool expression is true, this will repeatedly execute the code
between DO and OD.


## Version Information

Turtle Language Interpretter
Version 1.0, released 2/10/11

Matthew Zinke
zinkem@gmail.com

Project for CS 355 at Washington State University
Taught by Dr Wayne Cochran

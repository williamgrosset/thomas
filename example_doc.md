# turtle
:turtle: A simple shell interpreter.

![alt text](https://github.com/williamgrosset/turtle/blob/master/example.gif "Shell")

## Overview
This project was an assignment for the [Operating Systems](https://github.com/williamgrosset/turtle/blob/master/csc360_a1.pdf) class at the University of Victoria. The simple shell supports the basic functionality of a Linux terminal shell (see below) and uses the following kernel system calls: `fork()`, `execvp()`, and `waitpid()`.

### Commands
The 200-line simple shell supports basic command execution, changing directories, and running background processes. To create a background process, run `bg <cmd> <opts>`. To view running background processes, run `bglist`. <br /><br />Neither `bg`, `bglist`, or `cd` commands are case-sensitive.

### Usage
#### Compile
```
make setup
```

#### Run shell
```
make run
```

#### Clean build
```
make clean
```

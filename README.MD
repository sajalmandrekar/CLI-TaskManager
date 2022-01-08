# MTASK - A Command line Task Manager

- MTask is a simple command line task manager
- Main features include:
    1. adding/delete new tasks
    2. mark tasks as complete
    3. display pending/completed tasks

## Installation:
1. download the source code
2. (optional) add to PATH for system wide use

## Execution:
### Linux/Unix:
type `./task help` to get help
```
Usage :-
$ ./task add 2 "hello world"    # Add a new item with priority 2 and text "hello world" to the list
$ ./task ls                   # Show incomplete priority list items sorted by priority in ascending order
$ ./task del INDEX            # Delete the incomplete item with the given index
$ ./task done INDEX           # Mark the incomplete item with the given index as complete
$ ./task help                 # Show usage
$ ./task report               # Statistics
$ ./task reset                # Clears all pending and completed tasks
```

### Windows:
Use `.\task.bat` instead of `./task`

## Contact developer:
- developed by: Sajal Mandrekar
- contact: sajalmandrekar@gmail.com
- twitter: @mandrekar_sajal
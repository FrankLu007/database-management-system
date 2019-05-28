# Simple DBMS                                                                                                   

This project is a simple DBMS implemented using C++, as the education material of the Introduction to Database Systems in National Chiao Tung University

## Prerequisite

* make >= 4.1
* gcc >= 5.4.0
* g++ >= 7.4.0
* python >= 3.6.0

## System test

`$ python3 test/system/system_test.py ./shell [test_case..]`

> if no test_cases is specified, it will run all system tests by default

## Compile

`$ make`

## Quick Start
`$ ./shell`

### Table schema
#### `User` table
| field_name | data_type |
|---|---|
| id | int |
| name | char[255] |
| email | char[255] |
| age | int |

### Supported command
| Command | Description |
|---|---| 
| insert | The insert command only allow insert one `User_t` record at a time |
| select | The select command will only print out all the `User_t` records in the current databases |


## Storage mode

### Memory only

The memory only will not store any record after the process terminate

#### Usage:
`$ ./shell`

### File mode

The File mode will store the record into the specify \<DB file\>, the
next execute can automatically reload the record by the same
execution command.

#### Usage:
`$ ./shell <DB file>`

## Project Layout
    ├─ src/               Source files
    │  ├─ main.cpp        Entry point, which contains main()
    │  ├─ table.cpp       Command handler
    │  ├─ condition.cpp   Condition analyzer
    │  └─ data.cpp        Data handler
    ├─ test/              Unit tests & system tests
    │  ├─ include/        Test header files
    │  ├─ system/         System tests
    │  └─ *.c             Unit tests source file
    └─ Makefile           Makefile for make

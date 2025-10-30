# DBMS_Min_Project
- working on toydb database (src code is in C)

---
---

### According to my understanding
1. `toydb/pflayer/` : Have some implementation, where we will build only object files of pflayer (also some executable for tests)
2. `toydb/amlayer/` : Have some implementation, where we will build object files and build executable `a.out` after linking with object files of pflayer.

---
---

## Make commands for `./toydb/Makefile` (first do `cd ./toydb/`)
1. To build object files and `a.out` in amlayer
```
make
```
2. To build and run `a.out` from amlayer
```
make run
```
3. To delete all object and executable files
```
make clean
```
4. Same working as `make clean` + clears the terminal
```
make clear
```

---

## Other Makefiles
- The above make commands are corresponding to Makefile in `./toydb/` which is made by me, but there are other Makefiles also in `./toydb/amlayer/` and `./toydb/pflayer/` which are provided by our Course Instructor.

---
---
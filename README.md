# DBMS\_Min\_Project

This repository contains a mini-project to build a toy database (ToyDB) from scratch in C. The project is divided into distinct layers, each with its own source code, headers, and Makefile.

## Project Structure

The `toydb/` directory contains the core database source code, organized into three layers:

  * **`toydb/pflayer` (Page File Layer):** The lowest layer, responsible for file I/O, managing pages on disk, and maintaining a buffer pool. It compiles into a single object file: `pflayer/build/pflayer.o`.
  * **`toydb/amlayer` (Access Method Layer):** The middle layer, built on top of the PF Layer. It implements the access methods for data (e.g., B+ Tree indexing) and handles record searching, insertion, and scanning. It compiles into `amlayer/build/amlayer.o`.
  * **`toydb/splayer` (System Page Layer):** The top layer, which likely manages system-level data or catalog information.

The root directory also contains:

  * `data/`: Sample `.txt` files used to populate the database.
  * `Reports/`: Project reports for the assignment objectives.

-----

## How to Build and Run

All commands should be run from the `toydb/` directory. The build system is designed around testing each layer individually.

### Running Tests (Recommended)

This is the primary way to build and run the code. The test targets will automatically build all necessary dependencies (e.g., running an `amlayer` test will first build the `pflayer`).

  * **Run all tests from all layers:**
    ```bash
    make testall
    ```
  * **Run specific `amlayer` tests (builds AM+PF layers):**
    ```bash
    make am_test1
    make am_test2
    make am_test3
    ```
  * **Run specific `pflayer` tests (builds PF layer only):**
    ```bash
    make pf_testpf
    make pf_testbuf
    make pf_testhash
    ```
  * **Run the `splayer` tests (builds SP+PF layer only):**
    ```bash
    make sp_testsp
    ```

### Cleaning

  * **Clean all builds:** Deletes all object files and test executables from all layers (`pflayer`, `amlayer`, `splayer`) and the root directory.
    ```bash
    make clean
    ```
  * **Clear and clean:** Runs the `clean` target and also clears the terminal screen.
    ```bash
    make clear
    ```

### Main Executable

> **Important Note:** The `make` and `make run` commands are based on the `toydb.out` target, which compiled using `main.c`. We have to add its functionalities e.g. query processor

  * **Build `toydb.out`:**
    ```bash
    make
    ```
  * **Run `toydb.out`:**
    ```bash
    make run
    ```
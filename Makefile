# This Makefile acts as a proxy, forwarding all commands to the Makefile inside the 'toydb' directory.
# Some make command info (defined in toydb/Makefile) :
#
# make (or make toydb) : Builds the main 'toydb.out' executable by building all layers.
# make run             : Builds and then runs 'toydb.out'.
# make clean           : Removes 'toydb.out' and all build artifacts from all layers.
# make clear           : Runs 'make clean' and then clears the terminal.
#
# Tests
#
# make testall         : Runs all tests from all layers (am, pf, and sp).
#
# make am_test1        : Runs 'test1' from the 'amlayer' directory.
# make am_test2        : Runs 'test2' from the 'amlayer' directory.
# make am_test3        : Runs 'test3' from the 'amlayer' directory.
#
# make pf_testpf       : Runs 'testpf' from the 'pflayer' directory.
# make pf_testhash     : Runs 'testhash' from the 'pflayer' directory.
# make pf_testbuf      : Runs 'testbuf' from the 'pflayer' directory.
#
# make sp_testsp       : Runs 'testsp' from the 'splayer' directory.
# make sp_testconvert  : Runs 'testconvert' from the 'splayer' directory.
# make sp_testfind     : Runs 'testfind' from the 'splayer' directory.
# make sp_testdelete   : Runs 'testdelete' from the 'splayer' directory.
# make sp_testscan     : Runs 'testscan' from the 'splayer' directory.
# make sp_testanalyze     : Runs 'testanalyze' from the 'splayer' directory.

SUBDIR = toydb

.PHONY: build clear

build:
	$(MAKE) -C $(SUBDIR)

clear:
	$(MAKE) -C $(SUBDIR) $@
	clear
%:
	$(MAKE) -C $(SUBDIR) $@
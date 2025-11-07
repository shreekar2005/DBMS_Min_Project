test: default
	cd amlayer && ./a.out
	
default:
	cd pflayer && make
	cd amlayer && make 

pf:
	cd pflayer && make

am:
	cd amlayer && make

loadtest1:
	cd amlayer && cp test1.c main.c
loadtest2:
	cd amlayer && cp test2.c main.c
loadtest3:
	cd amlayer && cp test3.c main.c

clear : clean
	clear
clean:
	cd amlayer && make clean
	cd pflayer && make clean
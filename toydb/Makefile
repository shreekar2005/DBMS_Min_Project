default:
	cd pflayer && make
	cd amlayer && make 

pf:
	cd pflayer && make

am:
	cd amlayer && make 

run: default
	cd amlayer && ./a.out

clear : clean
	clear
clean:
	cd amlayer && make clean
	cd pflayer && make clean

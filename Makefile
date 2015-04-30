all: event_keeper

event_keeper: event_keeper.c event.c
	gcc event_keeper.c event.c -Wall -o event_keeper

run: event_keeper
	./event_keeper

clean:
	rm event_keeper

clean_db:
	rm database.bin

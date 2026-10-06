mmult: src/mmult.c
	gcc src/mmult.c -lpthread -o bin/mmult

quicksort: src/quick_sort.c
	gcc src/quick_sort.c -lpthread -o bin/quicksort

diningphilosophers: src/dining_philosophers.c
	gcc src/dining_philosophers.c -lpthread -o bin/diningphilosophers
	
thesleepingbarber: src/the_sleeping_barber.c
	gcc src/the_sleeping_barber.c -lpthread -o bin/thesleepingbarber

threadpool: src/thread_pool.c
	gcc src/thread_pool.c -lpthread -o bin/threadpool

init:
	mkdir -p bin

clean:
	rm -rf bin/*
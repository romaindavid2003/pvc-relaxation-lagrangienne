OBJS = tsp.o relaxation.o 


tsp : $(OBJS)
	gcc -O3 -lm -o tsp $(OBJS) 
tsp.o: tsp.c tsp.h
	gcc -O3 -lm -c tsp.c 
relaxation.o: relaxation.c tsp.h  
	gcc -O3 -lm -c relaxation.c

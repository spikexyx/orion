CC=g++
CUDAINCLUDE=/usr/local/cuda/include/
CUDALIB=/usr/local/cuda/lib64

libintcuda.so: cuda_intercept.cpp
	$(CC) -O0 -fPIC -shared cuda_intercept.cpp -I$(CUDAINCLUDE) -L$(CUDALIB) -ldl -lcudart -lpthread -o libintcuda.so

all:
	make libintcuda.so

clean:
	rm -rf *.o libintcuda.so

sched: scheduler.cpp cuda_intercept.cpp
	$(CC) -o scheduler scheduler.cpp cuda_intercept.cpp -I$(CUDAINCLUDE) -L$(CUDALIB) -ldl -lcudart -lpthread
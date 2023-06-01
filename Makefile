all:
	nvcc -Xcompiler=-fopenmp -O3 -arch=sm_60 --device-c CudaGo.cu mcts_cuda.cu
	nvcc -Xcompiler=-fopenmp -O3 -arch=sm_60 --device-link CudaGo.o mcts_cuda.o --output-file link.o
	g++ -fopenmp -o hybrid2 main.cpp CudaGo.o mcts_cuda.o link.o -I/usr/local/cuda/include -L/usr/local/cuda/lib64/ -lcudart
clean:
	rm -rf *.o *.out

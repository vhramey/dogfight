
OBJ = GenePool.o Gun.o MyVector.o NeuralNet.o Neuron.o NeuronLayer.o Plane.o Quaternion.o point.o
CPP_H = GenePool.cpp GenePool.h Gun.cpp Gun.h MyVector.cpp MyVector.h NeuralNet.cpp NeuralNet.h Neuron.cpp Neuron.h NeuronLayer.cpp NeuronLayer.h Plane.cpp Plane.h Quaternion.cpp Quaternion.h main.cpp point.cpp point.h

FLAGS = -g -c

dogfight : $(OBJ) $(CPP_H)
	g++ main.cpp $(OBJ) -o dogfight -g -framework COCOA -framework GLUT -framework OpenGL

GenePool.o : GenePool.cpp GenePool.h
	g++ GenePool.cpp $(FLAGS)

Gun.o : Gun.cpp Gun.h
	g++ Gun.cpp $(FLAGS)

MyVector.o : MyVector.cpp MyVector.h
	g++ MyVector.cpp $(FLAGS)

NeuralNet.o : NeuralNet.cpp NeuralNet.h
	g++ NeuralNet.cpp $(FLAGS)

Neuron.o : Neuron.cpp Neuron.h
	g++ Neuron.cpp $(FLAGS)

NeuronLayer.o : NeuronLayer.cpp NeuronLayer.h
	g++ NeuronLayer.cpp $(FLAGS)

Plane.o : Plane.cpp Plane.h
	g++ Plane.cpp $(FLAGS)

Quaternion.o :  Quaternion.cpp Quaternion.h
	g++ Quaternion.cpp $(FLAGS)

point.o : point.cpp point.h
	g++ point.cpp $(FLAGS)

clean :
	rm *.o

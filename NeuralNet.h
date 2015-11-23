/*
 *  NeuralNet.h
 *  dogfight
 *
 *  Created by Nicholas Mastronarde on Wed Feb 05 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef NEURALNET_H
#define NEURALNET_H

#include "NeuronLayer.h"
#include <vector>
#include <fstream>
#include <math.h>
#include <iostream>

using namespace std;

class NeuralNet
{
    public:
        NeuralNet();
		~NeuralNet() { vecLayers.clear(); }
        vector< double > Update( vector< double > &inputs );
        void CreateNet();
        vector< double > GetWeights() const;
		inline int GetNumberOfWeights() {return numberOfWeights;} 
        void PutWeights( vector<double> &weights );
        inline double Sigmoid( double activation, double response );

    private:
        int	numInputs;
        int	numOutputs;
        int	numHiddenLayers;
        int	neuronsPerHiddenLayer;
		int numberOfWeights;
        ifstream infile;
        
        // storage for each layer of neurons including the output layer
        vector< NeuronLayer > vecLayers;
};

#endif
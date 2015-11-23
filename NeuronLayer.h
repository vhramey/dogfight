/*
 *  NeuronLayer.h
 *  dogfight
 *
 *  Created by Nicholas Mastronarde on Wed Feb 05 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef NEURONLAYER_H
#define NEURONLAYER_H

#include "Neuron.h"
#include <vector>

using namespace std;

class NeuronLayer
{
    public:
        NeuronLayer( int p_numNeurons, int p_numInputsPerNeuron );
		~NeuronLayer() { vecNeurons.clear(); }
        inline int getNumNeurons() const { return numNeurons; }
        // vector of neurons in the layer
        vector< Neuron > vecNeurons;

    private:
        // the number of neurons in the layer
        int numNeurons;
};

#endif
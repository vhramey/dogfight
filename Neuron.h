/*
 *  Neuron.h
 *  dogfight
 *
 *  Created by Nicholas Mastronarde on Wed Feb 05 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef NEURON_H
#define NEURON_H

#include <vector>
#include <cstdlib>

using namespace std;

class Neuron
{
    public:
    
        Neuron( int p_numInputs );
		~Neuron() { vecWeight.clear(); }
        inline int getNumInputs() const { return numInputs; }
        // the weights for each input
        vector< double > vecWeight;
        
    private:
        inline double small_random() { return (double)(rand() % 4000 - 2000) / 2000.0; }
        // the number of inputs into the neuron
        int	numInputs;
};

#endif
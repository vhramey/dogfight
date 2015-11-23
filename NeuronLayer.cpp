/*
 *  NeuronLayer.cpp
 *  dogfight
 *
 *  Created by Nicholas Mastronarde on Wed Feb 05 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include "NeuronLayer.h"

NeuronLayer::NeuronLayer( int p_numNeurons, int p_numInputsPerNeuron )
{
    numNeurons = p_numNeurons;
    
    for( int i = 0; i < p_numNeurons; i++ )
    {
        // place neurons in vecNeurons vector
        vecNeurons.push_back( Neuron( p_numInputsPerNeuron ) );
    }
}
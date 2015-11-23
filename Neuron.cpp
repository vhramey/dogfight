/*
 *  Neuron.cpp
 *  dogfight
 *
 *  Created by Nicholas Mastronarde on Wed Feb 05 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include "Neuron.h"


Neuron::Neuron( int p_numInputs )
{
	
	numInputs = p_numInputs + 1;
    // we need an additional weight for the threshold hence the +1
    for( int i = 0; i < p_numInputs + 1; i++ )
    {
        // set up the weights with an initial random value
        vecWeight.push_back( small_random() );
    }
}
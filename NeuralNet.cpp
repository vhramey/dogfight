/*
 *  NeuralNet.cpp
 *  dogfight
 *
 *  Created by Nicholas Mastronarde on Wed Feb 05 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include "NeuralNet.h"
#include <string>
#include <fstream>


using namespace std;

NeuralNet::NeuralNet()
{
	
	//*
    string tempString;
    infile.open("par.ini");
    if( !infile )
        printf( "Error opening file");
	getline(infile, tempString, '\n'); // discards filename
    infile >> numInputs;
    getline(infile, tempString, '\n');
    infile >> numOutputs;
    getline(infile, tempString, '\n');
    infile >> numHiddenLayers;
    getline(infile, tempString, '\n');
    infile >> neuronsPerHiddenLayer;
    getline(infile, tempString, '\n');
	// before closing, load filename of neural net
	infile.close();
	
	numberOfWeights = numInputs * neuronsPerHiddenLayer 
		+ neuronsPerHiddenLayer * (numHiddenLayers - 1) + neuronsPerHiddenLayer * numHiddenLayers
		+ neuronsPerHiddenLayer * numOutputs + numOutputs;

	/*
    cout << "numInputs: " << numInputs << " numOutputs: " << numOutputs 
        << " numHiddenLayers: " << numHiddenLayers << " neuronsPerHiddenLayer: "
        << neuronsPerHiddenLayer << endl;
	
    infile.close();
	//*/
	/*
	numInputs = 1;
	numOutputs = 5;
	numHiddenLayers = 1;
	neuronsPerHiddenLayer = 10;
    //*/
    CreateNet();
}

void NeuralNet::CreateNet()
{
    // create the layers of the network
    if( numHiddenLayers > 0)
    {
        // create first hidden layer
        vecLayers.push_back( NeuronLayer( neuronsPerHiddenLayer, numInputs ) );
        
        for( int i = 0; i < numHiddenLayers - 1; i++ )
        {
            vecLayers.push_back( NeuronLayer( neuronsPerHiddenLayer, neuronsPerHiddenLayer) );
        }
        
        // create output layer
        vecLayers.push_back( NeuronLayer( numOutputs, neuronsPerHiddenLayer ) );
    }
    else
    {
        // create output layer
        vecLayers.push_back( NeuronLayer( numOutputs, numInputs ) );
    }
}

// returns empty vector if there is an error
// otherwise, returns a vector of outputs
vector< double > NeuralNet::Update(vector< double > &inputs)
{
    // stores the resultant outputs from each layer
    vector< double > outputs;
    int weight = 0;
    
    // first check that we have the correct amount of inputs
    if( inputs.size() != numInputs )
    {
        // just return an empty vector if incorrect.
		printf("NNerror %d %d", inputs.size(), numInputs);
        return outputs;
    }
    
    // For each layer...
    for( int i = 0; i < numHiddenLayers + 1; i++ )
    {
        if( i > 0 )
        {
            // outputs of previous layer are inputs of new layer
            inputs = outputs;
        }
        
        outputs.clear();
        weight = 0;
        
        // for each neuron sum the inptus * corresponding weights. Throw
        // the total at the sigmoid function to get the output.
        for( int j = 0; j < vecLayers[i].getNumNeurons(); j++ )
        {
            double netinput = 0;
            
            int tempNumInputs = vecLayers[i].vecNeurons[j].getNumInputs();
            
            // for each weight
			
            for( int k = 0; k < tempNumInputs - 1; k++ )
            {
                // sum the weights x inputs
                netinput += vecLayers[i].vecNeurons[j].vecWeight[k] * inputs[k];
            }
            
            // add in the bias
            netinput -= vecLayers[i].vecNeurons[j].vecWeight[tempNumInputs-1];
            
            // we can store the outputs from each layer as we generate them.
            // The combined activation is first filtered through the sigmoid
            // function
            outputs.push_back(Sigmoid(netinput, 1/*activation response*/));
            weight = 0;
        }
    }
    
    return outputs;
}

vector< double > NeuralNet::GetWeights() const
{
    vector< double > weights;
	double checksum = 0;
    
    // for each layer
    for( int i = 0; i < numHiddenLayers + 1; i++ )
    {
        //for each neuron
        for( int j = 0; j < vecLayers[i].getNumNeurons(); j++ )
        {
            //for each weight
            for( int k = 0; k < vecLayers[i].vecNeurons[j].getNumInputs(); k++ )
            {
                weights.push_back( vecLayers[i].vecNeurons[j].vecWeight[k] );
				/*printf("Get vecLayers[i].vecNeurons[j].vecWeight[k] = %f\n", 
					vecLayers[i].vecNeurons[j].vecWeight[k] );*/
				checksum += vecLayers[i].vecNeurons[j].vecWeight[k];
            }
        }
    }
    //printf("Get checksum = %f\n", checksum);
    return weights;
}

void NeuralNet::PutWeights( vector<double> &weights )
{
    // weight count
    int index = 0;
	double checksum = 0;
    // for each layer
    for( int i = 0; i < numHiddenLayers + 1; i++ )
    {
        //for each neuron
        for( int j = 0; j < vecLayers[i].getNumNeurons(); j++ )
        {
            //for each weight
            for( int k = 0; k < vecLayers[i].vecNeurons[j].getNumInputs(); k++ )
            {
                vecLayers[i].vecNeurons[j].vecWeight[k] = weights[index++];
				/*printf("Put vecLayers[i].vecNeurons[j].vecWeight[k] = %f\n", 
					vecLayers[i].vecNeurons[j].vecWeight[k] );*/
				checksum += vecLayers[i].vecNeurons[j].vecWeight[k];
            }
        }
    }
	//printf("Put checksum = %f\n", checksum);
}
    

// Sigmoid activation curve
double NeuralNet::Sigmoid(double activation, double response)
{
	return ( 1 / ( 1 + exp(-activation / response)));
}
                        
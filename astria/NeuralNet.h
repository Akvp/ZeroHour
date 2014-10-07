#ifndef _NEURAL_NETWORK_H_
#define _NEURAL_NETWORK_H_

#include <vector>
#include "utils.h"
#include "Params.h"

using namespace std;

struct SNeuron
{
	//number of inputs going into the neuron
	int NumInputs;
	//weight of each input
	vector<double> Weight;
	//constructor
	SNeuron(int numInputs);
};

struct SNeuronLayer
{
	int NumNeurons;
	vector<SNeuron> Neurons;
	SNeuronLayer(int numNeurons, int numInputsPerNeuron);
};

class CNeuralNet
{
public:
	CNeuralNet(int Input, int Output, int HiddenLayer, int NeuronsPerHiddenLayer);
	//create the net
	void Create();
	//get the weights from the neuralnetwork
	vector<double> GetWeights() const;
	//return the total number of weights in the net
	int GetNumberOfWeights() const;
	//replaces the weights with new ones
	void SetWeights(vector<double> & weights);
	//calculates the outputs from a set of inputs
	vector<double> Update(vector<double> & inputs);
	//sigmoid response curve
	inline double Sigmoid(double netInput, double activationResponse);

private:
	int NumInputs;
	int NumOutputs;
	int NumHiddenLayers;
	int NeuronsPerHiddenLayer;
	//storage for each layer of neurons including the output layer
	vector<SNeuronLayer> Layers;
};

#endif
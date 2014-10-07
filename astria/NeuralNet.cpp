#include "NeuralNet.h"

// =============================
// ====== SNeuron Methods ======
// =============================

SNeuron::SNeuron(int numInputs) : NumInputs(numInputs+1)
{
	for (int i = 0; i < numInputs + 1; i++)
	{
		Weight.push_back(RandNormal());
	}
}

// ===================================
// ====== SNeuronLayer Methods =======
// ===================================

SNeuronLayer::SNeuronLayer(int numNeurons, int numInputsPerNeuron) : NumNeurons(numNeurons)
{
	for (int i = 0; i < numNeurons; i++)
	{
		Neurons.push_back(SNeuron(numInputsPerNeuron));
	}
}

// ================================
// ====== CNeuralNet Methods ======
// ================================

// Setup a neural network
CNeuralNet::CNeuralNet(int Input, int Output, int Hidden, int NeuronsPerHidden)
{
	NumInputs = Input;
	NumOutputs = Output;
	NumHiddenLayers = Hidden;
	NeuronsPerHiddenLayer = NeuronsPerHidden;

	Create();
}

// Create a neural network
void CNeuralNet::Create()
{
	//create the layers of the network
	if (NumHiddenLayers > 0)
	{
		//create the first hidden layer
		Layers.push_back(SNeuronLayer(NeuronsPerHiddenLayer, NumInputs));
		
		for (int i = 0; i < NumHiddenLayers - 1; i++)
		{
			Layers.push_back(SNeuronLayer(NeuronsPerHiddenLayer, NeuronsPerHiddenLayer));
		}
		//create output layer
		Layers.push_back(SNeuronLayer(NumOutputs, NeuronsPerHiddenLayer));
	}
	else
	{
		//create output layer
		Layers.push_back(SNeuronLayer(NumOutputs, NumInputs));
	}
}

// Returns a vector containing the weights
vector<double> CNeuralNet::GetWeights() const
{
	//hold all the weights
	vector<double> weights;

	//for each layer
	for (int i = 0; i < NumHiddenLayers + 1; i++)
	{
		//for each neuron
		for (int j = 0; j < Layers[i].NumNeurons; j++)
		{
			//for each weight
			for (int k = 0; k < Layers[i].Neurons[i].NumInputs; k++)
			{
				weights.push_back(Layers[i].Neurons[j].Weight[k]);
			}
		}
	}
	return weights;
}

// Replace the weights in the NN with provided weight vector
void CNeuralNet::SetWeights(vector<double> &weights)
{
	int weightIndex = 0;

	//for each layer
	for (int i = 0; i < NumHiddenLayers + 1; i++)
	{
		//for each neuron
		for (int j = 0; j < Layers[i].NumNeurons; j++)
		{
			//for each weight
			for (int k = 0; k < Layers[i].Neurons[i].NumInputs; k++)
			{
				Layers[i].Neurons[j].Weight[k] = weights[weightIndex];
				weightIndex++;
			}
		}
	}
}

// Return the total number of weights needed for the net
int CNeuralNet::GetNumberOfWeights() const
{
	int NumWeights = 0;
	//for each layer
	for (int i = 0; i < NumHiddenLayers + 1; i++)
	{
		//for each neuron
		for (int j = 0; j < Layers[i].NumNeurons; j++)
		{
			//for each weight
			for (int k = 0; k < Layers[i].Neurons[i].NumInputs; k++)
			{
				NumWeights++;
			}
		}
	}
	return NumWeights;
}

// Returns a output vector given an input vector
vector<double> CNeuralNet::Update(vector<double> & inputs)
{
	//stores the resultant output from each layer
	vector<double> outputs;
	int cWeight = 0;

	//first check that we have the correct amount of inputs
	if (inputs.size() != NumInputs)
	{
		//returns empty vector
		return outputs;
	}

	//for each layer
	for (int i = 0; i < NumHiddenLayers + 1; i++)
	{
		if (i > 0)
		{
			inputs = outputs;
		}
		outputs.clear();
		cWeight = 0;

		//for each neuron sum the (inputs * corresponding weights)
		//throw the total at our sigmoid function to get the ouput
		for (int j = 0; j < Layers[i].NumNeurons; j++)
		{
			double netInput = 0;
			
			int NumInputs = Layers[i].Neurons[j].NumInputs;

			//for each weight
			for (int k = 0; k < NumInputs - 1; k++)
			{
				//sum the weights * inputs
				netInput += Layers[i].Neurons[j].Weight[k] * inputs[cWeight++];
			}
			//add in the bias
			netInput += Layers[i].Neurons[j].Weight[NumInputs - 1] * CParams::Bias;

			//we store the outputs from each layer as we generate them
			//the combined activation is first filtered through the sigmoid function
			outputs.push_back(Sigmoid(netInput, CParams::ActivationResponse));

			cWeight = 0;
		}
	}
	return outputs;
}

// Sigmoid function
double CNeuralNet::Sigmoid(double netInput, double activationResponse)
{
	return (1 / (1 + exp(-netInput / activationResponse)));
}
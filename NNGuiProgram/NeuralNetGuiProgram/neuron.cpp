/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#include "neuron.h"
#include "customnn.h"


using namespace std;

//static double eta  = 0.15; //overall good learning rate
//static double alpha = 0.5; //also a moderate choice

Neuron::Neuron(unsigned numOutputs, unsigned myIndex)
{
    //index appears to be the layer number
    for (unsigned CurrentConnection = 0; CurrentConnection < numOutputs; CurrentConnection++)
    {
        //recall that connection is a struct which contains both the weight and the delta weight
        m_OutputWeights.push_back(Connection());
        //here we set the weight of the relevant neurons weight as a random number
        m_OutputWeights.back().Weight = randomWeight();
    }
    m_myIndex = myIndex;
}

Neuron::Neuron(unsigned numOutputs, unsigned myIndex,vector<float> optimalWeights)
{
    //index appears to be the layer number
    for (unsigned CurrentConnection = 0; CurrentConnection < numOutputs; CurrentConnection++)
    {
        //recall that connection is a struct which contains both the weight and the delta weight
        m_OutputWeights.push_back(Connection());
        //here we set the weight of the relevant neurons weight as a random number
        m_OutputWeights.back().Weight = double(optimalWeights.at(CurrentConnection));
    }
    m_myIndex = myIndex;
}


void Neuron::NeuronFeedforward(const Layer &prevLayer, int activationFunctionType)
{
    double sum = 0.0;

    //sum the previous layers outputs (which are our inputs)
    //include the bias node from the previous layer

    for(unsigned CurrentNeuron = 0;CurrentNeuron < prevLayer.size();CurrentNeuron++)
    {
        //using outputval nullifies having to use the get and set outputval functions
        sum += prevLayer[CurrentNeuron].m_OutputVal *
                prevLayer[CurrentNeuron].m_OutputWeights[m_myIndex].Weight;
    }
    //also known as the application of the activation function
    m_OutputVal = Neuron::transferFunction(sum,activationFunctionType);
}

double Neuron::transferFunction(double x, int selectedFunction)
{
     //output must be between -1 and 1 so we use tanh

    switch (selectedFunction)
    {
    case 0:
        return tanh(x);
        break;

    case 1:
        //actually the ReLeu Function Seems more popular
        //return (1/(1+exp(-x)));
        if(x > 0)
        {
            return x;
        }
        else
        {
            return 0;
        }
        break;

    case 2:
        //apparently this is some source code for leaky ReLu
        if(x>double(x)*0.03)
        {
            return x;
        }
        else
        {
            return double(x)*0.03;
        }
        break;
    }

}

double Neuron::transferFunctionDerivative(double x,int selectedFunction)
{

    switch (selectedFunction)
    {
    case 0:
        return (1 - x*x);
        break;

    case 1:
        //actually the ReLeu Function Seems more popular
        //return (1/(1+exp(-x)))/(1 - (1/(1+exp(-x))));
        if(x > 0)
        {
            return 1;
        }
        else
        {
            return 0;//this seems to be a common alpha
        }
        break;

    case 2:
        //apparently this is some source code for leaky ReLu
        if(x>0)
        {
            return 1;
        }
        else
        {
            return 0.03;
        }
        break;
    }
     //return (1- tanh(x)*tanh(x)); //this is not doable right now but we will change this later
    //for some reason he used this approximation
    //return (1 - x*x);
    //return (2*x);
}

double Neuron::sumDOW(const Layer &nextLayer) const
{
    double sum = 0.0;

    // Sum our contributions of the errors that we feed to the next layer (of each node)
    // this does not include the bias

    for (unsigned n = 0; n < nextLayer.size() -1; n++)
    {
     sum += m_OutputWeights[n].Weight * nextLayer[n].m_gradient;
    }

    return sum;
}

void Neuron::calcHiddenGradients(const Layer &nextLayer, int activationFunctionType)
{
    //this allows us to keep reducing net error
    double DOW = sumDOW(nextLayer);
    m_gradient = DOW * Neuron::transferFunctionDerivative(m_OutputVal,activationFunctionType);
}

void Neuron::calcOutputGradients(double TargetVal, int activationFunctionType)
{
    //this allows us to keep reducing net error
    double delta = TargetVal - m_OutputVal;
    m_gradient = delta * Neuron::transferFunctionDerivative(m_OutputVal,activationFunctionType);
}

void Neuron::updateWeights(Layer &prevLayer,double dynamicEta,double dynamicAlpha)
{
    //The weights to be updated are in the Connection container in the neurons preceding layer
    //this means that what is really updated is the activation value of each neuron in the current layer
    //as determined bby the argument given to the activation function which is based on the weight
    //coefficient for the previous layer
    for (unsigned n = 0; n < prevLayer.size(); n++)
    {
        Neuron &neuronOfPrevLayer = prevLayer[n];
        double oldDelta = neuronOfPrevLayer.m_OutputWeights[m_myIndex].DeltaWeight;

        // individueal input magnified by the gradient and train rate (eta and alpha)
        // note that a value for eta (learning rate): (0.0 - slow learner,0.2 - medium learner,1.0 reckless learner)
        // note that for alpha (momentum): (0.0 - no momemtum,0.5 -moderate momemtum)
        // here we pass custom variables to change the weights

        double newDelta =
                // Individual input, magnified by the gradient and train rate:
                dynamicEta
                * neuronOfPrevLayer.GetOutputVal()
                * m_gradient
                // Also add momentum = a fraction of the previous delta weight;
                + dynamicAlpha
                * oldDelta;
        //this is where we modify and update our weights
        neuronOfPrevLayer.m_OutputWeights[m_myIndex].DeltaWeight = newDelta;
        neuronOfPrevLayer.m_OutputWeights[m_myIndex].Weight += newDelta;
    }
}

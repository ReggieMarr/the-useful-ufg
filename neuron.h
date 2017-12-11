#ifndef NEURON_H
#define NEURON_H
//#include <customnn.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
//#include "executeneuralnet.h"
using namespace std;
//each connection may also be refered to as a synapses
//and is the bound between two neurons of different layers
//this is where the weight is applied
struct Connection
{
    double Weight;
    double DeltaWeight;
};


//we defined Neuron and classes in this feedforward manner such that we can call neurons with reference to the layer
//that they reside on (i.e. CurrentNeuron = m_layers[layernum][neuronnum])
class Neuron;
class ExecuteNeuralNet;

typedef vector<Neuron> Layer;
//typedef vector<Layer> Model;

// ********Class Neuron ********

class Neuron
{
public:

    //Neuron Neuronforotherclasses;
    Neuron(unsigned numOutputs, unsigned m_myIndex);
    Neuron(unsigned numOutputs, unsigned m_myIndex,vector<float> optimalWeights);
    Neuron() {}

    //again since we're not modifying anything we can use a const qualifier
    void setOutputVal(double val)
    {
        m_OutputVal = val;
    }

    double GetOutputVal(void) const
    {
        return m_OutputVal;
    }
    //this was originally called Feedforward in the example video but the naming convention is confusing
    void NeuronFeedforward(const Layer &prevLayer,int activationFunctionType);
    void calcOutputGradients(double TargetVal,int activationFunctionType);
    //making this static to rid errors (not original)
    void calcHiddenGradients(const Layer &nextLayer,int activationFunctionType);
    //updateWeights cant be const qualified as it will modify values
    void updateWeights(Layer &prevLayer, double dynamicEta, double dynamicAlpha);
    double eta;//  = 0.25; //overall good learning rate
    double alpha;// = 0.6; //also a moderate choice
    vector<Connection> m_OutputWeights;
    double m_OutputVal;

private:

    //returns a random number between 0 and 1
    static double randomWeight(void)
    {
        return rand()/double(RAND_MAX);
    }

    static double transferFunction(double x,int selectedFunction);
    static double transferFunctionDerivative(double x, int selectedFunction);
    double sumDOW(const Layer &nextLayer) const;
    //double m_OutputVal;
    unsigned m_myIndex;
    //vector<Connection> m_OutputWeights;
    double m_gradient;
};

#endif // NEURON_H

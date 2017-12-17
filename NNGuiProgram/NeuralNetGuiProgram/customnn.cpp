#include "customnn.h"
#include "neuron.h"

//double m_recentAverageSmoothingFactor = 100.0; // Number of training samples to average over

CustomNN::CustomNN(const vector<unsigned> &Topology)
{

    unsigned NumberofLayers = Topology.size();

    for (unsigned CurrentLayerNum = 0; CurrentLayerNum < NumberofLayers;CurrentLayerNum++)
    {
        m_layers.push_back(Layer());
        //apparently this sets the output number to be 0 if it is the outputlayer and to the correct
        //output num otherwise
        //it works but its fuckin weird
        unsigned numOutputs = CurrentLayerNum == Topology.size()-1 ? 0 : Topology[CurrentLayerNum+1];

        //we have made a new layer, we now want to fill it with neurons and add a bias neuron to the layer
        for(unsigned CurrentNeuronNum = 0; CurrentNeuronNum <= Topology[CurrentLayerNum]; CurrentNeuronNum++)
        {
            m_layers.back().push_back(Neuron(numOutputs,CurrentNeuronNum));
            //cout << "Made " << CurrentNeuronNum << " Neurons on layer " << CurrentLayerNum << endl;
        }
        //Force the bias node to 1.0 (its the last neuron created)
        m_layers.back().back().setOutputVal(1.0);
    }
}

CustomNN::CustomNN(const vector<unsigned> &Topology,runtimeModel optimalModel)
{

    unsigned NumberofLayers = Topology.size();

    for (unsigned CurrentLayerNum = 0; CurrentLayerNum < NumberofLayers;CurrentLayerNum++)
    {
        m_layers.push_back(Layer());
        //apparently this sets the output number to be 0 if it is the outputlayer and to the correct
        //output num otherwise
        //it works but its fuckin weird
        unsigned numOutputs = CurrentLayerNum == Topology.size()-1 ? 0 : Topology[CurrentLayerNum+1];

        //we have made a new layer, we now want to fill it with neurons and add a bias neuron to the layer
        for(unsigned CurrentNeuronNum = 0; CurrentNeuronNum <= Topology[CurrentLayerNum]; CurrentNeuronNum++)
        {
            m_layers.back().push_back(Neuron(numOutputs,CurrentNeuronNum,optimalModel[CurrentLayerNum][CurrentNeuronNum]));
            //cout << "Made " << CurrentNeuronNum << " Neurons on layer " << CurrentLayerNum << endl;
        }
        //Force the bias node to 1.0 (its the last neuron created)
        m_layers.back().back().setOutputVal(1.0);
    }
}

void CustomNN::Feedforward(const vector<double> &InputVals, int activationFunctionType)
{
    //we use assert here to test that what we expect to be true is in fact correct
    //we subtract 1 to account for the bias neuron
    assert(InputVals.size() == m_layers[0].size()-1);

    //Assign (latch) the input values to the neurons
    for (unsigned i = 0; i < InputVals.size(); i++)
    {
        m_layers[0][i].setOutputVal(InputVals[i]);
    }
    //Forward Propogate
    //this goes through each neuron in each layer (minus the bias neuron) and moves the value forward a layer (skipping the input layer)
    for (unsigned layerNum = 1;layerNum<m_layers.size();layerNum++)
    {
        Layer &prevLayer = m_layers[layerNum-1];
        for (unsigned CurrentNeuron = 0; CurrentNeuron<m_layers[layerNum].size()-1;CurrentNeuron++)
        {
            //class neuron is responsible for the nitty gritty math
            m_layers[layerNum][CurrentNeuron].NeuronFeedforward(prevLayer,activationFunctionType);
        }
    }
}


//this is to be more system modelling
void CustomNN::BackPropogation(const vector<double> &TargetVals, double learnToEta,
                               double momentumToAlpha, double memoryRangeToSmoothingFactor,
                               int activationFunctionType)
{
    /* Calculate overal net error (Root Mean Square Error)
       n is meant to represent currentneuron in this scope
    */

    Layer &OutputLayer = m_layers.back();
    //initialized to zero since this is a new training session
    m_error = 0.0;

    for (unsigned n = 0; n < OutputLayer.size()-1;n++)
    {
        double delta = TargetVals[n]- OutputLayer[n].GetOutputVal();
        m_error += delta * delta;
    }

    m_error /= OutputLayer.size() - 1;
    m_error = sqrt(m_error); //This is the RMS

    // Implement a recent average measurement
    //he uses 100 and the git uses 0.5, 100 seems to work better
    m_recentAverageSmoothingFactor = memoryRangeToSmoothingFactor;
    m_recentAverageError =
            (m_recentAverageError * m_recentAverageSmoothingFactor + m_error)
            / (m_recentAverageSmoothingFactor + 1.0);

    //Calculate output layer gradients
    for (unsigned n = 0; n < OutputLayer.size()-1;n++)
    {
     //recall that the neuron class has to do the nitty gritty math
     OutputLayer[n].calcOutputGradients(TargetVals[n],activationFunctionType);
    }
    //Calculate gradients on hidden layers
    for (unsigned layerNum = m_layers.size() - 2; layerNum > 0; layerNum--)
    {
        Layer &hiddenLayer = m_layers[layerNum];
        Layer &nextLayer = m_layers[layerNum+1];

        for (unsigned n = 0;n < hiddenLayer.size();n++)
        {
            hiddenLayer[n].calcHiddenGradients(nextLayer,activationFunctionType);
        }
    }
    //For all layer from outputs to first hidden layer
    //update connection weights

    for (unsigned layerNum = m_layers.size() - 1;layerNum > 0; layerNum--)
    {
        Layer &CurrentLayer = m_layers[layerNum];
        Layer &prevLayer = m_layers[layerNum -1];

        for (unsigned n = 0;n<CurrentLayer.size() - 1; n++)
        {
            CurrentLayer[n].updateWeights(prevLayer,learnToEta,momentumToAlpha);
        }
    }
}

//This is to be used for efficiency training
void CustomNN::BackPropogation(const vector<double> &TargetVals, double learnToEta,
                               double momentumToAlpha, double memoryRangeToSmoothingFactor,
                               int activationFunctionType,const vector<int> Outputs,const vector<int> Inputs)
{
    /* Calculate overal net error (Root Mean Square Error)
       n is meant to represent currentneuron in this scope
    */

    Layer &OutputLayer = m_layers.back();

    double sumInverseEfficiency = 0.0;

    for (uint cycleMatches = 0;cycleMatches < Outputs.size();cycleMatches++)
    {
        sumInverseEfficiency += Inputs.at(cycleMatches) / Outputs.at(cycleMatches);
    }

    m_recentAverageError = sumInverseEfficiency;
    //Calculate output layer gradients
    for (unsigned n = 0; n < OutputLayer.size()-1;n++)
    {
     //recall that the neuron class has to do the nitty gritty math
     OutputLayer[n].calcOutputGradients(TargetVals[n],activationFunctionType);
    }
    //Calculate gradients on hidden layers
    for (unsigned layerNum = m_layers.size() - 2; layerNum > 0; layerNum--)
    {
        Layer &hiddenLayer = m_layers[layerNum];
        Layer &nextLayer = m_layers[layerNum+1];

        for (unsigned n = 0;n < hiddenLayer.size();n++)
        {
            hiddenLayer[n].calcHiddenGradients(nextLayer,activationFunctionType);
        }
    }
    //For all layer from outputs to first hidden layer
    //update connection weights

    for (unsigned layerNum = m_layers.size() - 1;layerNum > 0; layerNum--)
    {
        Layer &CurrentLayer = m_layers[layerNum];
        Layer &prevLayer = m_layers[layerNum -1];

        for (unsigned n = 0;n<CurrentLayer.size() - 1; n++)
        {
            CurrentLayer[n].updateWeights(prevLayer,learnToEta,momentumToAlpha);
        }
    }
}

void CustomNN::GetResults(vector<double> &ResultVals) const
{
    ResultVals.clear();

    for (unsigned n = 0; n < m_layers.back().size() - 1; n++)
    {
        ResultVals.push_back(m_layers.back()[n].GetOutputVal());
    }

}

//for (unsigned layerNum = m_layers.size() - 1;layerNum > 0; layerNum--)
//{
//    Layer &CurrentLayer = m_layers[layerNum];
//    Layer &prevLayer = m_layers[layerNum -1];

//    for (unsigned n = 0;n<CurrentLayer.size() - 1; n++)
//    {
//        CurrentLayer[n].updateWeights(prevLayer,learnToEta,momentumToAlpha);
//    }
//}

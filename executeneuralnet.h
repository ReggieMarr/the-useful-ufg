#ifndef EXECUTENEURALNET_H
#define EXECUTENEURALNET_H
#include <vector>
#include <neuron.h>
#include <QCoreApplication>
#include "modbus/modbus.h"
#include "modbus/modbus-tcp.h"
#include "modbus/modbus-version.h"

using namespace std;

class Neuron;

typedef vector<vector<vector<float>>> runtimeModel;

class ExecuteNeuralNet
{

public:

    //we pass multiple values by reference and call the function as a void so that
    //we can manipulate multiple variables at once


    void  TrainNN(vector<double> &smoothedError,vector<vector<double>> &targetOutput,
                vector<vector<double>> &NNOutput,double learningRate, double momentum,
                float memoryRange, string filename, int selectedFunction,
                bool mySQLMode,vector<vector<Layer>> &NNModelLayers);

    void  TrainNN(vector<double> &smoothedError, vector<vector<double>> &targetOutput,
                vector<vector<double>> &NNOutput,double learningRate, double momentum,
                float memoryRange, string filename, int selectedFunction, bool mySQLMode,
                vector<vector<Layer>> &NNModelLayers, int numberOfHiddenLayers, int firstLayerNeurons);

    void  RunNN(vector<double> &smoothedError,vector<vector<double>> &targetOutput,
                vector<vector<double>> &NNOutput,double learningRate, double momentum,
                float memoryRange, string filename, int selectedFunction,
                bool mySQLMode,vector<vector<Layer>> &NNModelLayers,runtimeModel receivedModel);

    void  RunNN(vector<vector<double> > &NNOutput, int selectedFunction, runtimeModel receivedModel,
                 int manualVal1, int manualVal2, bool manualMode);

    void RunNNModbusOffline(vector<vector<double> > &NNOutput,int selectedFunction,
                            runtimeModel receivedModel,int modbusPortNum,string IPAddress);

    void RunNNModbusOnline(vector<vector<double> > &NNOutput, int selectedFunction,
                            runtimeModel receivedModel,bool stopCondition,int modbusPortNum,char *IPAddress);

    ExecuteNeuralNet();

private:
    modbus_t * m_modbus;

};

#endif // EXECUTENEURALNET_H

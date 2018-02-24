#include "executeneuralnet.h"
#include <iostream>
#include <customnn.h>
#include <fstream>
#include <sstream>
#include "loadtrainingdata.h"
#include "neuron.h"
#include "mysqlsetupwindow.h"


ExecuteNeuralNet::ExecuteNeuralNet()
{

}

void showVectorVals(string label, vector<double> &v)
{
    cout << label << " ";
    for (unsigned i = 0; i < v.size(); ++i) {
        cout << v[i] << " ";
    }

    cout << endl;
}

void  ExecuteNeuralNet::TrainNN(vector<double> &smoothedError, vector<vector<double> > &targetOutput,
                                  vector<vector<double> > &NNOutput, double learningRate,
                                  double momentum, float memoryRange, string filename,
                                  int selectedFunction, bool mySQLMode, vector<vector<Layer>> &NNModelLayers)
{
    vector<double> allAverageError;
    allAverageError.clear();

    vector<Layer> CurrentModel;
    vector<vector<Layer>> AllModels;

    const string dataFile = filename;//.toStdString();//"/home/reggie/Project_Apps/NeuralNet_Still/StillControlNNAlgorithm/David_Miller_NN/MakingDMData/trainingdata.txt";

    TrainingData trainData(dataFile);

    //cout << "Got training data \n";
    // e.g., { 3, 2, 1 }
    vector<unsigned> topology;
    trainData.getTopology(topology);

    //this is where we set the topology and since
    //this is a XOR net we use 2,4,1
//    topology.push_back(2);
//    topology.push_back(4);
//    topology.push_back(1);

    CustomNN myNet(topology);
//    Neuron NNObj;
//    NNObj.eta = learningRate;
//    NNObj.alpha = momentum;
//      momentumToAlpha = momentum;
//      learnToEta = learningRate;

//    Neuron neuronObj;
//    neuronObj.eta = learningRate;
//    neuronObj.alpha = momentum;

    vector<double> inputVals, currentTargetVals, currentResultVals;
    int trainingPass = 0;


    while (!trainData.isEof()) {
        ++trainingPass;
        //cout << endl << "Pass " << trainingPass;

        // Get new input data and feed it forward:
        if (trainData.getNextInputs(inputVals) != topology[0]) {
            //cout << "\n you broke it, it went to this " << trainData.getNextInputs(inputVals) << endl;
            break;
        }
        //showVectorVals(": Inputs:", inputVals);
        myNet.Feedforward(inputVals,selectedFunction);

        // Collect the net's actual output results:
        myNet.GetResults(currentResultVals);
        NNOutput.push_back(currentResultVals);
        //showVectorVals("Outputs:", resultVals);

        // Train the net what the outputs should have been:
        trainData.getTargetOutputs(currentTargetVals);
        targetOutput.push_back(currentTargetVals);
        //showVectorVals("Targets:", targetVals);
        assert(currentTargetVals.size() == topology.back());


        myNet.BackPropogation(currentTargetVals,learningRate,momentum,memoryRange,selectedFunction);

        CurrentModel.clear();
        CurrentModel = myNet.m_layers;
        // Report how well the training is working, average over recent samples:
        //cout << "Net recent average error: "
                //<< myNet.getRecentAverageError() << endl;
        allAverageError.push_back(myNet.getRecentAverageError());

        AllModels.push_back(CurrentModel);
    }

    //cout << endl << "Done" << endl;

    NNModelLayers = AllModels;
    smoothedError.clear();
    smoothedError = allAverageError;
    //return allAverageError;
    //return a.exec();
}

void  ExecuteNeuralNet::TrainNN(vector<double> &smoothedError,vector<vector<double> > &targetOutput,
                              vector<vector<double> > &NNOutput, double learningRate,
                              double momentum, float memoryRange, string filename,
                              int selectedFunction, bool mySQLMode, vector<vector<Layer>> &NNModelLayers,
                              int numberOfHiddenLayers, int firstLayerNeurons)
{
    vector<double> allAverageError;
    allAverageError.clear();

    vector<Layer> CurrentModel;
    vector<vector<Layer>> AllModels;



    if(!mySQLMode)
    {
        const string dataFile = filename;

        TrainingData trainData(dataFile);

        //cout << "Got training data \n";
        // e.g., { 3, 2, 1 }
        vector<unsigned> topology;
        trainData.getTopology(topology);

        //this is where we set the topology and since
        //this is a XOR net we use 2,4,1
    //    topology.push_back(2);
    //    topology.push_back(4);
    //    topology.push_back(1);
        unsigned int firstLayerNum = topology.front();
        unsigned int lastLayerNum = topology.back();

        topology.clear();

        topology.push_back(firstLayerNum);
        topology.push_back(firstLayerNeurons);

        if(numberOfHiddenLayers>1)
        {
        int hiddenLayerDecrement = firstLayerNeurons/numberOfHiddenLayers;
        int currentLayerNeuronNum = firstLayerNeurons;

        for(uint hiddenLayerCycle = 1;hiddenLayerCycle<numberOfHiddenLayers;hiddenLayerCycle++)
        {
            currentLayerNeuronNum = currentLayerNeuronNum - hiddenLayerDecrement;
            topology.push_back(currentLayerNeuronNum);
        }
        }

        topology.push_back(lastLayerNum);

        CustomNN myNet(topology);

        vector<double> inputVals, currentTargetVals, currentResultVals;
        int trainingPass = 0;


        while (!trainData.isEof()) {
            ++trainingPass;
            // Get new input data and feed it forward:
            if (trainData.getNextInputs(inputVals) != topology[0]) {
                //cout << "\n you broke it, it went to this " << trainData.getNextInputs(inputVals) << endl;
                break;
            }
            //showVectorVals(": Inputs:", inputVals);
            myNet.Feedforward(inputVals,selectedFunction);

            // Collect the net's actual output results:
            myNet.GetResults(currentResultVals);
            NNOutput.push_back(currentResultVals);
            //showVectorVals("Outputs:", resultVals);

            // Train the net what the outputs should have been:
            trainData.getTargetOutputs(currentTargetVals);
            targetOutput.push_back(currentTargetVals);
            //showVectorVals("Targets:", targetVals);
            assert(currentTargetVals.size() == topology.back());
            myNet.BackPropogation(currentTargetVals,learningRate,momentum,memoryRange,selectedFunction);

            CurrentModel.clear();
            CurrentModel = myNet.m_layers;
            // Report how well the training is working, average over recent samples:
            allAverageError.push_back(myNet.getRecentAverageError());

            AllModels.push_back(CurrentModel);
        }

        NNModelLayers = AllModels;
        smoothedError.clear();
        smoothedError = allAverageError;
    }
    else
    {
        mySQLAPI mySQLObj;
        vector<unsigned> topology;
        char ***SQLout;
        char sqlcmd[350] ={'\0'};
        char *configServer = "192.168.2.162";
        char *configUser = "reggie";
        char *configPassword = "Still";
        int sqlcolsize = 1;
        int sqlrowsize = 1;
        char checkstr[50] = {'\0'};

        //Sizing the SQLout based on the number of rows in Stations_and_Devices
        SQLout = (char ***)malloc(sizeof(char **));
        SQLout[0] = (char **)malloc(sizeof(char *));
        SQLout[0][0] = (char *)malloc(sizeof(char ));
        memset(&sqlcmd[0], 0, sizeof(sqlcmd));
        strcpy(sqlcmd,"SELECT COUNT(*) FROM Devices_References");
        mySQLObj.sql_main(sqlcmd,SQLout,"stillas3_project",true,sqlcolsize,configServer,configUser,configPassword);

        sqlrowsize = atoi(SQLout[0][0]);

        memset(&sqlcmd[0], 0, sizeof(sqlcmd));
        strcpy(sqlcmd,"SELECT Count(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name = 'Devices_References'");
        mySQLObj.sql_main(sqlcmd,SQLout,"stillas3_project",true,sqlcolsize,configServer,configUser,configPassword);

//        bool mySQLAPI::sql_main(char *sqlcmd,char ***SQLout, char *database,bool returnmode,
//                                int columnsize,char *server,char *user,char *password)

        sqlcolsize = atoi(SQLout[0][0]);

        free(SQLout[0][0]);
        free(SQLout[0]);
        free(SQLout);

        //Setting the size of string in each SQLoutput followed by the number of rows and then columns
        SQLout = (char ***)calloc(250, sizeof(char **));
        for (int i=0;i <sqlrowsize; i++)
        {
            SQLout[i] = (char **)calloc(sqlrowsize, sizeof(char*));
            for (int j =0;j<sqlcolsize;j++)
            {
                SQLout[i][j] = (char *)calloc(250,sizeof(char));
            }
        }

        strcat(checkstr,"\"");
        strcpy(sqlcmd,"SELECT COUNT(*) FROM `Devices_References` WHERE Device_Primary_ID =\"");
        strcat(sqlcmd,checkstr);
        mySQLObj.sql_main(sqlcmd,SQLout,"stillas3_project",true,sqlcolsize,configServer,configUser,configPassword);

        //this is where we set the topology and since
        //this is a XOR net we use 2,4,1
    //    topology.push_back(2);
    //    topology.push_back(4);
    //    topology.push_back(1);
        unsigned int firstLayerNum = topology.front();
        unsigned int lastLayerNum = topology.back();

        topology.clear();

        topology.push_back(firstLayerNum);
        topology.push_back(firstLayerNeurons);

        if(numberOfHiddenLayers>1)
        {
        int hiddenLayerDecrement = firstLayerNeurons/numberOfHiddenLayers;
        int currentLayerNeuronNum = firstLayerNeurons;

        for(uint hiddenLayerCycle = 1;hiddenLayerCycle<numberOfHiddenLayers;hiddenLayerCycle++)
        {
            currentLayerNeuronNum = currentLayerNeuronNum - hiddenLayerDecrement;
            topology.push_back(currentLayerNeuronNum);
        }
        }

        topology.push_back(lastLayerNum);

        CustomNN myNet(topology);

        vector<double> inputVals, currentTargetVals, currentResultVals;

        bool stopMySQLVal = false;

        while(!stopMySQLVal)
        {
            //showVectorVals(": Inputs:", inputVals);
            myNet.Feedforward(inputVals,selectedFunction);

            // Collect the net's actual output results:
            myNet.GetResults(currentResultVals);
            NNOutput.push_back(currentResultVals);
            //showVectorVals("Outputs:", resultVals);

            // Train the net what the outputs should have been:

            targetOutput.push_back(currentTargetVals);
            //showVectorVals("Targets:", targetVals);
            assert(currentTargetVals.size() == topology.back());
            myNet.BackPropogation(currentTargetVals,learningRate,momentum,memoryRange,selectedFunction);

            CurrentModel.clear();
            CurrentModel = myNet.m_layers;
            // Report how well the training is working, average over recent samples:
            allAverageError.push_back(myNet.getRecentAverageError());

            AllModels.push_back(CurrentModel);

            for (int i=0; i<sqlcolsize; i++)
            {
                for (int j = 0;j<sqlrowsize;j++)
                {
                    //printf("SQLout[%d][%d] \n",i,j);
                    free(SQLout[i][j]);
                }
                   //printf("SQLout[%d] \n",i);
                free (SQLout[i]);
            }
            free(SQLout);
        }

        NNModelLayers = AllModels;
        smoothedError.clear();
        smoothedError = allAverageError;


    }


}

void  ExecuteNeuralNet::MethodRunNN(vector<double> &smoothedError,vector<vector<double>> &targetInput,vector<vector<double>> &targetOutput,
                                    vector<vector<double> > &NNOutput,double learningRate,double momentum, float memoryRange, string filename,
                                    int selectedFunction, vector<vector<Layer>> &NNModelLayers, runtimeModel receivedModel)
{
    vector<double> allAverageError;
    allAverageError.clear();

    vector<Layer> CurrentModel;
    vector<vector<Layer>> AllModels;

    const string dataFile = filename;//.toStdString();//"/home/reggie/Project_Apps/NeuralNet_Still/StillControlNNAlgorithm/David_Miller_NN/MakingDMData/trainingdata.txt";

    TrainingData trainData(dataFile);

    //cout << "Got training data \n";
    // e.g., { 3, 2, 1 }
    vector<unsigned> topology;
    trainData.getTopology(topology);

    //this is where we set the topology and since
    //this is a XOR net we use 2,4,1
//    topology.push_back(2);
//    topology.push_back(4);
//    topology.push_back(1);

    CustomNN myNet(topology,receivedModel);
//    Neuron NNObj;
//    NNObj.eta = learningRate;
//    NNObj.alpha = momentum;
//      momentumToAlpha = momentum;
//      learnToEta = learningRate;

//    Neuron neuronObj;
//    neuronObj.eta = learningRate;
//    neuronObj.alpha = momentum;

    NNOutput.clear(); targetOutput.clear();
    vector<double> inputVals, currentTargetVals, currentResultVals;
    int trainingPass = 0;


    while (!trainData.isEof()) {
        ++trainingPass;
        //cout << endl << "Pass " << trainingPass;

        // Get new input data and feed it forward:
        if (trainData.getNextInputs(inputVals) != topology[0]) {
            //cout << "\n you broke it, it went to this " << trainData.getNextInputs(inputVals) << endl;
            break;
        }
        //showVectorVals(": Inputs:", inputVals);
        myNet.Feedforward(inputVals,selectedFunction);

        // Collect the net's actual output results:
        //not sure how this is supposed to work but it really doesnt send the correct
        //outputs
        myNet.GetResults(currentResultVals);

        //showVectorVals("Outputs:", resultVals);

        // Train the net what the outputs should have been:
        trainData.getTargetOutputs(currentTargetVals);
        targetOutput.push_back(currentTargetVals);
        //showVectorVals("Targets:", targetVals);
        assert(currentTargetVals.size() == topology.back());

        //myNet.BackPropogation(currentTargetVals,learningRate,momentum,memoryRange,selectedFunction);

        CurrentModel.clear();
        CurrentModel = myNet.m_layers;

        NNOutput.push_back(currentResultVals);
        // Report how well the training is working, average over recent samples:
        //cout << "Net recent average error: "
                //<< myNet.getRecentAverageError() << endl;
        allAverageError.push_back(myNet.getRecentAverageError());

        AllModels.push_back(CurrentModel);
    }

    //cout << endl << "Done" << endl;

    NNModelLayers = AllModels;
    smoothedError.clear();
    smoothedError = allAverageError;
    //return allAverageError;
    //return a.exec();
}


void  ExecuteNeuralNet::RunNN(vector<double> &smoothedError, vector<vector<double>> &targetOutput, vector<vector<double> > &NNOutput, double learningRate,
                                        double momentum, float memoryRange, string filename,
                                        int selectedFunction, bool mySQLMode, vector<vector<Layer>> &NNModelLayers, runtimeModel receivedModel)
{
    vector<double> allAverageError;
    allAverageError.clear();

    vector<Layer> CurrentModel;
    vector<vector<Layer>> AllModels;

    const string dataFile = filename;//.toStdString();//"/home/reggie/Project_Apps/NeuralNet_Still/StillControlNNAlgorithm/David_Miller_NN/MakingDMData/trainingdata.txt";

    TrainingData trainData(dataFile);

    //cout << "Got training data \n";
    // e.g., { 3, 2, 1 }
    vector<unsigned> topology;
    trainData.getTopology(topology);

    //this is where we set the topology and since
    //this is a XOR net we use 2,4,1
//    topology.push_back(2);
//    topology.push_back(4);
//    topology.push_back(1);

    CustomNN myNet(topology,receivedModel);
//    Neuron NNObj;
//    NNObj.eta = learningRate;
//    NNObj.alpha = momentum;
//      momentumToAlpha = momentum;
//      learnToEta = learningRate;

//    Neuron neuronObj;
//    neuronObj.eta = learningRate;
//    neuronObj.alpha = momentum;

    NNOutput.clear(); targetOutput.clear();
    vector<double> inputVals, currentTargetVals, currentResultVals;
    int trainingPass = 0;


    while (!trainData.isEof()) {
        ++trainingPass;
        //cout << endl << "Pass " << trainingPass;

        // Get new input data and feed it forward:
        if (trainData.getNextInputs(inputVals) != topology[0]) {
            //cout << "\n you broke it, it went to this " << trainData.getNextInputs(inputVals) << endl;
            break;
        }
        //showVectorVals(": Inputs:", inputVals);
        myNet.Feedforward(inputVals,selectedFunction);

        // Collect the net's actual output results:
        //not sure how this is supposed to work but it really doesnt send the correct
        //outputs
        myNet.GetResults(currentResultVals);

        //showVectorVals("Outputs:", resultVals);

        // Train the net what the outputs should have been:
        trainData.getTargetOutputs(currentTargetVals);
        targetOutput.push_back(currentTargetVals);
        //showVectorVals("Targets:", targetVals);
        assert(currentTargetVals.size() == topology.back());

        //myNet.BackPropogation(currentTargetVals,learningRate,momentum,memoryRange,selectedFunction);

        CurrentModel.clear();
        CurrentModel = myNet.m_layers;

        NNOutput.push_back(currentResultVals);
        // Report how well the training is working, average over recent samples:
        //cout << "Net recent average error: "
                //<< myNet.getRecentAverageError() << endl;
        allAverageError.push_back(myNet.getRecentAverageError());

        AllModels.push_back(CurrentModel);
    }

    //cout << endl << "Done" << endl;

    NNModelLayers = AllModels;
    smoothedError.clear();
    smoothedError = allAverageError;
    //return allAverageError;
    //return a.exec();
}


void  ExecuteNeuralNet::RunNN(vector<vector<double> > &NNOutput,int selectedFunction,
                              runtimeModel receivedModel,int manualVal1,int manualVal2,bool manualMode)
{
    vector<unsigned> topology;

    NNOutput.clear();
    int ret = -1;
    vector<double> inputVals, currentResultVals;

    if(manualMode)
    {
        topology.push_back(2);
        topology.push_back(4);
        topology.push_back(1);

       inputVals = {manualVal1,manualVal2};
    }
    else
    {
        topology.push_back(5);
        topology.push_back(8);
        topology.push_back(8);
        topology.push_back(2);

        uint8_t dest[1024];
        uint16_t * dest16 = (uint16_t *) dest;

        char * IPAddress = "192.168.2.80";
        int modbusPortNum = 502;

        m_modbus = modbus_new_tcp(IPAddress, modbusPortNum);
        modbus_connect(m_modbus);

        memset( dest, 0, 1024 );

        bool is16Bit = false;
        modbus_set_slave( m_modbus, 1 );

        ret = modbus_read_registers( m_modbus, 1, 5, dest16 );
        is16Bit = true;

        for(uint i = 0;i<6;i++)
        {
            inputVals.push_back(double(dest16[i]));
            cout << "Register[" << i << "] = " << dest16[i] << endl;
        }

    }


    CustomNN myNet(topology,receivedModel);
    myNet.Feedforward(inputVals,selectedFunction);
    myNet.GetResults(currentResultVals);
    NNOutput.push_back(currentResultVals);

    if(!manualMode)
    {
        uint8_t num = currentResultVals.size();

        uint8_t * data = new uint8_t[num];
        for( int i = 0; i < num; ++i )
        {
            data[i] = currentResultVals.at(i);
        }

        int addr = 0;

        ret = modbus_write_bits( m_modbus, addr, num, data );
        delete[] data;


        modbus_close(m_modbus);
        modbus_free(m_modbus);
    }


}


void ExecuteNeuralNet::RunNNModbusOnline(vector<vector<double> > &NNOutput,int selectedFunction,
                                         runtimeModel receivedModel,bool stopCondition,int modbusPortNum,
                                         QString IPAddress)
{
    vector<unsigned> topology;

    topology.push_back(3);
    topology.push_back(6);
    topology.push_back(5);
    topology.push_back(4);
    topology.push_back(2);
    //this is what triggers the runmode
    CustomNN myNet(topology,receivedModel);

    NNOutput.clear();
    vector<double> inputVals, currentTargetVals, currentResultVals;

    while (!stopCondition)
    {
        uint8_t dest[1024];
        uint16_t * dest16 = (uint16_t *) dest;

        QString tempQString = IPAddress;
        QByteArray tempByteArray = tempQString.toLatin1();
        char * ModbusIP = tempByteArray.data();//"192.168.2.80";

        m_modbus = modbus_new_tcp(ModbusIP, modbusPortNum);
        modbus_connect(m_modbus);

        memset( dest, 0, 1024 );

        int ret = -1;
        bool is16Bit = false;
        modbus_set_slave( m_modbus, 1 );

        ret = modbus_read_registers( m_modbus, 1, 5, dest16 );
        is16Bit = true;

        for(uint i = 0;i<6;i++)
        {
            inputVals.push_back(double(dest16[i]));
            cout << "Register[" << i << "] = " << dest16[i] << endl;
        }

        myNet.Feedforward(inputVals,selectedFunction);
        myNet.GetResults(currentResultVals);
        NNOutput.push_back(currentResultVals);

        uint8_t num = currentResultVals.size();

        uint8_t * data = new uint8_t[num];
        for( int i = 0; i < num; ++i )
        {
            data[i] = currentResultVals.at(i);
        }

        int addr = 0;

        ret = modbus_write_bits( m_modbus, addr, num, data );
        delete[] data;
        currentResultVals.clear();

        //myNet.BackPropogation(currentTargetVals,learningRate,momentum,memoryRange,selectedFunction);

        //CurrentModel.clear();
        //CurrentModel = myNet.m_layers;
    }

    modbus_close(m_modbus);
    modbus_free(m_modbus);
}

void ExecuteNeuralNet::RunNNModbusOffline(vector<vector<double> > &NNOutput,int selectedFunction,
                        runtimeModel receivedModel,int modbusPortNum,QString IPAddress)
{
    vector<unsigned> topology;

    topology.push_back(3);
    topology.push_back(6);
    topology.push_back(5);
    topology.push_back(4);
    topology.push_back(2);
    //this is what triggers the runmode
    CustomNN myNet(topology,receivedModel);

    NNOutput.clear();
    vector<double> inputVals, currentTargetVals, currentResultVals;

    bool stopCondition = true;

    while (!stopCondition)
    {
        uint8_t dest[1024];
        uint16_t * dest16 = (uint16_t *) dest;

        QString tempQString = IPAddress;
        QByteArray tempByteArray = tempQString.toLatin1();
        char * ModbusIP = tempByteArray.data();//"192.168.2.80";

        m_modbus = modbus_new_tcp(ModbusIP, modbusPortNum);
        modbus_connect(m_modbus);

        memset( dest, 0, 1024 );

        int ret = -1;
        bool is16Bit = false;
        modbus_set_slave( m_modbus, 1 );

        ret = modbus_read_registers( m_modbus, 1, 5, dest16 );
        is16Bit = true;

        for(uint i = 0;i<6;i++)
        {
            inputVals.push_back(double(dest16[i]));
            cout << "Register[" << i << "] = " << dest16[i] << endl;
        }

        myNet.Feedforward(inputVals,selectedFunction);
        myNet.GetResults(currentResultVals);
        NNOutput.push_back(currentResultVals);

        uint8_t num = currentResultVals.size();

        uint8_t * data = new uint8_t[num];
        for( int i = 0; i < num; ++i )
        {
            data[i] = currentResultVals.at(i);
        }

        int addr = 0;

        ret = modbus_write_bits( m_modbus, addr, num, data );
        delete[] data;
        currentResultVals.clear();

        //myNet.BackPropogation(currentTargetVals,learningRate,momentum,memoryRange,selectedFunction);

        //CurrentModel.clear();
        //CurrentModel = myNet.m_layers;
    }

    modbus_close(m_modbus);
    modbus_free(m_modbus);
}

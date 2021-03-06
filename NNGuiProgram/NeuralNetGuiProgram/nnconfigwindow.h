/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#ifndef NNCONFIGWINDOW_H
#define NNCONFIGWINDOW_H

#include <QMainWindow>
#include <executeneuralnet.h>
#include "mysqlcustomquerywindow.h"
#include "mysqlsetupwindow.h"
#include "neuron.h"
#include "customnn.h"
//currently using nlohmann json
//version 3.0.0 as 2.0.6 is not supported by
//ubuntu 17.10
//#include "json.hpp"
#include "src/json.hpp"
#include "modbussetupwindow.h"


using json = nlohmann::json;

struct NNModel {
    vector<vector<Layer>> trainingModels; //this holds the layers which have the weights and represents the over all model
    vector<double> modelError; //this is the error determined by the model output at a given sequence compared to the prefered output and smoothed over a range of previous outputs
    double learningRate; //this determines the gain on step size towards the gradient
    double momentum; //this determines the force with which the steps size continues (speeds up training but can go over small minimums)
    float errorSmoothingRange; //this determines the range over which the error is averaged
    int bestPerformancePass; //this logs the point during the training at which the model performed with the least smoothed error
    string ActivationFunction;
    vector<unsigned> Topology;
};

typedef vector<vector<vector<float>>> runtimeModel;

namespace Ui {
class NNConfigWindow;
}

class NNConfigWindow : public QMainWindow
{
    Q_OBJECT

public:
    runtimeModel jsonChosenModel;
    explicit NNConfigWindow(QWidget *parent = 0,bool trainingMode = true);
    int hiddenLayersNum;
    int initialHiddenLayersNeuronNum;
    bool useCustomTopology = false;
    int chosenActivation;
    bool mySQLIsValid = false;
    setupConfiguration mySQLConfig;

    modbusConfig modbusTargetConfig;


    ~NNConfigWindow();

private slots:
    void on_StartNNButton_clicked();

    void on_learningRateSlider_sliderMoved(int position);

    void on_momentumSlider_sliderMoved(int position);

    void on_memorySlider_sliderMoved(int position);

    void on_pushButton_clicked();

    void on_customTopologyChkBox_clicked(bool checked);

    void on_initialNeuronsinHiddenLayers_valueChanged(int arg1);

    void on_hiddenLayersSelector_valueChanged(int arg1);

    void on_activationFunctinoComboBox_activated(int index);

    void on_txtTrainingDataType_clicked(bool checked);

    void on_mySQLTrainingDataSourceType_clicked(bool checked);

    void on_addInputPushButton_clicked();

    void on_addOutputPushButton_clicked();

    void on_setupPushButton_clicked();

    void on_activationFunctinoComboBox_currentIndexChanged(int index);

    void on_saveBiasesandWeights_clicked();

    void on_mySQLTargetSelect_clicked(bool checked);

    void on_commandLinkButton_clicked();

    void on_SelectTarget_clicked();

    void on_targetSelectBox_currentIndexChanged(int index);

private:

    int manualInput1,manualInput2;
    bool manualMode = false;
    NNModel trainingModelParameters;
    string filename;
    ExecuteNeuralNet NNControlObj;
    void SendExecuteCommand(void);
    Ui::NNConfigWindow *ui;
    bool mySQLMode = false;
    int OutputCount;
    int InputCount;
    int runType;
    mySQLCustomQueryWindow *queryWindow;
    mySQlSetupWindow *setupmySQLSetupWindow;
    modbusSetupWindow *modbusConfigWindow;
    int activationFunctionType;
    float errorSmoothingRange;

    bool trainingModeReceived;

    //run mode variables
    uint Modeltype;//0 = dat,1 = mySQL,2 = json
    string modelFile;

    json modelJson = {
        {"Model Error:",0.0},
        {"Learning Rate:",0.0},
        {"Momentum:",0.0},
        {"Error Smoothing Range:",0.0},
        {"Best Performance Pass:",0},
        {"Training Model:",
            {"Layer:",
             {"Neuron:",
             {"OutputVal:",0.0},
             {"Output Weight:",5.0}, }
            },
        },

        };
    //vector<Layer> runtimeModel;


};

#endif // NNCONFIGWINDOW_H

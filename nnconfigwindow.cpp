#include "nnconfigwindow.h"
#include "ui_nnconfigwindow.h"
#include "executeneuralnet.h"
#include <vector>
#include <QFileDialog>
#include <QMessageBox>
#include "boost/filesystem.hpp"
#include "startupwindow.h"
//#include "jsoncpp/json/json.h"


using namespace std;

NNConfigWindow::NNConfigWindow(QWidget *parent, bool trainingMode) :
    QMainWindow(parent),
    ui(new Ui::NNConfigWindow)
{
    ui->setupUi(this);

    ui->targetmodeLabel->setVisible(false);
    trainingModeReceived = trainingMode;
    if(trainingMode)
    {
        ui->mySQLTargetSelect->setVisible(false);
        ui->offlineSelect->setVisible(false);
        ui->SelectTarget->setVisible(false);
        ui->OPCSelect->setVisible(false);

        ui->StartNNButton->setEnabled(false);

        ui->learningRateSlider->setValue(0.15);
        ui->momentumSlider->setValue(0.50);
        ui->memorySlider->setValue(0.50);
        InputCount = 0;
        OutputCount = 0;

        // create graph and assign data to it:
        ui->NNPlotWidget->addGraph();
        // give the axes some labels:
        ui->NNPlotWidget->xAxis->setLabel("Passes");
        ui->NNPlotWidget->yAxis->setLabel("Error");
        // set axes ranges, so we see all data:
        ui->NNPlotWidget->xAxis->setRange(0, 2001);
        ui->NNPlotWidget->yAxis->setRange(-1, 1);
        ui->NNPlotWidget->replot();
        ui->pushButton->setVisible(false);
        ui->addInputLabel->setVisible(false);
        ui->addInputLCD->setVisible(false);
        ui->addInputPushButton->setVisible(false);
        ui->addOutputLabel->setVisible(false);
        ui->addOutputLCD->setVisible(false);
        ui->addOutputPushButton->setVisible(false);
        ui->setupPushButton->setVisible(false);
        ui->customTopologyChkBox->setVisible(false);
        ui->initialNeuronsinHiddenLayers->setVisible(false);
        ui->hiddenLayersSelector->setVisible(false);
        ui->HiddenLayerLabel->setVisible(false);
        ui->HiddenNeuronLabel->setVisible(false);
        useCustomTopology = false;
        QStringList list=(QStringList()<<"Sigmoid"<<"ReLu"<<"Leaky ReLu");
        ui->activationFunctinoComboBox->addItems(list);
        ui->activationFunctinoComboBox->setCurrentIndex(0);
        activationFunctionType = 0;//default activation function set to Sigmoid
        ui->saveBiasesandWeights->setVisible(false);

        ui->hiddenLayersSelector->move(760,570);
        ui->initialNeuronsinHiddenLayers->move(760,640);
    }
    else
    {
        // create graph and assign data to it:
        ui->NNPlotWidget->addGraph();
        // give the axes some labels:
        ui->NNPlotWidget->xAxis->setLabel("Passes");
        ui->NNPlotWidget->yAxis->setLabel("Error");
        // set axes ranges, so we see all data:
        ui->NNPlotWidget->xAxis->setRange(0, 2001);
        ui->NNPlotWidget->yAxis->setRange(-1, 1);
        ui->NNPlotWidget->replot();

        ui->mySQLTargetSelect->setVisible(true);
        ui->offlineSelect->setVisible(true);
        ui->SelectTarget->setVisible(true);
        ui->OPCSelect->setVisible(true);

        ui->saveBiasesandWeights->setVisible(false);
        ui->pushButton->setVisible(false);
        ui->addInputLabel->setVisible(false);
        ui->addInputLCD->setVisible(false);
        ui->addInputPushButton->setVisible(false);
        ui->addOutputLabel->setVisible(false);
        ui->addOutputLCD->setVisible(false);
        ui->addOutputPushButton->setVisible(false);
        ui->setupPushButton->setVisible(false);
        ui->customTopologyChkBox->setVisible(false);
        ui->initialNeuronsinHiddenLayers->setVisible(false);
        ui->hiddenLayersSelector->setVisible(false);
        ui->HiddenLayerLabel->setVisible(false);
        ui->HiddenNeuronLabel->setVisible(false);

        ui->addInputLabel->setVisible(false);
        ui->addInputLCD->setVisible(false);
        ui->addInputPushButton->setVisible(false);
        ui->addOutputLabel->setVisible(false);
        ui->addOutputLCD->setVisible(false);
        ui->addOutputPushButton->setVisible(false);
        ui->setupPushButton->setVisible(false);

        ui->initialNeuronsinHiddenLayers->setVisible(false);
        ui->hiddenLayersSelector->setVisible(false);
        ui->HiddenLayerLabel->setVisible(false);
        ui->HiddenNeuronLabel->setVisible(false);

        ui->initialNeuronsinHiddenLayers->setVisible(false);
        ui->hiddenLayersSelector->setVisible(false);
        ui->HiddenLayerLabel->setVisible(false);
        ui->HiddenNeuronLabel->setVisible(false);

        //these are the main things that dont otherwise go away
        ui->learningRateSlider->setVisible(false);
        ui->LearningRateLCD->setVisible(false);
        ui->momentumSlider->setVisible(false);
        ui->MomentumLCD->setVisible(false);
        ui->mainToolBar->setVisible(false);
        ui->memoryLCD->setVisible(false);
        ui->memorySlider->setVisible(false);
        ui->HiddenLayerLabel->setVisible(false);
        ui->HiddenNeuronLabel->setVisible(false);
        //ui->HiddenNeuronLabel_2->setVisible(false);
        //ui->txtTrainingDataType->setVisible(false);
        //ui->mySQLTrainingDataSourceType->setVisible(false);
        //ui->pushButton->setVisible(false);
        ui->StartNNButton->setVisible(false);
        ui->label->setVisible(false);
        ui->label_2->setVisible(false);
        ui->label_3->setVisible(false);
        ui->label_4->setVisible(false);
        ui->label_5->setVisible(false);
        ui->label_6->setVisible(false);
        ui->label_8->setVisible(false);
        ui->minPassLCD->setVisible(false);
        ui->minValLCD->setVisible(false);
        ui->activationFunctinoComboBox->setVisible(false);
        ui->avgErrorLCD->setVisible(false);

        ui->HiddenNeuronLabel_2->setText("Select Model Source Type");
        ui->txtTrainingDataType->setText("Local File");
        ui->pushButton->setVisible(true);
        ui->txtTrainingDataType->setChecked(true);

        ui->hiddenLayersSelector->move(280,535);
        ui->initialNeuronsinHiddenLayers->move(280,485);
    }
}


NNConfigWindow::~NNConfigWindow()
{
    delete ui;
}

void NNConfigWindow::SendExecuteCommand(void)
{
    double learningrate = double(ui->learningRateSlider->value())/100.0;
    double momentum = double(ui->momentumSlider->value())/100.0;
    errorSmoothingRange = double(ui->memorySlider->value())/10.0;
    vector<double> x;
    vector<double> NNError;
    vector<vector<double>> NNOutput;
    vector<vector<double>> TargetOutput;
    if(!useCustomTopology)
    {
        if(trainingModeReceived)
        {

            (NNControlObj.TrainNN(NNError,TargetOutput,NNOutput,learningrate,momentum,
                                errorSmoothingRange,filename,activationFunctionType,
                                true,trainingModelParameters.trainingModels));
        }
        else
        {

            if(manualMode)
            {

                NNControlObj.RunNN(NNOutput,activationFunctionType,jsonChosenModel,
                                   manualInput1,manualInput2,manualMode);
            }
            else if(modbusMode)
            {
                NNControlObj.RunNN(NNOutput,activationFunctionType,jsonChosenModel,
                                   manualInput1,manualInput2,false);
            }
            else
            {

                (NNControlObj.RunNN(NNError,TargetOutput,NNOutput,learningrate,momentum,
                                    errorSmoothingRange,filename,activationFunctionType,
                                    true,trainingModelParameters.trainingModels,jsonChosenModel));

                ui->avgErrorLCD->display(NNError.back());
                ui->NNPlotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
            }


        }


    }
    else
    {

        (NNControlObj.TrainNN(NNError,TargetOutput,NNOutput,learningrate,momentum,
                            errorSmoothingRange,filename,activationFunctionType,
                            true,trainingModelParameters.trainingModels,
                            hiddenLayersNum,initialHiddenLayersNeuronNum));
        ui->avgErrorLCD->display(NNError.back());
    }


    // generate some data:
    double minError = 1.0;
    int minErrorPass = 0;
    for(unsigned i = 0;i<NNError.size();i++)
    {
        if(NNError.at(i) < minError && i > 10)
        {
            minError = NNError.at(i);
            minErrorPass = i;
        }
        x.push_back(i);
    }

    switch (activationFunctionType)
    {
    case 0:
        trainingModelParameters.ActivationFunction = "Sigmoid";
        break;

    case 1:
        trainingModelParameters.ActivationFunction = "ReLu";
        break;

    case 2:
        trainingModelParameters.ActivationFunction = "LeakyReLu";
        break;
    }

    modelJson["Learning Rate:"] = learningrate;
    modelJson["Model Error:"] = NNError;
    modelJson["Momentum:"] = momentum;
    modelJson["Best Performance Pass:"] = minErrorPass;
    modelJson["Error Smoothing:"] = errorSmoothingRange;

    trainingModelParameters.modelError = NNError;
    trainingModelParameters.momentum = momentum;
    trainingModelParameters.bestPerformancePass = minErrorPass;
    ui->minValLCD->display(minError);
    ui->minPassLCD->display(minErrorPass);

    QVector<double> plottedY;
    QVector<double> plottedX;

    if(!manualMode)
    {
        plottedX.clear(); plottedY.clear();
        plottedY =  QVector<double>::fromStdVector(NNError);
        plottedX = QVector<double>::fromStdVector(x);

        ui->NNPlotWidget->graph(0)->setData(plottedX,plottedY);
        ui->NNPlotWidget->graph(0)->rescaleAxes(true);

        if(trainingModeReceived)
        {
            QVector<double> specialX;// = {minErrorPass,minErrorPass};
            QVector<double> specialY;// = {0,1};
            specialX.push_back(minErrorPass);specialX.push_back(minErrorPass);
            specialY.push_back(-1);specialY.push_back(1);
            ui->NNPlotWidget->addGraph();
            ui->NNPlotWidget->graph(1)->setPen(QPen(QColor(255, 100, 0)));
            ui->NNPlotWidget->graph(1)->setData(specialX, specialY);
        }
        else
        {
            ui->NNPlotWidget->addGraph();

            plottedY.clear();
            vector<double> firstOutput;

            for(uint outputCycle = 0;outputCycle< NNOutput.size(); outputCycle++)
            {
                firstOutput.push_back(NNOutput[outputCycle][0]);
            }

            plottedY =  QVector<double>::fromStdVector(firstOutput);
            ui->NNPlotWidget->graph(1)->setPen(QPen(QColor(124,252,0)));
            ui->NNPlotWidget->graph(1)->setData(plottedX, plottedY);

            ui->NNPlotWidget->addGraph();

            plottedY.clear();
            firstOutput.clear();
            for(uint outputCycle = 0;outputCycle< TargetOutput.size(); outputCycle++)
            {
                firstOutput.push_back(TargetOutput[outputCycle][0]);
            }
            plottedY =  QVector<double>::fromStdVector(firstOutput);
            ui->NNPlotWidget->graph(2)->setPen(QPen(QColor(30,144,255)));
            ui->NNPlotWidget->graph(2)->setData(plottedX, plottedY);
        }


        ui->NNPlotWidget->replot();
        ui->saveBiasesandWeights->setVisible(true);
    }
    else
    {
        ui->addInputLCD->display(round(NNOutput[0][0]));
    }
}

void NNConfigWindow::on_StartNNButton_clicked()
{
    SendExecuteCommand();
}

void NNConfigWindow::on_learningRateSlider_sliderMoved(int position)
{
    ui->LearningRateLCD->display(double(position)/100.0);
    //SendExecuteCommand();

}

void NNConfigWindow::on_momentumSlider_sliderMoved(int position)
{
    ui->MomentumLCD->display(double(position)/100.0);
    //SendExecuteCommand();
}

void NNConfigWindow::on_memorySlider_sliderMoved(int position)
{
    //this is the smoothing slider
    ui->memoryLCD->display(double(position)/10.0);
    //SendExecuteCommand();
}

void NNConfigWindow::on_pushButton_clicked()
{
    //QWindow::setTransientParent(NNConfigWindow.parent());


    filename = QFileDialog::getOpenFileName(
                this,
                tr("Select Data File"),
                "/",
                "Json Files (*.json);;Text Files (*.txt);;CSV Files (*.dat);;All Files (*.*)"
                ).toStdString();


    if(trainingModeReceived)
    {
        ui->StartNNButton->setEnabled(true);
        ui->customTopologyChkBox->setVisible(true);
    }
    else
    {
        //this is where we read in the neural network json file and find our network parameters
       //note that this should be made into a function later and also made more flexible for various var types

        ifstream NNJsonModel(filename);

        string filetype = boost::filesystem::extension(filename);
        if(filetype == ".json" && NNJsonModel)
        {

            json inputModel = json::parse(NNJsonModel);
            //inputModel << NNJsonModel;

            string bestPassString = "Pass" + to_string(inputModel["Model"][4]["BestPerformancePass"].get<int>());

            string squishName = inputModel["Model"][0]["Activation Function"].get<string>();

            if(squishName == " Sigmoid")
            {
                activationFunctionType = 0;
            }
            else if(squishName == " ReLu")
            {
                activationFunctionType = 1;
            }
            else if(squishName == " LeakyReLu")
            {
                activationFunctionType = 2;
            }
            else
            {
                cout << "Major danger will robinson \n";
            }

            inputModel = inputModel["Model"][6][bestPassString];
            vector<float> Weights;
            vector<vector<float>> Neurons;
            vector<vector<vector<float>>> Layers;
            json cycleJsonLayers, cycleJsonNeurons,weightJson;
            int layerCounter = 0,neuronCounter =0;
            for(auto cyclePasses = inputModel.begin(); cyclePasses != inputModel.end();cyclePasses++)
            {
                string jsonCycler = "Layer";
                cycleJsonLayers = inputModel[jsonCycler + to_string(int(layerCounter))];
                layerCounter++;

                for(auto cycleLayers = cycleJsonLayers.begin();cycleLayers != cycleJsonLayers.end();cycleLayers++)
                {
                    jsonCycler = "Neuron";
                    cycleJsonNeurons = cycleJsonLayers[jsonCycler + to_string(int(neuronCounter))];
                    weightJson = cycleJsonNeurons[0]["OutputWeight"];

                    Weights = weightJson.get<vector<float>>();

                    neuronCounter++;
                    Neurons.push_back(Weights);
                    Weights.clear();
                }
                Layers.push_back(Neurons);
                Neurons.clear();

                neuronCounter = 0;
            }
            jsonChosenModel = Layers;


            //runtimeModel = inputModel["Training Model:"][inputModel.at("Best Performance Pass:").get<int>()].get<vector<Layer>>();
        }
        else if(NNJsonModel)
        {



        }
    }


}

void NNConfigWindow::on_customTopologyChkBox_clicked(bool checked)
{
    if(checked)
    {
        ui->initialNeuronsinHiddenLayers->setVisible(true);
        ui->hiddenLayersSelector->setVisible(true);
        hiddenLayersNum = 1; ui->hiddenLayersSelector->setValue(hiddenLayersNum);
        initialHiddenLayersNeuronNum = 5; ui->initialNeuronsinHiddenLayers->setValue(initialHiddenLayersNeuronNum);
        ui->HiddenLayerLabel->setVisible(true);
        ui->HiddenNeuronLabel->setVisible(true);
        useCustomTopology = true;
    }
    else
    {
        useCustomTopology = false;
        ui->initialNeuronsinHiddenLayers->setVisible(false);
        ui->hiddenLayersSelector->setVisible(false);
        ui->HiddenLayerLabel->setVisible(false);
        ui->HiddenNeuronLabel->setVisible(false);
    }
}

void NNConfigWindow::on_initialNeuronsinHiddenLayers_valueChanged(int arg1)
{
    if(trainingModeReceived)
    {
        initialHiddenLayersNeuronNum = arg1;
    }
    else
    {
        manualInput1=arg1;
    }

}

void NNConfigWindow::on_hiddenLayersSelector_valueChanged(int arg1)
{

    if(trainingModeReceived)
    {
        hiddenLayersNum = arg1;
    }
    else
    {
        manualInput2=arg1;
    }

}

void NNConfigWindow::on_activationFunctinoComboBox_activated(int index)
{
    chosenActivation = index;
}

void NNConfigWindow::on_txtTrainingDataType_clicked(bool checked)
{
    if(checked)
    {
        ui->pushButton->setVisible(true);
        ui->pushButton->setText("Select Data File");
        mySQLMode = false;
        ui->addInputLabel->setVisible(false);
        ui->addInputLCD->setVisible(false);
        ui->addInputPushButton->setVisible(false);
        ui->addOutputLabel->setVisible(false);
        ui->addOutputLCD->setVisible(false);
        ui->addOutputPushButton->setVisible(false);
        ui->setupPushButton->setVisible(false);
    }
}

void NNConfigWindow::on_mySQLTrainingDataSourceType_clicked(bool checked)
{
    if(checked)
    {
        ui->pushButton->setVisible(false);
        ui->addInputLabel->setVisible(true);
        ui->addInputLCD->setVisible(true);
        ui->addInputPushButton->setVisible(true);
        ui->addOutputLabel->setVisible(true);
        ui->addOutputLCD->setVisible(true);
        ui->addOutputPushButton->setVisible(true);
        ui->setupPushButton->setVisible(true);
        mySQLMode = true;
        ui->customTopologyChkBox->setChecked(true);
    }
}

void NNConfigWindow::on_addInputPushButton_clicked()
{
    InputCount++;
    ui->addInputLCD->display(InputCount);
    queryWindow = new mySQLCustomQueryWindow(this);
    queryWindow->show();
}

void NNConfigWindow::on_addOutputPushButton_clicked()
{
    OutputCount++;
    ui->addInputLCD->display(OutputCount);
    queryWindow = new mySQLCustomQueryWindow(this);
    queryWindow->show();
}

void NNConfigWindow::on_setupPushButton_clicked()
{
    setupmySQLSetupWindow = new mySQlSetupWindow(this,mySQLConfig);
    setupmySQLSetupWindow->show();
}

void NNConfigWindow::on_activationFunctinoComboBox_currentIndexChanged(int index)
{
    activationFunctionType = index;
}

void NNConfigWindow::on_saveBiasesandWeights_clicked()
{
    string modelSaveLocation = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
    "/home",
    QFileDialog::ShowDirsOnly
    | QFileDialog::DontResolveSymlinks).toStdString();
    ofstream NeuralNetData;
    NeuralNetData.open(modelSaveLocation + "/NeuralNetData.json");

    if(NeuralNetData)
    {
        NeuralNetData << "{ \"\Model\"\: [" << endl;
        NeuralNetData << "{ \"\Activation Function\"\: " << "\"\ " << trainingModelParameters.ActivationFunction << "\"\ }, \n";
        NeuralNetData << "{ \"\LearningRate\"\: " << trainingModelParameters.learningRate << "}, \n";
        NeuralNetData << "{ \"\Momentum\"\: " << trainingModelParameters.momentum << "}, \n";
        NeuralNetData << "{ \"\SmoothingRange\"\: " << trainingModelParameters.errorSmoothingRange << "}, \n";
        NeuralNetData << "{ \"\BestPerformancePass\"\: " << trainingModelParameters.bestPerformancePass << "}, \n";
        //this should be done one level higher
        NeuralNetData << "{ \"\BestPerformanceError\"\: " << trainingModelParameters.modelError.at(trainingModelParameters.bestPerformancePass) << "}, \n";
        NeuralNetData << endl << endl << endl <<  endl;
        NeuralNetData << "   {";
        for(uint cyclePasses = 0;cyclePasses < trainingModelParameters.trainingModels.size(); cyclePasses++)
        {
            NeuralNetData << "   \"\Pass"<< cyclePasses << "\"\: " << " {" << endl;
            //NeuralNetData << "      \"\Layer"<< cycleLayers <<"\"\: " << " {" << endl;
            for (uint cycleLayers = 0; cycleLayers < trainingModelParameters.trainingModels[cyclePasses].size();cycleLayers++)
            {
                NeuralNetData << "      \"\Layer"<< cycleLayers <<"\"\: " << " {" << endl;
                //NeuralNetData << "          \"\Neuron"<< cycleLayers << "\"\: " << " {[" << endl;
                for (uint cycleNeurons = 0; cycleNeurons < trainingModelParameters.trainingModels[cyclePasses][cycleLayers].size();cycleNeurons++ )
                {
                    NeuralNetData << "          \"\Neuron"<< cycleNeurons << "\"\: " << " [" << endl;
                    NeuralNetData << "              { \"\OutputVal\"\: " << trainingModelParameters.trainingModels[cyclePasses][cycleLayers][cycleNeurons].m_OutputVal << ","<< endl;
                    NeuralNetData << "               \"\OutputWeight\"\: " << " [ " << endl;
                    for(uint cycleWeights = 0;cycleWeights < trainingModelParameters.trainingModels[cyclePasses][cycleLayers][cycleNeurons].m_OutputWeights.size(); cycleWeights++ )
                    {
                        if(cycleWeights == trainingModelParameters.trainingModels[cyclePasses][cycleLayers][cycleNeurons].m_OutputWeights.size()-1)
                        {
                            NeuralNetData << "                  " << trainingModelParameters.trainingModels[cyclePasses][cycleLayers][cycleNeurons].m_OutputWeights[cycleWeights].Weight << endl;
                        }
                        else
                        {
                            NeuralNetData << "                  " << trainingModelParameters.trainingModels[cyclePasses][cycleLayers][cycleNeurons].m_OutputWeights[cycleWeights].Weight << "," << endl;;
                        }
                    }
                    NeuralNetData << "                   ] }" << endl;

                    if(cycleNeurons == trainingModelParameters.trainingModels[cyclePasses][cycleLayers].size()-1)
                    {
                        NeuralNetData << "          ]" << endl;
                    }
                    else
                    {
                        NeuralNetData << "          ]," << endl;
                    }
                }
                if(cycleLayers == trainingModelParameters.trainingModels[cyclePasses].size()-1)
                {
                    NeuralNetData << "      } " << endl;
                }
                else
                {
                    NeuralNetData << "      }, " << endl;
                }
            }
            if(cyclePasses == trainingModelParameters.trainingModels.size()-1)
            {
                NeuralNetData << "      } " << endl;
            }
            else
            {
                NeuralNetData << "      }, " << endl;
            }
        }
        NeuralNetData << "      }, " << endl;
        NeuralNetData << endl << endl << endl << endl;

        NeuralNetData << " { \"\Model Error\"\: [\n";

        for(uint cycleError = 0; cycleError < trainingModelParameters.modelError.size(); cycleError++)
        {
            if(cycleError == trainingModelParameters.modelError.size()-1)
            {
                NeuralNetData << trainingModelParameters.modelError.at(cycleError) << endl;
            }
            else
            {
                NeuralNetData << trainingModelParameters.modelError.at(cycleError) << ", \n";
            }
        }
        NeuralNetData << "] } \n ] \n }";

        NeuralNetData.close();
    }
    else
    {
        cout << "couldnt create data file \n";
    }

}

void NNConfigWindow::on_mySQLTargetSelect_clicked(bool checked)
{
    if(checked)
    {
        if(!modbusMode)
        {
            manualMode = true;
        }

        ui->hiddenLayersSelector->setVisible(true);
        ui->hiddenLayersSelector->setMinimum(0);
        ui->hiddenLayersSelector->setMaximum(1);
        ui->initialNeuronsinHiddenLayers->setVisible(true);
        ui->initialNeuronsinHiddenLayers->setMinimum(0);
        ui->initialNeuronsinHiddenLayers->setMaximum(1);
        ui->addInputLCD->setVisible(true);
        ui->StartNNButton->setVisible(true);
    }

}

void NNConfigWindow::on_offlineSelect_clicked(bool checked)
{

}

void NNConfigWindow::on_OPCSelect_clicked(bool checked)
{
    modbusMode = true;
}

void NNConfigWindow::on_commandLinkButton_clicked()
{
    StartupWindow *mainMenu;
    this->close();
    mainMenu = new StartupWindow(this);
    mainMenu->show();
}

void NNConfigWindow::on_SelectTarget_clicked()
{
    filename = QFileDialog::getOpenFileName(
                this,
                tr("Select Data File"),
                "/",
                "Text Files (*.txt);;All Files (*.*)"
                ).toStdString();

    ui->StartNNButton->setVisible(true);
}


/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#ifndef CUSTOMNN_H
#define CUSTOMNN_H

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include "neuron.h"

using namespace std;
typedef vector<vector<vector<float>>> runtimeModel;
// ********Class CustomNN ********
//this is reffered to as class NET or Network in the tutorial
class CustomNN
{
public:
    //void DefineNN();
    //topology refers to both the number of layers (input,hidden,output) and the size of each layer
    CustomNN(const vector<unsigned> &Topology);
    CustomNN(const vector<unsigned> &Topology,runtimeModel optimalModel);
    //since Feedforward just passes vals to neurons we can use const here
    void Feedforward(const vector<double> &InputVals,int activationFunctionType);
    void BackPropogation(const vector<double> &TargetVals, double learnToEta, double momentumToAlpha,
                         double memoryRangeToSmoothingFactor,int activationFunctionType);
    void BackPropogation(const vector<double> &TargetVals, double learnToEta,
                         double momentumToAlpha, double memoryRangeToSmoothingFactor,
                         int activationFunctionType, const vector<double> Outputs, const vector<double> Inputs);
    //since GetResults doesnt actually do anything and just reads
    //the results and spits them back so it is a const function
    //note that the const is not neccessary
    void GetResults(vector<double> &ResultVals) const;
    double getRecentAverageError(void) const { return m_recentAverageError; }
    vector<Layer> m_layers;

private:
    //vector<Layer> m_layers;
    double m_error;
    double m_recentAverageError;
    double m_recentAverageSmoothingFactor;
};

#endif // CUSTOMNN_H

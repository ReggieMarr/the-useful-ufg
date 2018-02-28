/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#ifndef LOADTRAININGDATA_H
#define LOADTRAININGDATA_H
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

// Silly class to read training data from a text file -- Replace This.
// Replace class TrainingData with whatever you need to get input data into the
// program, e.g., connect to a database, or take a stream of data from stdin, or
// from a file specified by a command line argument, etc.

class TrainingData
{
public:
    TrainingData(const string filename);
    bool isEof(void) { return m_trainingDataFile.eof(); }
    void getTopology(vector<unsigned> &topology);

    // Returns the number of input values read from the file:
    unsigned getNextInputs(vector<double> &inputVals);
    unsigned getTargetOutputs(vector<double> &targetOutputVals);

    //his was stupid so i added one thats nice for csv
    void setInputsandOuputs(vector<double> &targetOutputVals,vector<double> &inputVals);

private:
    ifstream m_trainingDataFile;
};

#endif // LOADTRAININGDATA_H

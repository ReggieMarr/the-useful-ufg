/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#include "onlinenn.h"

onlineNN::onlineNN()
{

}

NNModel onlineNN::readModel(ifstream &ModelName)
{
    NNModel returnModel;
    string line;

    while(getline(ModelName, line) && !ModelName.eof())
    {
        stringstream ss(line);
        string label;
        string searchedVar;

        if(ss >> label)
        {
            ss >> searchedVar;
            //cout << searchedVar << endl;

        }

    }




    return returnModel;
}

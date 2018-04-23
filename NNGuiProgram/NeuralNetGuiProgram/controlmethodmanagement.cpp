/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#include "controlmethodmanagement.h"
#include "iostream"



controlMethodManagement::controlMethodManagement()
{
     start = chrono::steady_clock::now();
}

void controlMethodManagement::executeCustomMethods(methodSetup methodsToRead)//TODO should be const qualified
{
    int comparisonType;
    double valueA;
    double valueB;
    int methodnum = 1;//methodsToRead.userSelection.size();
    int statementNum = methodsToRead.userSelection.size();
    double statementCombineType = 1;
    bool currentBool,lastBool,firstStatment = true,executeMethod = false;
    //start = chrono::steady_clock::now();


    chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    if(chrono::duration_cast<chrono::seconds>(end - start).count() > 25)
    {
        start = std::chrono::steady_clock::now();
    }


    for(int cycleMethods = 0;cycleMethods < methodnum;cycleMethods++)
    {
        for(int cycleStatements = 0;cycleStatements < statementNum;cycleStatements++)
        {
            /*
             *
             * If we are recreating a row and changing the layout it means that some information must be kept in the
             * customObjectMethods struct but some must be either deleted or altered to match the format of the new layout
             * ________________________________________________________________________________________
            * | Layout Type |      Visual Representation(showing combobox item 0)       | Cycle Size |
            * |_____________|___________________________________________________________|____________|
            * |      0      |[If][Time Passed][Seconds][<][Static Var][Line Edit][Then] |      6     |
            * |      1      |[If][Static Var][Line Edit][<][Time Passed][Seconds][Then] |      6     |
            * |      2      |[If][Time Passed][Seconds][<] [Tag Var][Then]              |      5     |
            * |      3      |[If][Tag Var][<][Time Passed][Seconds][Then]               |      5     |
            * |      4      |[If][Static Var][Line Edit][<][Tag Var][Then]              |      5     |
            * |      5      |[If][Tag Var][<][Static Var][Line Edit][Then]              |      5     |
            * |      6      |[If][Tag Var][<][Tag Var][Then]                            |      4     |
            * |      7      |[If][Tag Var][<][Time Passed][Seconds][Then]               |      5     |
            * ****************************************************************************************
            */

            switch(methodsToRead.layoutType.at(cycleStatements))
            {
                case 0:
                {
                    comparisonType = methodsToRead.userSelection.at(cycleStatements).at(3);
                    end = std::chrono::steady_clock::now();
                    valueA = (methodsToRead.userSelection.at(cycleStatements).at(2) == 1) ? chrono::duration_cast<chrono::milliseconds>(end - start).count()
                                                                      : chrono::duration_cast<chrono::seconds>(end - start).count();
                    valueB = methodsToRead.userSelection.at(cycleStatements).at(5);
                }
                    break;
                case 1:
                {
                    comparisonType = methodsToRead.userSelection.at(cycleStatements).at(3);
                    end = std::chrono::steady_clock::now();
                    valueB = (methodsToRead.userSelection.at(cycleStatements).at(5) == 1) ? chrono::duration_cast<chrono::milliseconds>(end - start).count()
                                                                      : chrono::duration_cast<chrono::seconds>(end - start).count();
                    valueA = methodsToRead.userSelection.at(cycleStatements).at(2);
                }
                    break;
                case 2:
                {
                    comparisonType = methodsToRead.userSelection.at(cycleStatements).at(3);
                    end = std::chrono::steady_clock::now();
                    valueA = (methodsToRead.userSelection.at(cycleStatements).at(2) == 1) ? chrono::duration_cast<chrono::milliseconds>(end - start).count()
                                                                      : chrono::duration_cast<chrono::seconds>(end - start).count();
                    valueB = methodsToRead.userSelection.at(cycleStatements).at(4);//TODO this needs to represent a tag
                }
                    break;
                case 3:
                {
                    comparisonType = methodsToRead.userSelection.at(cycleStatements).at(2);
                }
                    break;
                case 4:
                {
                    comparisonType = methodsToRead.userSelection.at(cycleStatements).at(3);
                }
                    break;
                case 5:
                {
                    comparisonType = methodsToRead.userSelection.at(cycleStatements).at(2);
                }
                    break;
                case 6:
                {
                    comparisonType = methodsToRead.userSelection.at(cycleStatements).at(2);
                }
                    break;
                case 7:
                {
                    comparisonType = methodsToRead.userSelection.at(cycleStatements).at(2);
                }
                    break;
            }


            switch (comparisonType)
            {
                case 0:
                {
                    currentBool = (valueA > valueB) ? true : false;
                }
                    break;
                case 1:
                {
                    currentBool = (valueA == valueB) ? true : false;
                }
                    break;
                case 2:
                {
                    currentBool = (valueA != valueB) ? true : false;
                }
                    break;
                case 3:
                {
                    currentBool = (valueA < valueB) ? true : false;
                }
                    break;
            }

            if(!firstStatment)
            {
                if(statementCombineType == 1)
                {
                    executeMethod = currentBool && lastBool;
                }
                else
                {
                    executeMethod = currentBool || lastBool;
                }
            }
            else
            {
                executeMethod = currentBool;
                ~firstStatment;
            }

            lastBool = currentBool;
        }
    }

    if(executeMethod)
    {
        cout << "Statement is true. " << "Val A is " << valueA << " Val B is " << valueB << endl;
    }
}

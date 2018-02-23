#include "controlmethodmanagement.h"



controlMethodManagement::controlMethodManagement()
{

}

void controlMethodManagement::executeCustomMethods()
{
    int inputType;
    int comparisonType;
    double valueA;
    double valueB;
    int methodnum = 10;
    int statementNum = 3;

    for(int cycleMethods = 0;cycleMethods < methodnum;cycleMethods++)
    {
        for(int cycleStatements = 0;cycleStatements < statementNum;cycleStatements++)
        {

            switch (inputType) {
            case 0:
            {
                switch (comparisonType) {
                case 0:
                {
                    if(valueA > valueB)
                    {

                    }
                }
                    break;
                case 1:
                {
                    if(valueA == valueB)
                    {

                    }
                }
                    break;
                case 2:
                {
                    if(valueA < valueB)
                    {

                    }
                }
                    break;
                }

            }

                break;
            case 1:
            {
                switch (comparisonType) {
                case 0:
                {
                    while(valueA > valueB)
                    {

                    }
                }
                    break;
                case 1:
                {
                    while(valueA == valueB)
                    {

                    }
                }
                    break;
                case 2:
                {
                    while(valueA < valueB)
                    {

                    }
                }
                    break;
                }
            }

                break;
            }

        }
    }


}

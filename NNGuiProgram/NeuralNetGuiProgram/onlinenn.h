#ifndef ONLINENN_H
#define ONLINENN_H
#include "nnconfigwindow.h"

class onlineNN
{
public:
    onlineNN();
    NNModel readModel(ifstream &ModelName);
private:

};

#endif // ONLINENN_H

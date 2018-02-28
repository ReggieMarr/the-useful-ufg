/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
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

/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#ifndef CONTROLMETHODMANAGEMENT_H
#define CONTROLMETHODMANAGEMENT_H
#include "executeneuralnet.h"
#include "runtime_window.h"
#include "chrono"

using namespace std;

class controlMethodManagement
{
public:
    controlMethodManagement();
    void executeCustomMethods(methodSetup methodsToRead);

private:
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
};

#endif // CONTROLMETHODMANAGEMENT_H

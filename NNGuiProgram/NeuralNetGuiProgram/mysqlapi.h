/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 *
 * This uses MySQLAPI Licensed under GPLV2.0 via the followling
  Oracle America, Inc.
  Attn: Senior Vice President
  Development and Engineering Legal
  500 Oracle Parkway, 10th Floor
  Redwood Shores, CA 94065
 */
#include <mysql/mysql.h>
#include <stdbool.h>
#include "string"
#ifndef MYSQLAPI_H
#define MYSQLAPI_H


class mySQLAPI
{
public:

    mySQLAPI();
    bool sql_main(char *sqlcmd, char ***SQLout, char *database, bool returnmode,
                  int columnsize, char *server, char *user, char *password);
};

#endif // MYSQLAPI_H

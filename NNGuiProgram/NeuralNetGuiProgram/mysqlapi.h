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

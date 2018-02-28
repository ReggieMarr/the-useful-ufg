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
#include <stdio.h>
#include "string.h"

#include "mysqlapi.h"

mySQLAPI::mySQLAPI()
{

}
//must be named main other returns error
bool mySQLAPI::sql_main(char *sqlcmd,char ***SQLout, char *database,bool returnmode,
                        int columnsize,char *server,char *user,char *password)
{

   MYSQL *conn;
   MYSQL_RES *res;
   MYSQL_ROW row;
   bool checkval;
   /*
   Setup for Learning Factory
       MySQL Database
   */
   //char *server = "192.168.0.20";
   //char *user = "dev";
   //char *password = "Factory1";


   int rowcnt = 0;

   conn = mysql_init(NULL);

   /* Connect to database */
   if (!mysql_real_connect(conn, server,
         user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      checkval = false;
      //exit(1);//error 101 means not on the same network
      //error 103 means invalid username/password
      //error 111 means firewall
   }
   else
   {
       checkval = true;
   }


   /* send SQL query */
   if (mysql_query(conn, sqlcmd)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      checkval = false;
      //exit(2);
   }

   res = mysql_use_result(conn);

   /* output table name */
   //printf("MySQL Tables in mysql database:\n");

   if (returnmode)
   {
       while ((row = mysql_fetch_row(res)) != NULL)
       {
           for (int coli = 0;coli < columnsize;coli++)
           {
               //printf("SQL Row # %d = %s \n",rowcnt, row[coli]);
               strcpy(SQLout[rowcnt][coli],row[coli]);
           }
           rowcnt++;
       }
   }



   /* close connection */
   mysql_free_result(res);
   mysql_close(conn);

   return checkval;
}

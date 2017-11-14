/* file: datasource_mysql.cpp */
//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2017 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================

/*
! Content:
!
! C++ sample of a MySQL data source.
!
! 1) Connect to the database and create a table there.
!
! 2) Create a dictionary from the data table and read the data from it using
!    the ODBCDataSource functionality. Print the data from the table.
!
! 3) Delete the table from the database and disconnect.
!
!******************************************************************************/

#if defined(_WIN32) || defined(_WIN64)
#define NOMINMAX
#include <windows.h>
#endif

#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include <iostream>
#include <ctime>

#include "daal.h"
#include "data_management/data_source/odbc_data_source.h"
#include "service.h"

using namespace daal;
using namespace std;

#if defined(_WIN64) || defined (__x86_64__)
string dataSourceName = "mySQL_test";
#else
string dataSourceName = "mySQL_test_32";
#endif

SQLRETURN connectServer();
SQLRETURN createUniqueTableName(string &tableName);
SQLRETURN createMySQLTable(string tableName);
SQLRETURN dropMySQLTable(string tableName);
SQLRETURN disconnectServer();
void      diagnoseError(string functionName, SQLHANDLE handle, SQLSMALLINT handleType);

/* External handles to connect to the database to manage a table for the sample */
SQLHENV   henv  = SQL_NULL_HENV;
SQLHDBC   hdbc  = SQL_NULL_HDBC;
SQLHSTMT  hstmt = SQL_NULL_HSTMT;
SQLRETURN ret;

int main(int argc, char *argv[])
{
    SQLRETURN ret;
    string tableName;

    /* Connect to the server via the ODBC API */
    ret = connectServer();
    if (!SQL_SUCCEEDED(ret)) { return ret; }

    /* Create a unique name for the data table */
    ret = createUniqueTableName(tableName);
    if (!SQL_SUCCEEDED(ret)) { return ret; }
    cout << "tableName = " << tableName << endl;

    /* Create the data table with the unique name */
    ret = createMySQLTable(tableName);
    if (!SQL_SUCCEEDED(ret)) { return ret; }

    /* Create ODBCDataSource to read from the MySQL database to HomogenNumericTable */
    ODBCDataSource<MySQLFeatureManager> dataSource(
        dataSourceName, tableName, "", "",
        DataSource::doAllocateNumericTable, DataSource::doDictionaryFromContext);

    /* Get the number of rows in the data table */
    size_t nRows = dataSource.getNumberOfAvailableRows();

    /* Load the number of rows from the data table */
    dataSource.loadDataBlock();

    /* Print the numeric table */
    printNumericTable(dataSource.getNumericTable());

    /* Free the connection handles to read the data from the data table in ODBCDataSource */
    dataSource.freeHandles();

    /* Delete the table from the data source */
    ret = dropMySQLTable(tableName);
    if (!SQL_SUCCEEDED(ret)) { return ret; }

    /* Disconnect from the server */
    ret = disconnectServer();
    if (!SQL_SUCCEEDED(ret)) { return ret; }

    return 0;
};

/* Connect to the server via the ODBC API */
SQLRETURN connectServer()
{
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLAllocHandle henv", henv, SQL_HANDLE_ENV); return ret; }

    ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_UINTEGER);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLSetEnvAttr", henv, SQL_HANDLE_ENV); return ret; }

    ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLAllocHandle hdbc", henv, SQL_HANDLE_ENV); return ret; }

    ret = SQLConnect(hdbc, (SQLCHAR *)dataSourceName.c_str(), SQL_NTS, NULL, 0, NULL, 0);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLConnect", hdbc, SQL_HANDLE_DBC); return ret; }

    return SQL_SUCCESS;
}

/* Create a unique table name with a connection ID as a key */
SQLRETURN createUniqueTableName(string &uniqueTableName)
{
    double connectionID;
    string query_exec = "SELECT CONNECTION_ID();";
    SQLHSTMT hstmt = SQL_NULL_HSTMT;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLAllocHandle createName", hdbc, SQL_HANDLE_DBC); return ret; }

    ret = SQLExecDirect(hstmt, (SQLCHAR *)query_exec.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLExecDirect createName", hstmt, SQL_HANDLE_STMT); return ret; }

    ret = SQLBindCol(hstmt, 1, SQL_C_DOUBLE, (SQLPOINTER)&connectionID, 0, NULL);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLBindCol createName", hstmt, SQL_HANDLE_STMT); return ret; }

    ret = SQLFetchScroll(hstmt, SQL_FETCH_NEXT, 1);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLFetchScroll createName", hstmt, SQL_HANDLE_STMT); return ret; }

    ret = SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLFreeHandle createName", hstmt, SQL_HANDLE_STMT); return ret; }

    time_t seconds = time(NULL);

    stringstream ss;
    ss << "daal_table" << "_" << connectionID << "_" << seconds;

    uniqueTableName = ss.str();

    return SQL_SUCCESS;
}

/* Create the data table with the unique table name */
SQLRETURN createMySQLTable(string tableName)
{
    SQLHSTMT hstmt = SQL_NULL_HSTMT;
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLAllocHandle create", hdbc, SQL_HANDLE_DBC); return ret; }

    string createTableSQLStatement = "CREATE TABLE " + tableName + " (DoubleColumn1 double, DoubleColumn2 double);";
    string insertSQLStatement      = "INSERT INTO " + tableName + " VALUES (1.23, 4.56), (7.89, 1.56), (2.62, 9.35);";

    ret = SQLExecDirect(hstmt, (SQLCHAR *)createTableSQLStatement.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLExecDirect create", hstmt, SQL_HANDLE_STMT); return ret; }

    ret = SQLExecDirect(hstmt, (SQLCHAR *)insertSQLStatement.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLExecDirect insert", hstmt, SQL_HANDLE_STMT); return ret; }

    ret = SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLFreeHandle create", hstmt, SQL_HANDLE_STMT); return ret; }

    return SQL_SUCCESS;
}

/* Drop the table with the unique table name */
SQLRETURN dropMySQLTable(string tableName)
{
    string dropTableSQLStatement = "DROP TABLE " + tableName + ";";
    SQLHSTMT hstmt = SQL_NULL_HSTMT;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLAllocHandle drop", hdbc, SQL_HANDLE_DBC); return ret; }

    ret = SQLExecDirect(hstmt, (SQLCHAR *)dropTableSQLStatement.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLExecDirect drop", hstmt, SQL_HANDLE_STMT); return ret; }

    ret = SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLFreeHandle drop", hstmt, SQL_HANDLE_STMT); return ret; }

    return SQL_SUCCESS;
}

/* Disconnect from the server via the ODBC API */
SQLRETURN disconnectServer()
{
    ret = SQLDisconnect(hdbc);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLDisconnect", hdbc, SQL_HANDLE_DBC); return ret; }

    ret = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLFreeHandle hdbc disconnect", hdbc, SQL_HANDLE_DBC); return ret; }

    ret = SQLFreeHandle(SQL_HANDLE_ENV, henv);
    if (!SQL_SUCCEEDED(ret)) { diagnoseError("SQLFreeHandle henv disconnect", henv, SQL_HANDLE_ENV); return ret; }

    return SQL_SUCCESS;
}

/* Diagnose an ODBC error */
void diagnoseError(string functionName, SQLHANDLE handle, SQLSMALLINT handleType)
{
    SQLINTEGER nativeError;
    SQLCHAR SQLState[6], messageText[SQL_MAX_MESSAGE_LENGTH];
    SQLSMALLINT i, textLength;
    SQLRETURN ret = SQL_SUCCESS;
    cout << endl << "The driver returns diagnostic message from function " << functionName << endl << endl;

    i = 1;
    while ( ret != SQL_NO_DATA )
    {
        ret = SQLGetDiagRec(handleType, handle, i++, SQLState, &nativeError, messageText, sizeof(messageText), &textLength);
        if (SQL_SUCCEEDED(ret))
        {
            cout << SQLState << ":" << i << ":" << nativeError << ":" << messageText << endl;
        }
    }
}

/* Check the status. In case of failure, drop the table, disconnect, and exit with an error code */
void free(string tableName, string functionName)
{
    cout << "Error in function:" << functionName << endl;
    dropMySQLTable(tableName);
    disconnectServer();
}

//
//  database_sqlite3.hpp
//  sqlite3-test
//
//  Created by Alex Simmons on 21/11/18.
//  Copyright © 2018 Alex Simmons. All rights reserved.
//

#ifndef database_sqlite3_hpp
#define database_sqlite3_hpp
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <vector>
#include <tuple>
#include <string.h>
#include <vector>
#include <algorithm>
#include "../util.hpp"

using namespace std;

typedef vector<tuple<string, string>> vector_tuple;

struct FileInsertStatement {
    string FILENAME;
    string PATH;
    string FILETYPE;
    string DATATYPE;
    string DATE;
};

struct DataTypes {
    static const string TLE;
    static const string HISTORICAL_TLE;
    static const string SATCAT;
};

struct TableTypes {
    static const string TLE;
    static const string TLE_LATEST;
    static const string TLE_PUBLISH;
    static const string OMM;
    static const string BOXSCORE;
    static const string SATCAT;
    static const string LAUNCH_SITE;
    static const string SATCAT_CHANGE;
    static const string SATCAT_DEBUT;
    static const string DECAY;
    static const string TIP;
    static const string ANNOUNCEMENT;
    static const string FILES;
};

struct Table {
    string name;
    string type;
};

struct Tables {
    Table TLE {"TLE", TableTypes::TLE};
    Table SATCAT{"SATCAT", TableTypes::SATCAT};
    Table FILES{"FILES", TableTypes::FILES};
    vector<tuple<string, Table>> Satelites;
};

class PreparedTable {
public:
    PreparedTable();
    void appendColumn(string, string);
    vector_tuple* getColumns();
    void setTableName(string);
    string getTableName();
private:
    string tablename;
    vector_tuple columns;
};

class TableQuery {
public:
    vector< tuple<string, string>> args;
    TableQuery(void);
    void append(string, string);
    void prepareTable(struct Tables);
};

class Database {
public:
    Database(const char *, bool);
    ~Database(void);
    Tables T;
    void select(const char *);
    void createTable(PreparedTable*);
    void insertFromFile(string, struct Table);
    void insertFromString(struct FileInsertStatement, struct Table);
    void printResults(vector<vector<string>>*);
    std::vector<std::vector<std::string> > * selectStatement(string);
    sqlite3 *db;
private:
    char *zErrMsg = 0;
    int rc;
    char *sql;
    bool checkSqlExec();
    void openDatabase(const char *);
    void insertStatement(string);
    void selectStatement(char *);
    void iniitialiseSATCATTable(Table);
    void initialiseTLETable(Table);
    void initialiseDownloadsTable(Table);
    vector_tuple getTableColumns(string);
    
};
#endif /* database_sqlite3_hpp */

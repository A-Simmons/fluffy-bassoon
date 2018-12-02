//
//  database_sqlite3.cpp
//  sqlite3-test
//
//  Created by Alex Simmons on 21/11/18.
//  Copyright © 2018 Alex Simmons. All rights reserved.
//

#include "database_sqlite3.hpp"
#define BUFFER_SIZE 1024

static void inputBufferRemoveStringCharacter(FILE *pFile, char *sInputBuf) {
    fgets (sInputBuf, BUFFER_SIZE, pFile);
    string strInputBuf = string(sInputBuf);
    strInputBuf.erase(std::remove(strInputBuf.begin(), strInputBuf.end(), '"'), strInputBuf.end());
    strcpy(sInputBuf, strInputBuf.c_str());
}

static void importTLE(FILE *pFile, sqlite3_stmt *stmt, bool ordinal) {
    char sInputBuf [BUFFER_SIZE] = "\0";
    char * sNORAD_CAT_ID = 0;
    char * sEPOCH = 0;
    char  ROWID [BUFFER_SIZE] = "\0";
    char  DATE [BUFFER_SIZE] = "\0";
    int i = 2;
    inputBufferRemoveStringCharacter(pFile, sInputBuf);
    if ((sInputBuf[0] != '\0') and (sInputBuf[0] != '\n') and (sInputBuf[0] != '\r')) {
        strtok_new(sInputBuf, ","); /* ORDINAL or COMMENT */
        if (ordinal)
            strtok_new(NULL, ","); /* COMMENT */
        strtok_new (NULL, ","); /* ORIGINATOR */
        sNORAD_CAT_ID = strtok_new (NULL, ","); /* NORAD_CAT_ID */
        sqlite3_bind_text(stmt, i++, sNORAD_CAT_ID , -1, SQLITE_TRANSIENT); /* NORAD_CAT_ID */
        strtok_new (NULL, ","); /* OBJECT_NAME */
        sqlite3_bind_text(stmt, i++, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); /* OBJECT_TYPE */
        strtok_new (NULL, ","); /* CLASSIFICATION_TYPE */
        sqlite3_bind_text(stmt, i++, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); /* INTLDES */
        sEPOCH = strtok_new (NULL, ","); /* EPOCH */
        sqlite3_bind_text(stmt, i++, sEPOCH , -1, SQLITE_TRANSIENT); /* EPOCH */
        sqlite3_bind_text(stmt, i++, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); /* EPICH_MICROSECONDS */
        strtok_new (NULL, ","); /* MEAN_MOTION */
        strtok_new (NULL, ","); /* ECCENTRICITY */
        strtok_new (NULL, ","); /* INCLINATION */
        strtok_new (NULL, ","); /* RA_OF_ASC_NODE */
        strtok_new (NULL, ","); /* ARG_OF_PERICENTER */
        strtok_new (NULL, ","); /* MEAN_ANOMALY */
        strtok_new (NULL, ","); /* EPHEMERIS_TYPE */
        strtok_new (NULL, ","); /* ELEMENT_SET_NO */
        strtok_new (NULL, ","); /* REV_AT_EPOCH */
        strtok_new (NULL, ","); /* BSTAR */
        strtok_new (NULL, ","); /* MEAN_MOTION_DOT */
        strtok_new (NULL, ","); /* MEAN_MOTION_DDOT */
        strtok_new (NULL, ","); /* FILE */
        sqlite3_bind_text(stmt, i++, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); /* TLE_LINE0 */
        sqlite3_bind_text(stmt, i++, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); /* TLE_LINE1 */
        sqlite3_bind_text(stmt, i++, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); /* TLE_LINE2 */
        
        sprintf(DATE, "%s-%s-%s", std::string(&sEPOCH[0], &sEPOCH[4]).c_str(), std::string(&sEPOCH[5], &sEPOCH[7]).c_str(), std::string(&sEPOCH[8], &sEPOCH[10]).c_str());
        sqlite3_bind_text(stmt, i++, DATE, -1, SQLITE_TRANSIENT); /* DATE */
        sprintf(ROWID, "%s%s%s%s", sNORAD_CAT_ID, std::string(&sEPOCH[0], &sEPOCH[4]).c_str(), std::string(&sEPOCH[5], &sEPOCH[7]).c_str(), std::string(&sEPOCH[8], &sEPOCH[10]).c_str());
        sqlite3_bind_text(stmt, 1, ROWID, -1, SQLITE_TRANSIENT);
    }
    
}

static void importSATCAT(FILE *pFile, sqlite3_stmt *stmt) {
    char sInputBuf [BUFFER_SIZE] = "\0";
    inputBufferRemoveStringCharacter(pFile, sInputBuf);
    sqlite3_bind_text(stmt, 1, strtok_new (sInputBuf, ",") , -1, SQLITE_TRANSIENT); //INTLDES //
    sqlite3_bind_text(stmt, 2, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // NORAD_CAT_ID //
    sqlite3_bind_text(stmt, 3, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // OBJECT_TYPE //
    sqlite3_bind_text(stmt, 4, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // SATNAME //
    sqlite3_bind_text(stmt, 5, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // COUNTRY //
    sqlite3_bind_text(stmt, 6, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // LAUNCH //
    sqlite3_bind_text(stmt, 7, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // SITE //
    sqlite3_bind_text(stmt, 8, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // DECAY //
    sqlite3_bind_text(stmt, 9, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // PERIOD //
    sqlite3_bind_text(stmt, 10, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // INCLINATION //
    sqlite3_bind_text(stmt, 11, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // APOGEE //
    sqlite3_bind_text(stmt, 12, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // PERIGEE //
    sqlite3_bind_text(stmt, 13, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // COMMENT //
    sqlite3_bind_text(stmt, 14, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // COMMENTCODE //
    sqlite3_bind_text(stmt, 15, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // RCSVALUE //
    sqlite3_bind_text(stmt, 16, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // RCS_SIZE //
    sqlite3_bind_text(stmt, 17, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // FILE //
    sqlite3_bind_text(stmt, 18, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // LAUNCH_YEAR //
    sqlite3_bind_text(stmt, 19, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // LAUNCH_NUM //
    sqlite3_bind_text(stmt, 20, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // LAUNCH_PIECE //
    sqlite3_bind_text(stmt, 21, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // CURRENT //
    sqlite3_bind_text(stmt, 22, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // OBJECT_NAME //
    sqlite3_bind_text(stmt, 23, strtok_new (NULL, ","), -1, SQLITE_TRANSIENT); // OBJECT_ID //
    //FIXME: Find out why last column for SATCAT doesn't play nice
}

static void getSqlInsertStatement(char *sSQL, const std::string &tablename, const std::string &type) {
    string values;
    if (type == TableTypes::TLE)
        values = "(@ROWID, @NCID, @OT, @INT, @EP, @EPMS, @TL0, @TL1, @TL2, @EDATE)";
    else if (type == TableTypes::SATCAT)
        values = "(@INTDES, @NCID, @OT, @SN, @COUNT, @LAUNCH, @SITE, @DECAY, @PERIOD, @INC, @APO, @PERI, @COM, @CC, @RCSV, @RCSS, @FILE, @LY, @LN, @LP, @CURR, @ON, @OID)";
    else if (type == TableTypes::FILES)
        values = "(@FNAME, @PATH, @FTYPE, @DTYPE, @DATE)";
    sprintf(sSQL, "INSERT INTO %s VALUES %s", tablename.c_str(), values.c_str());
}

Database::Database(const char *dbname, bool dropTables) {
    this->openDatabase(dbname);
    if (dropTables) {
        sqlite3_exec(this->db, "DROP TABLE IF EXISTS SATCAT;", NULL, NULL, NULL);
        sqlite3_exec(this->db, "DROP TABLE IF EXISTS TLE;", NULL, NULL, NULL);
        sqlite3_exec(this->db, "DROP TABLE IF EXISTS FILES;", NULL, NULL, NULL);
    }
    this->iniitialiseSATCATTable(this->T.SATCAT);
    this->initialiseTLETable(this->T.TLE);
    this->initialiseDownloadsTable(this->T.FILES);
    
}

Database::~Database(void) {
    sqlite3_close(this->db);
}

void Database::openDatabase(const char *dbname) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    this->rc = sqlite3_open(dbname, &(this->db));
    if (!(this->checkSqlExec())) {
        exit(0);
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Opened Database = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "seconds" << std::endl;
    

}

std::vector<std::vector<std::string> > * Database::selectStatement(string sqlStatement) {
    std::vector< std::vector< std::string > > * result = new std::vector< std::vector< std::string > >;
    std::vector< std::string > rowResult;
    sqlite3_stmt * stmt;
    sqlite3_prepare_v2(this->db, sqlStatement.c_str(), -1, &stmt, NULL); //preparing the statement
    sqlite3_step( stmt );//executing the statement
    string text;
    while( sqlite3_column_text( stmt, 0 ) )
    {
        rowResult.clear();
        for( int i = 0; i < sqlite3_column_count(stmt); i++ ) {
            text = std::string( (char *)sqlite3_column_text( stmt, i ) );
            if (text == "") text = "NULL";
            rowResult.push_back( text );
        }
        result->push_back(rowResult);
        sqlite3_step( stmt );
    }
    return result;
}

void Database::createTable(PreparedTable* pTable) {
    string sql = "CREATE TABLE IF NOT EXISTS ";
    sql.append(pTable->getTableName());
    sql.append("( ");
    vector_tuple* columns = pTable->getColumns();
    for (vector_tuple::const_iterator i = columns->begin(); i != columns->end(); ++i) {
        sql.append(get<0>(*i));
        sql.append(" ");
        sql.append(get<1>(*i));
        if (next(i) != columns->end())
            sql.append(", ");
    }
    sql.append(" );");
    this->rc = sqlite3_exec(this->db, sql.c_str(), 0, 0, &(this->zErrMsg));
    
}

bool Database::checkSqlExec() {
    return !(this->rc);
}

void Database::iniitialiseSATCATTable(Table T) {
    PreparedTable pTable;
    pTable.setTableName(T.name);
    pTable.appendColumn("INTLDES", "TEXT");
    pTable.appendColumn("NORAD_CAT_ID", "INTEGER PRIMARY KEY");
    pTable.appendColumn("OBJECT_TYPE", "TEXT");
    pTable.appendColumn("SATNAME", "TEXT");
    pTable.appendColumn("COUNTRY", "TEXT");
    pTable.appendColumn("LAUNCH", "DATE");
    pTable.appendColumn("SITE", "TEXT");
    pTable.appendColumn("DECAY", "DATE");
    pTable.appendColumn("PERIOD", "FLOAT");
    pTable.appendColumn("INCLINATION", "FLOAT");
    pTable.appendColumn("APOGEE", "INTEGER");
    pTable.appendColumn("PERIGEE", "INTEGER");
    pTable.appendColumn("COMMENT", "TEXT");
    pTable.appendColumn("COMMENTCODE", "TEXT");
    pTable.appendColumn("RCSVALUE", "INTEGER");
    pTable.appendColumn("RCS_SIZE", "TEXT");
    pTable.appendColumn("FILE", "INTEGER");
    pTable.appendColumn("LAUNCH_YEAR", "INTEGER");
    pTable.appendColumn("LAUNCH_NUM", "INTEGER");
    pTable.appendColumn("LAUNCH_PIECE", "INTEGER");
    pTable.appendColumn("CURRENT", "TEXT");
    pTable.appendColumn("OBJECT_NAME", "TEXT");
    pTable.appendColumn("OBJECT_ID", "TEXT");
    this->createTable(&pTable);
}

void Database::initialiseTLETable(Table T) {
    PreparedTable pTable;
    pTable.setTableName(T.name);
    pTable.appendColumn("ROWID", "INTEGER PRIMARY KEY");
    pTable.appendColumn("NORAD_CAT_ID", "INTEGER");
    pTable.appendColumn("OBJECT_TYPE", "TEXT");
    pTable.appendColumn("INTLDES", "TEXT");
    pTable.appendColumn("EPOCH", "DATETIME");
    pTable.appendColumn("EPOCH_MICROSECONDS", "DATE");
    pTable.appendColumn("TLE_LINE0", "TEXT");
    pTable.appendColumn("TLE_LINE1", "TEXT");
    pTable.appendColumn("TLE_LINE2", "TEXT");
    pTable.appendColumn("EPOCH_DATE", "DATE");
    this->createTable(&pTable);
}

void Database::initialiseDownloadsTable(Table T) {
    PreparedTable pTable;
    pTable.setTableName(T.name);
    pTable.appendColumn("FILENAME", "TEXT");
    pTable.appendColumn("PATH", "TEXT");
    pTable.appendColumn("FILETYPE", "TEXT");
    pTable.appendColumn("DATATYPE", "DATETIME");
    pTable.appendColumn("DATE", "DATE");
    this->createTable(&pTable);
}

void Database::insertFromString(struct FileInsertStatement str, struct Table tab) {
    char * sErrMsg = 0;
    char sSQL [BUFFER_SIZE] = "\0";
    sprintf(sSQL, "INSERT INTO %s VALUES ('%s', '%s', '%s', '%s', date('%s'))", tab.name.c_str(), str.FILENAME.c_str(), str.PATH.c_str(), str.FILETYPE.c_str(), str.DATATYPE.c_str(), str.DATE.c_str());
    sqlite3_exec(this->db, sSQL, NULL, NULL, &sErrMsg);
}

void Database::insertFromFile(string path, struct Table table_type) {
    sqlite3_stmt * stmt;
    char * sErrMsg = 0;
    const char * tail = 0;
    int n = 0;
    FILE * pFile;
    
    sqlite3_exec(this->db, "PRAGMA journal_mode = MEMORY", NULL, NULL, &sErrMsg);
    char sSQL [BUFFER_SIZE] = "\0";
    getSqlInsertStatement(sSQL, table_type.name, table_type.type);  /* PREPARE STATEMENT */
    
    sqlite3_prepare_v2(this->db, sSQL, BUFFER_SIZE, &stmt, &tail);
    sqlite3_exec(this->db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
    
    pFile = fopen (path.c_str(),"r");
    char header [BUFFER_SIZE] = "\0";
    bool headerOrd;
    if (!feof(pFile)) {// Skip Header
        inputBufferRemoveStringCharacter(pFile, header);
        headerOrd = strncmp(strtok_new (header, ","), "ORDINAL", 3) == 0; // Test if header starts with ORDINAL so we can skip it in TLE
    }
    while (!feof(pFile)) {
        if (table_type.type == TableTypes::TLE) {
            importTLE(pFile, stmt, headerOrd);
        } else if (table_type.type == TableTypes::SATCAT)
            importSATCAT(pFile, stmt);
        
        sqlite3_step(stmt);
        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
        n++;
    }
    fclose (pFile);
    sqlite3_exec(this->db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    sqlite3_finalize(stmt);
}

void Database::printResults(vector<vector<string> > * results) {
    for (std::vector< std::vector<std::string> >::iterator i = results->begin(); i != results->end(); ++i) {
        for (std::vector<std::string>::const_iterator j = i->begin(); j != i->end(); ++j) {
            cout << *j << ", ";
        }
        std::cout << std::endl;
    }
}

vector_tuple Database::getTableColumns(string tablename) {
    sqlite3_stmt *statement;
    string colName;
    string colType;
    vector_tuple columns;
    string sql="PRAGMA table_info(";
    sql.append(tablename);
    sql.append(");");
    this->rc = sqlite3_prepare_v2(this->db, sql.c_str(), -1, &statement, 0);
    if (this->rc==SQLITE_OK) {
        while(sqlite3_step(statement) == SQLITE_ROW) {
            colName = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)));
            colType = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)));
            columns.push_back(tuple<string, string> (colName, colType));
        }
    }
    return columns;
}

PreparedTable::PreparedTable() {
    
}

void PreparedTable::setTableName(string name) {
    this->tablename = name;
}

string PreparedTable::getTableName() {
    return (this->tablename);
}

vector_tuple* PreparedTable::getColumns() {
    return &(this->columns);
}

void PreparedTable::appendColumn(string name, string type) {
    this->columns.push_back(tuple<string, string> (name, type));
}

TableQuery::TableQuery() {
    
}

void TableQuery::append(string field, string value) {
    this->args.push_back( make_tuple(field, value));
}


/* TABLE TYPES */
const string TableTypes::TLE = "TLE";
const string TableTypes::TLE_LATEST = "TLE_LATEST";
const string TableTypes::TLE_PUBLISH = "TLE_PUBLISH";
const string TableTypes::OMM = "OMM";
const string TableTypes::BOXSCORE = "BOXSCORE";
const string TableTypes::SATCAT = "SATCAT";
const string TableTypes::LAUNCH_SITE = "LAUNCH_SITE";
const string TableTypes::SATCAT_CHANGE = "SATCAT_CHANGE";
const string TableTypes::SATCAT_DEBUT = "SATCAT_DEBUT";
const string TableTypes::DECAY = "DECAY";
const string TableTypes::TIP = "TIP";
const string TableTypes::ANNOUNCEMENT = "ANNOUNCEMENT";
const string TableTypes::FILES = "FILES";


const string DataTypes::TLE = "TLE";
const string DataTypes::HISTORICAL_TLE = "HISTORICAL_TLE";
const string DataTypes::SATCAT = "SATCAT";

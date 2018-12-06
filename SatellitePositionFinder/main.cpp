#include <iostream>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <ctime>
#include <chrono>
#include <dirent.h>
#include <cstring>
#include <vector>
#include "SpaceTrack/spacetrack.cpp"
#include "SpaceTrack/spacetrackquery.cpp"
#include "Database/database_sqlite3.cpp"

using namespace std;
namespace
{
std::vector<std::string> GetDirectoryFiles(const std::string &dir)
{
    std::vector<std::string> files;
    std::shared_ptr<DIR> directory_ptr(opendir(dir.c_str()), [](DIR *dir) { dir &&closedir(dir); });
    struct dirent *dirent_ptr;
    if (!directory_ptr)
    {
        std::cout << "Error opening : " << std::strerror(errno) << dir << std::endl;
        return files;
    }

    while ((dirent_ptr = readdir(directory_ptr.get())) != nullptr)
    {
        files.push_back(std::string(dirent_ptr->d_name));
    }
    return files;
}
} // namespace

void getHistoricalTLEData(SpaceTrackConn *c, string path_file, string start, string end)
{
    auto start_clock = std::chrono::high_resolution_clock::now();
    SpaceTrackQuery *q = new SpaceTrackQuery();
    q->setClass("tle");
    q->setFormat("csv");
    q->setFileName(path_file);
    string date = start;
    date.append("--");
    date.append(end);
    q->append("EPOCH", date);
    q->append("OBJECT_TYPE", "<>DEBRIS");
    string res = c->getData(q);
    auto finish_clock = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_acquire = finish_clock - start_clock;
    std::cout << "Acquire Satellite Data: " << elapsed_acquire.count() << " s\n";

    // ---- Save to file ---- //
    streamToFile(res, path_file);
}

void interestedSatellite(SpaceTrackConn *c, string path_file)
{
    auto start = std::chrono::high_resolution_clock::now();
    // ---- Interested Satellite Query ---- //
    SpaceTrackQuery *q = new SpaceTrackQuery();
    q->setClass("satcat");
    q->setFormat("csv");
    q->setFileName(path_file);
    q->append("COUNTRY", "<>null-value");
    q->append("OBJECT_TYPE", "<>DEBRIS");
    q->append("DECAY", "null-value");
    q->append("CURRENT", "Y");
    c->receiveSpeed = 5000 * 1024;
    string res = c->getData(q);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_acquire = finish - start;
    std::cout << "Acquire Satellite Data: " << elapsed_acquire.count() << " s\n";

    // ---- Save to file ---- //
    start = std::chrono::high_resolution_clock::now();
    streamToFile(res, path_file);
    finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_save_to_file = finish - start;
    std::cout << "Save data to file: " << elapsed_save_to_file.count() << " s\n";
}

static void testSelectStatement(bool printResults, const std::string &sqlStatement, Database *test)
{
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    begin = std::chrono::steady_clock::now();
    vector<vector<string>> *results = test->selectStatement(sqlStatement);
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "seconds" << std::endl;
    if (printResults)
        test->printResults(results);
}

unsigned long existsSelectStatement(const std::string &sqlStatement, Database *db)
{
    vector<vector<string>> *results = db->selectStatement(sqlStatement);
    return strtoul(results->begin()->begin()->c_str(), NULL, 0);
}

static void getTLEData(string startDateStr, string type, string path, Database *db, SpaceTrackConn &c)
{
    char startDate[100];
    char endDate[100];
    struct FileInsertStatement FIS;
    string name, path_file, sqlStatement, start, end;
    unsigned long fileExistInDatabase;
    bool fileExists;
    // Download Date
    struct tm dDate = {0};
    strptime(startDateStr.c_str(), "%Y-%m-%dT", &dDate);
    dDate.tm_isdst = 0;
    long downloadDate = mktime(&dDate);

    // Current Date (to not exceed)
    time_t timeNow;
    time(&timeNow);

    while (downloadDate < timeNow)
    {
        //cout << "Updating Historical TLE records." << endl;
        strftime(startDate, sizeof(startDate), "%Y-%m-%d", &dDate);
        dDate.tm_mday += 1;
        mktime(&dDate);
        downloadDate = mktime(&dDate);
        strftime(endDate, sizeof(endDate), "%Y-%m-%d", &dDate);
        start = string(startDate);
        name = type;
        name.append("_");
        name.append(start);
        name.append(".csv");
        path_file = path;
        path_file.append(name);
        FIS.FILENAME = name;
        FIS.PATH = path;
        FIS.DATE = start;
        FIS.FILETYPE = "csv";
        FIS.DATATYPE = type;
        FIS.IMPORTED = false;
        sqlStatement = "SELECT COUNT(*) FROM FILES WHERE FILENAME = '";
        sqlStatement.append(name);
        sqlStatement.append("';");
        fileExistInDatabase = existsSelectStatement(sqlStatement, db);
        fileExists = file_exists(path_file);
        if ((fileExistInDatabase == 0) and (fileExists == false))
        {
            getHistoricalTLEData(&c, path_file, startDate, endDate);
            db->insertFromString(FIS, db->T.FILES);
        }
        else if ((fileExistInDatabase == 0) and (fileExists == true))
        {
            db->insertFromString(FIS, db->T.FILES);
        }
        else if (fileExistInDatabase != 0)
        {
        }
    }
    cout << "Historical records updated." << endl;
}

static void importFromDirectory(Database *test)
{
    std::string path = "/Historical_TLE_Data/";
    //getTLEData(c);
    const auto &directory_path = std::string("./Historical_TLE_Data/");
    string sql = "SELECT IMPORTED FROM FILES WHERE FILENAME='";
    string ss;
    vector<string> files = GetDirectoryFiles(directory_path);
    sort(files.begin(), files.end());
    string dir_path = "./Historical_TLE_Data";
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    begin = std::chrono::steady_clock::now();
    for (const auto &file : files)
    {
        if ((file == ".") or (file == "..") or (file == ".DS_Store"))
            continue;
        ss = sql;
        ss.append(file);
        ss.append("';");
        vector<vector<string>> *result = test->selectStatement(ss);
        if (strcmp(result->begin()->begin()->c_str(), "0") == 0)
        {
            path = directory_path;
            path.append(file);
            std::cout << "Importing: " << file << std::endl;
            test->insertFromFile(path, test->T.TLE);
            ss = "UPDATE FILES SET IMPORTED=1 WHERE FILENAME='";
            ss.append(file);
            ss.append("';");
            test->selectStatement(ss);
        }
        else
        {
            //std::cout << "Already Imported: " << file << std::endl;
        }
    }
    end = std::chrono::steady_clock::now();
    std::cout << "All files imported: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "seconds" << std::endl;
}

int main(int argc, char *argv[])
{
    // INITIALISE PARAMETERS //
    string startDate = "2016-01-01";
    double lat0 = -51.794;
    double long0 = -25.910;
    double inc = 5.0/60;
    double radius = 200;
    string startTimeStr = "2018-12-6 05:45:00";
    string endTimeStr = "2018-12-6 07:45:00";

    SpaceTrackConn c("h2807809@nwytg.net", "z5nfI0sgeHob3t0PZF2uP364inQe8", "https://www.space-track.org/ajaxauth/login");

    Database *test = new Database("SatelliteData.db", false);
    getTLEData(startDate, "HISTORICAL_TLE", "Historical_TLE_Data/", test, c);
    importFromDirectory(test);
    string sqlStatement;
    
    sqlStatement = "CREATE INDEX IF NOT EXISTS NCID on TLE (NORAD_CAT_ID);";
    cout << "Create index if not exists for NORAD_CAT_ID in table TLE: ";
    testSelectStatement(false, sqlStatement, test);
    sqlStatement = "CREATE INDEX IF NOT EXISTS EPOCHINDEX on TLE (EPOCH);";
    cout << "Create index if not exists for EPOCH in table TLE: ";
    testSelectStatement(false, sqlStatement, test);

    sqlStatement = "SELECT n.* FROM TLE n INNER JOIN ( SELECT NORAD_CAT_ID, MAX(EPOCH) as EPOCH FROM TLE WHERE EPOCH < date('2018-12-10') AND OBJECT_TYPE = 'PAYLOAD' GROUP BY NORAD_CAT_ID ) as max USING (NORAD_CAT_ID, EPOCH);";
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    begin = std::chrono::steady_clock::now();
    vector<vector<string>>* results = test->selectStatement(sqlStatement);
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "seconds" << std::endl;

    begin = std::chrono::steady_clock::now();
    runTLE2(results, startTimeStr, endTimeStr, lat0, long0, inc, radius);
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "seconds" << std::endl;
    
    return 0;
}

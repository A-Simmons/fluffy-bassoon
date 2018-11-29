#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include "SpaceTrack/spacetrack.hpp"
#include "SpaceTrack/spacetrackquery.hpp"
#include "nlohmann/json.h"
#include "Database/database_sqlite3.hpp"
#include <dirent.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <memory>
using json = nlohmann::json;
//#include <filesystem>
using namespace std;

namespace {
    std::vector<std::string> GetDirectoryFiles(const std::string& dir) {
        std::vector<std::string> files;
        std::shared_ptr<DIR> directory_ptr(opendir(dir.c_str()), [](DIR* dir){ dir && closedir(dir); });
        struct dirent *dirent_ptr;
        if (!directory_ptr) {
            std::cout << "Error opening : " << std::strerror(errno) << dir << std::endl;
            return files;
        }
        
        while ((dirent_ptr = readdir(directory_ptr.get())) != nullptr) {
            files.push_back(std::string(dirent_ptr->d_name));
        }
        return files;
    }
}  // namespace

 static void streamToFile(const std::string &res, std::string fileName) {
     std::ofstream o(fileName);
     o << res;
     o.close();
 }
 
 
 void getHistoricalTLEData(SpaceTrackConn* c, string path_file, string start, string end) {
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

 void interestedSatellite(SpaceTrackConn* c, string path_file) {
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
     c->receiveSpeed = 5000*1024;
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
 
 #define InterestedSatCat "SATCAT_Example.csv"
 
 static void getTLEData(SpaceTrackConn &c) {
     time_t timeNow;
     time (&timeNow);
     
     struct tm tm;
     char startDate[100];
     char endDate[100];
     strptime("2018 05 14", "%Y %m %d", &tm);
     
     cout << (mktime(&tm) < timeNow) << endl;
     while (mktime(&tm) < timeNow) {
         strftime(startDate, sizeof(startDate), "%Y-%m-%d", &tm);
         tm.tm_mday += 1;
         mktime(&tm);
         strftime(endDate, sizeof(endDate), "%Y-%m-%d", &tm);
         string start = string(startDate);
         string end = string(endDate);
         string path_file = "Historical_TLE_Data/test_";
         path_file.append(start);
         path_file.append("--");
         path_file.append(end);
         path_file.append(".csv");
         getHistoricalTLEData(&c, path_file, startDate, endDate);
     }
 }

static void importFromDirectory(Database *test) {
    std::string path = "/Historical_TLE_Data/";
    //getTLEData(c);
    const auto& directory_path = std::string("./Historical_TLE_Data/");
    vector<string> files = GetDirectoryFiles(directory_path);
    sort(files.begin(), files.end());
    string dir_path = "./Historical_TLE_Data";
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    begin = std::chrono::steady_clock::now();
    for (const auto& file : files) {
        if ((file == ".") or (file == "..") or (file == ".DS_Store"))
            continue;
        path = directory_path;
        path.append(file);
        std::cout << "Importing: " << file << std::endl;
        test->insertFromFile(path, test->T.TLE);
    }
    end = std::chrono::steady_clock::now();
    std::cout << "All files imported: " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "seconds" << std::endl;
}

static void testSelectStatement(bool printResults, const std::string &sqlStatement, Database *test) {
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    begin = std::chrono::steady_clock::now();
    vector<vector<string>> * results = test->selectStatement(sqlStatement);
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "seconds" << std::endl;
    if (printResults)
        test->printResults(results);
}

int main(int argc, char *argv[])
{
    // Space Track Connection
    
     SpaceTrackConn c("h2807809@nwytg.net", "z5nfI0sgeHob3t0PZF2uP364inQe8", "https://www.space-track.org/ajaxauth/login");
    

    
    //interestedSatellite(&c, InterestedSatCat);
    
    // Database
    Database* test = new Database("TEST.db");
    //test->insertFromFile(InterestedSatCat, test->T.SATCAT);
    //test->insertFromFile("TLE_Example2.csv", test->T.TLE);
    //importFromDirectory(test);
    
    /*
    string sqlStatement = "CREATE INDEX IF NOT EXISTS NCID on TLE (NORAD_CAT_ID);";
    begin = std::chrono::steady_clock::now();
    test->selectStatement(sqlStatement);
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "seconds" << std::endl;
    test->printResults(test->selectStatement(sqlStatement));

    */
    string sqlStatement = "SELECT n.* FROM TLE n INNER JOIN ( SELECT NORAD_CAT_ID, MAX(EPOCH) as EPOCH FROM TLE WHERE EPOCH < date('2018-06-02') AND OBJECT_TYPE = 'PAYLOAD' GROUP BY NORAD_CAT_ID ) as max USING (NORAD_CAT_ID, EPOCH);";
    testSelectStatement(false, sqlStatement, test);
    

    sqlStatement = "SELECT COUNT(*) FROM TLE;";
    testSelectStatement(true, sqlStatement, test);
    return 0;
}

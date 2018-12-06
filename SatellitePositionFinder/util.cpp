//
//  util.cpp
//  Satellites
//
//  Created by Alex Simmons on 23/11/18.
//  Copyright © 2018 Alex Simmons. All rights reserved.
//

#include "util.hpp"
#include <math.h>
#include <cmath> 
#include "libsgp4/SGP4.cc"
#include "libsgp4/CoordTopocentric.cc"
#include "libsgp4/CoordGeodetic.cc"
#include <string.h>

#define earthRadiusKm 6371.0
char *strtok_new(char * string, char const * delimiter){
    static char *source = NULL;
    char *p, *riturn = 0;
    if(string != NULL)         source = string;
    if(source == NULL)         return NULL;
    
    if((p = strpbrk (source, delimiter)) != NULL) {
        *p  = 0;
        riturn = source;
        source = ++p;
    }
    return riturn;
}

static void streamToFile(const std::string &res, std::string fileName)
{
    std::ofstream o(fileName);
    o << res;
    o.close();
}

inline bool file_exists(const std::string &name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

// This function converts decimal degrees to radians
double deg2rad(double deg) {
  return (deg * M_PI / 180);
}

//  This function converts radians to decimal degrees
double rad2deg(double rad) {
  return (rad * 180 / M_PI);
}

double distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d) {
  double lat1r, lon1r, lat2r, lon2r, u, v;
  lat1r = deg2rad(lat1d);
  lon1r = deg2rad(lon1d);
  lat2r = deg2rad(lat2d);
  lon2r = deg2rad(lon2d);
  u = sin((lat2r - lat1r)/2);
  v = sin((lon2r - lon1r)/2);
  return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
}

bool getDist(SGP4 sgp4, DateTime dt, double * dist, double lat0, double long0, double radius) {
    try
    {
        Eci eci = sgp4.FindPosition(dt);
        CoordGeodetic geo = eci.ToGeodetic();
        *dist = distanceEarth(lat0, long0, rad2deg(geo.latitude), rad2deg(geo.longitude));
        if (*dist < radius)
        {
            return true;
        }
        return false;
    }
    catch (SatelliteException &e)
    {
        //std::cerr << rowid << " " << e.what() << std::endl;
        return false;
    }
    catch (DecayedException &e)
    {
        //std::cerr << rowid << " "  << e.what() << std::endl;
        return false;
    }
}

void getLineElements(vector<string>::const_iterator line, string* rowid, string* line1, string* line2, string* dt) {
    *(line); // ROWID
    *rowid = *(++line); // NORAD_CAT_ID
    *(++line); // OBJECT_TYPE
    *(++line); // INTLDES
    *dt = *(++line); // EPOCH
    *(++line); // EPOCH_MICROSECONDS
    *(++line); // TLE_LINE0
    *line1 = *(++line); // TLE_LINE1
    *line2 = *(++line); // TLE_LINE2
}

void runTLE2(vector<vector<string>>* results, string startTimeStr, string endTimeStr, double lat0, double long0, double inc, double radius) {
    string line1, line2, dt, rowid;
    struct tm tleEpoch, desiredStartEpoch, desiredEndEpoch;
    double startMins, endMins, curr, start, end, dist;
    DateTime datetime;
    vector<string>::const_iterator line;
    for (std::vector< std::vector<std::string> >::iterator i = results->begin(); i != results->end(); ++i) {
        getLineElements(i->begin(), &rowid, &line1, &line2, &dt);
        strptime(dt.c_str(), "%Y-%m-%d %H:%M:%S", &tleEpoch);
        strptime(startTimeStr.c_str(), "%Y-%m-%d %H:%M:%S", &desiredStartEpoch);
        strptime(endTimeStr.c_str(), "%Y-%m-%d %H:%M:%S", &desiredEndEpoch);
        startMins = difftime(mktime(&desiredStartEpoch),mktime(&tleEpoch))/60;
        endMins = difftime(mktime(&desiredEndEpoch),mktime(&tleEpoch))/60;
        curr = startMins;
        Tle tle("Test", line1, line2);
        SGP4 sgp4(tle);
        bool flag;
        while (curr < endMins) {
            datetime = tle.Epoch().AddMinutes(curr);
            if (getDist(sgp4, datetime, &dist, lat0, long0, radius)) {
                curr = startMins;
                bool prevWasTrue = false;
                while (curr < endMins) {
                    datetime = tle.Epoch().AddMinutes(curr);
                    flag = getDist(sgp4, datetime, &dist, lat0, long0, radius);
                    if ((flag == true) and (prevWasTrue == false)) {
                        cout << "START " << rowid << " " << datetime << " " << dist << endl;   
                        prevWasTrue = true;
                    } else if ((flag == false) and (prevWasTrue == true)) {
                        cout << "STOP  " <<  rowid << " " << datetime << " " << dist << endl;   
                        prevWasTrue = false;
                    } else if (flag == false) {
                        prevWasTrue = false;
                    }
                    curr+=1.0/60;
                }
                break;
            }
            curr+=inc;
        }
    }
}


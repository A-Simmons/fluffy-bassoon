//
//  spacetrack.cpp
//  Satellites
//
//  Created by Alex Simmons on 13/11/18.
//  Copyright © 2018 Alex Simmons. All rights reserved.
//
#include <stdio.h>
#include "spacetrack.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>
using namespace std;

struct MemoryStruct {
    char *memory;
    size_t size;
};

size_t  write_data(void *ptr, size_t size, size_t nmemb, FILE *stream, void *userdata)
{
    static int first_time=1;
    char outfilename[FILENAME_MAX] = "body.out";
    static FILE *outfile;
    size_t written;
    if (first_time) {
        first_time = 0;
        outfile = fopen(outfilename,"w");
        if (outfile == NULL) {
            return -1;
        }
        fprintf(stderr,"Streaming data to <%s>\n",outfilename);
    }
    written = fwrite(ptr,size,nmemb,outfile);
    return written;
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    
    char *ptr = static_cast<char*>(realloc(mem->memory, mem->size + realsize + 1));
    if(ptr == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

SpaceTrackConn::SpaceTrackConn(std::string username, std::string password, std::string address) {
    this->setUsername(username);
    this->setPassword(password);
    this->setAddress(address);
}

void SpaceTrackConn::setUsername(std::string username) {
    this->username = username;
}

void SpaceTrackConn::setPassword(std::string password) {
    this->password = password;
}

void SpaceTrackConn::setAddress(std::string address) {
    this->address = address;
}

string SpaceTrackConn::query(std::string queryString) {
    CURL *curl;
    CURLcode res;
    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    /* Fill in the username/identity field */
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "identity",
                 CURLFORM_COPYCONTENTS, this->username.c_str(),
                 CURLFORM_END);
    
    /* Fill in the password field */
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "password",
                 CURLFORM_COPYCONTENTS, this->password.c_str(),
                 CURLFORM_END);
    
    /* Fill in the query field */
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "query",
                 CURLFORM_COPYCONTENTS, queryString.c_str(),
                 CURLFORM_END);
    
    /* Fill in the submit field */
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "submit",
                 CURLFORM_COPYCONTENTS, "send",
                 CURLFORM_END);
    
    curl = curl_easy_init();
    
    if(curl) {
        struct MemoryStruct chunk;
        
        chunk.memory = static_cast<char*>(malloc(1));  /* will be grown as needed by the realloc above */
        chunk.size = 0;    /* no data at this point */
        curl_easy_setopt(curl, CURLOPT_URL, this->address.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS  ,1);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)this->receiveSpeed);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        res = curl_easy_perform(curl);
        printf("Stream complete. %lu bytes retrieved\n", (unsigned long)chunk.size);
        
        string s(chunk.memory);
        curl_easy_cleanup(curl);
        free(chunk.memory);
        curl_global_cleanup();
        return s;
    }
    return string();
}

int SpaceTrackConn::checkModelDefFormat(string formatClass, string field) {
    string formatClasses[] = {"tle", "tle_latest", "tle_publish", "omm", "boxscore", "satcat", "launch_site", "satcat_change", "satcat_debut", "decay", "tip", "announcement"};
    
    // Done
    string tleFields[] = {"COMMENT", "ORIGINATOR", "NORAD_CAT_ID", "OBJECT_NAME", "OBJECT_TYPE", "CLASSIFICATION_TYPE", "INTLDES", "EPOCH", "EPOCH_MICROSECONDS", "MEAN_MOTION", "ECCENTRICITY", "INCLINATION", "RA_OF_ASC_NODE", "ARG_OF_PERICENTER", "MEAN_ANOMALY", "EPHEMERIS_TYPE", "ELEMENT_SET_NO", "REV_AT_EPOCH", "BSTAR", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1", "TLE_LINE2", "OBJECT_ID", "OBJECT_NUMBER", "SEMIMAJOR_AXIS", "PERIOD", "APOGEE", "PERIGEE", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1"};
    
    string tle_latestFields[] = {"COMMENT", "ORIGINATOR", "NORAD_CAT_ID", "OBJECT_NAME", "OBJECT_TYPE", "CLASSIFICATION_TYPE", "INTLDES", "EPOCH", "EPOCH_MICROSECONDS", "MEAN_MOTION", "ECCENTRICITY", "INCLINATION", "RA_OF_ASC_NODE", "ARG_OF_PERICENTER", "MEAN_ANOMALY", "EPHEMERIS_TYPE", "ELEMENT_SET_NO", "REV_AT_EPOCH", "BSTAR", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1", "TLE_LINE2", "OBJECT_ID", "OBJECT_NUMBER", "SEMIMAJOR_AXIS", "PERIOD", "APOGEE", "PERIGEE", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1"};
    
    string tle_publishFields[] = {"COMMENT", "ORIGINATOR", "NORAD_CAT_ID", "OBJECT_NAME", "OBJECT_TYPE", "CLASSIFICATION_TYPE", "INTLDES", "EPOCH", "EPOCH_MICROSECONDS", "MEAN_MOTION", "ECCENTRICITY", "INCLINATION", "RA_OF_ASC_NODE", "ARG_OF_PERICENTER", "MEAN_ANOMALY", "EPHEMERIS_TYPE", "ELEMENT_SET_NO", "REV_AT_EPOCH", "BSTAR", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1", "TLE_LINE2", "OBJECT_ID", "OBJECT_NUMBER", "SEMIMAJOR_AXIS", "PERIOD", "APOGEE", "PERIGEE", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1"};
    
    string ommFields[] = {"COMMENT", "ORIGINATOR", "NORAD_CAT_ID", "OBJECT_NAME", "OBJECT_TYPE", "CLASSIFICATION_TYPE", "INTLDES", "EPOCH", "EPOCH_MICROSECONDS", "MEAN_MOTION", "ECCENTRICITY", "INCLINATION", "RA_OF_ASC_NODE", "ARG_OF_PERICENTER", "MEAN_ANOMALY", "EPHEMERIS_TYPE", "ELEMENT_SET_NO", "REV_AT_EPOCH", "BSTAR", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1", "TLE_LINE2", "OBJECT_ID", "OBJECT_NUMBER", "SEMIMAJOR_AXIS", "PERIOD", "APOGEE", "PERIGEE", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1"};
    
    // Done
    string boxscoreFields[] = {"COUNTRY", "SPADOC_CD", "ORBITAL_TBA", "ORBITAL_PAYLOAD_COUNT", "ORBITAL_ROCKET_BODY_COUNT", "ORBITAL_DEBRIS_COUNT", "ORBITAL_TOTAL_COUNT", "DECAYED_PAYLOAD_COUNT", "DECAYED_ROCKET_BODY_COUNT", "DECAYED_DEBRIS_COUNT", "DECAYED_TOTAL_COUNT", "COUNTRY_TOTAL"};
    
    // Done
    string satcatFields[] = {"INTLDES", "NORAD_CAT_ID", "OBJECT_TYPE", "SATNAME", "COUNTRY", "LAUNCH", "SITE", "DECAY", "PERIOD", "INCLINATION", "APOGEE", "PERIGEE", "COMMENT", "COMMENTCODE", "RCSVALUE", "RCS_SIZE", "FILE", "LAUNCH_YEAR", "LAUNCH_NUM", "LAUNCH_PIECE", "CURRENT", "OBJECT_NAME", "OBJECT_ID", "OBJECT_NUMBER"};
    
    string launch_siteFields[] = {"COMMENT", "ORIGINATOR", "NORAD_CAT_ID", "OBJECT_NAME", "OBJECT_TYPE", "CLASSIFICATION_TYPE", "INTLDES", "EPOCH", "EPOCH_MICROSECONDS", "MEAN_MOTION", "ECCENTRICITY", "INCLINATION", "RA_OF_ASC_NODE", "ARG_OF_PERICENTER", "MEAN_ANOMALY", "EPHEMERIS_TYPE", "ELEMENT_SET_NO", "REV_AT_EPOCH", "BSTAR", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1", "TLE_LINE2", "OBJECT_ID", "OBJECT_NUMBER", "SEMIMAJOR_AXIS", "PERIOD", "APOGEE", "PERIGEE", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1"};
    
    string satcat_changeFields[] = {"COMMENT", "ORIGINATOR", "NORAD_CAT_ID", "OBJECT_NAME", "OBJECT_TYPE", "CLASSIFICATION_TYPE", "INTLDES", "EPOCH", "EPOCH_MICROSECONDS", "MEAN_MOTION", "ECCENTRICITY", "INCLINATION", "RA_OF_ASC_NODE", "ARG_OF_PERICENTER", "MEAN_ANOMALY", "EPHEMERIS_TYPE", "ELEMENT_SET_NO", "REV_AT_EPOCH", "BSTAR", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1", "TLE_LINE2", "OBJECT_ID", "OBJECT_NUMBER", "SEMIMAJOR_AXIS", "PERIOD", "APOGEE", "PERIGEE", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1"};
    
    string satcat_debutFields[] = {"COMMENT", "ORIGINATOR", "NORAD_CAT_ID", "OBJECT_NAME", "OBJECT_TYPE", "CLASSIFICATION_TYPE", "INTLDES", "EPOCH", "EPOCH_MICROSECONDS", "MEAN_MOTION", "ECCENTRICITY", "INCLINATION", "RA_OF_ASC_NODE", "ARG_OF_PERICENTER", "MEAN_ANOMALY", "EPHEMERIS_TYPE", "ELEMENT_SET_NO", "REV_AT_EPOCH", "BSTAR", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1", "TLE_LINE2", "OBJECT_ID", "OBJECT_NUMBER", "SEMIMAJOR_AXIS", "PERIOD", "APOGEE", "PERIGEE", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1"};
    
    string decayFields[] = {"COMMENT", "ORIGINATOR", "NORAD_CAT_ID", "OBJECT_NAME", "OBJECT_TYPE", "CLASSIFICATION_TYPE", "INTLDES", "EPOCH", "EPOCH_MICROSECONDS", "MEAN_MOTION", "ECCENTRICITY", "INCLINATION", "RA_OF_ASC_NODE", "ARG_OF_PERICENTER", "MEAN_ANOMALY", "EPHEMERIS_TYPE", "ELEMENT_SET_NO", "REV_AT_EPOCH", "BSTAR", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1", "TLE_LINE2", "OBJECT_ID", "OBJECT_NUMBER", "SEMIMAJOR_AXIS", "PERIOD", "APOGEE", "PERIGEE", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1"};
    
    string tipFields[] = {"COMMENT", "ORIGINATOR", "NORAD_CAT_ID", "OBJECT_NAME", "OBJECT_TYPE", "CLASSIFICATION_TYPE", "INTLDES", "EPOCH", "EPOCH_MICROSECONDS", "MEAN_MOTION", "ECCENTRICITY", "INCLINATION", "RA_OF_ASC_NODE", "ARG_OF_PERICENTER", "MEAN_ANOMALY", "EPHEMERIS_TYPE", "ELEMENT_SET_NO", "REV_AT_EPOCH", "BSTAR", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1", "TLE_LINE2", "OBJECT_ID", "OBJECT_NUMBER", "SEMIMAJOR_AXIS", "PERIOD", "APOGEE", "PERIGEE", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1"};
    
    string announcementFields[] = {"COMMENT", "ORIGINATOR", "NORAD_CAT_ID", "OBJECT_NAME", "OBJECT_TYPE", "CLASSIFICATION_TYPE", "INTLDES", "EPOCH", "EPOCH_MICROSECONDS", "MEAN_MOTION", "ECCENTRICITY", "INCLINATION", "RA_OF_ASC_NODE", "ARG_OF_PERICENTER", "MEAN_ANOMALY", "EPHEMERIS_TYPE", "ELEMENT_SET_NO", "REV_AT_EPOCH", "BSTAR", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1", "TLE_LINE2", "OBJECT_ID", "OBJECT_NUMBER", "SEMIMAJOR_AXIS", "PERIOD", "APOGEE", "PERIGEE", "MEAN_MOTION_DOT", "MEAN_MOTION_DDOT", "FILE", "TLE_LINE0", "TLE_LINE1"};
    
    bool formatfound = (std::find(std::begin(formatClasses), std::end(formatClasses), formatClass) != std::end(formatClasses)); // Check if formatClass is in known formats
    if (formatfound) {
        bool fieldfound = 0;
        if (formatClass == "tle") {
            fieldfound = (std::find(std::begin(tleFields), std::end(tleFields), field) != std::end(tleFields)); // Check if formatClass is in known formats
        } else if (formatClass == "tle_latest") {
            fieldfound = (std::find(std::begin(tle_latestFields), std::end(tle_latestFields), field) != std::end(tle_latestFields)); // Check if formatClass is in known formats
        } else if (formatClass == "boxscore") {
            fieldfound = (std::find(std::begin(boxscoreFields), std::end(boxscoreFields), field) != std::end(boxscoreFields)); // Check if formatClass is in known formats
        } else if (formatClass == "satcat") {
            fieldfound = (std::find(std::begin(satcatFields), std::end(satcatFields), field) != std::end(satcatFields)); // Check if formatClass is in known formats
        }
        if (fieldfound)
            return 0;
        else
            return 2;
    } else
        return 1;
}

bool SpaceTrackConn::checkFields(SpaceTrackQuery* q) {
    int result = 0;
    // Iterate over vector, pulling out pointer to tuple as it
    for (std::vector< std::tuple<std::string, std::string>>::iterator it = q->args.begin(); it != q->args.end(); ++it) {
        // Field name is 0th index in tuple, check it conforms with known fields in format
        int r = this->checkModelDefFormat(q->formatClass, std::get<0>(*it));
        if (r) {
            result = 1;
            cout << std::get<0>(*it) << ": " << std::get<1>(*it) << "\n";
        }
    }
    return result;
}

string SpaceTrackConn::getData(SpaceTrackQuery* q) {
    if (this->checkFields(q)) {
        cout << "Something bad happened\n";
        exit (EXIT_FAILURE);
    }
    std::string s = "https://www.space-track.org/basicspacedata/query/class/";
    s.append(q->formatClass);
    s.append("/");
    for (std::vector< std::tuple<std::string, std::string>>::iterator it = q->args.begin(); it != q->args.end(); ++it) {
        s.append(get<0>(*it));
        s.append("/");
        s.append(get<1>(*it));
        s.append("/");
    }
    s.append("format/");
    s.append(q->format);
    cout << s << "\n";
    return this->query(s);
}

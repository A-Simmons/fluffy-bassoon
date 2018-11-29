//
//  spacetrack.hpp
//  Satellites
//
//  Created by Alex Simmons on 13/11/18.
//  Copyright © 2018 Alex Simmons. All rights reserved.
//

#ifndef spacetrack_hpp
#define spacetrack_hpp

#include <stdio.h>
#include <string>
#include </usr/local/curl/include/curl/curl.h>
#include <curl/easy.h>
#include <tuple>
#include <vector>
#include <list>
#include "spacetrackquery.hpp"

using namespace std;

class SpaceTrackConn {
public:
    std::string username = "";
    std::string password = "";
    std::string address = "https://www.space-track.org/basicspacedata/query/class/boxscore";
    
    int receiveSpeed = 5000*1024;
    
    
    SpaceTrackConn(string, string, string);
    void setUsername(string);
    void setPassword(string);
    void setAddress(string);
    string getData(SpaceTrackQuery*);
private:
    string query(std::string);
    int checkModelDefFormat(string, string);
    bool checkFields(SpaceTrackQuery*);
};

#endif /* spacetrack_hpp */

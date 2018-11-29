//
//  spacetrackquery.hpp
//  Satellites
//
//  Created by Alex Simmons on 13/11/18.
//  Copyright © 2018 Alex Simmons. All rights reserved.
//

#ifndef spacetrackquery_hpp
#define spacetrackquery_hpp

#include <stdio.h>
#include <string>
#include <tuple>
#include <vector>

using namespace std;
class SpaceTrackQuery {
public:
    string formatClass;
    string format = "json";
    string filename = "body.out";
    vector< tuple<string, string>> args;
    SpaceTrackQuery(void);
    void append(string, string);
    void setClass(string);
    void setFormat(string);
    void setFileName(string);
private:
};
#endif /* spacetrackquery_hpp */

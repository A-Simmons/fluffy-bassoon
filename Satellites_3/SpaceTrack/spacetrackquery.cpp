//
//  spacetrackquery.cpp
//  Satellites
//
//  Created by Alex Simmons on 13/11/18.
//  Copyright © 2018 Alex Simmons. All rights reserved.
//

#include "spacetrackquery.hpp"

SpaceTrackQuery::SpaceTrackQuery() {
    
}

void SpaceTrackQuery::append(string field, string value) {
    this->args.push_back( make_tuple(field, value));
}

void SpaceTrackQuery::setClass(string formatClass) {
    this->formatClass = formatClass;
}

void SpaceTrackQuery::setFormat(string format) {
    this->format = format;
}

void SpaceTrackQuery::setFileName(string filename) {
    this->filename = filename;
}

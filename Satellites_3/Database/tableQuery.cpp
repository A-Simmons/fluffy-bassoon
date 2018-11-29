//
//  tableQuery.cpp
//  Satellites
//
//  Created by Alex Simmons on 13/11/18.
//  Copyright © 2018 Alex Simmons. All rights reserved.
//

#include "tableQuery.hpp"
TableQuery::TableQuery() {
    
}

void TableQuery::append(string field, string value) {
    this->args.push_back( make_tuple(field, value));
}


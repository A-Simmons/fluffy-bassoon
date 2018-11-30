//
//  tableQuery.hpp
//  Satellites
//
//  Created by Alex Simmons on 13/11/18.
//  Copyright © 2018 Alex Simmons. All rights reserved.
//

#ifndef tableQuery_hpp
#define tableQuery_hpp

#include <tuple>
#include <vector>
#include <stdio.h>
#include <string>

using namespace std;
class TableQuery {
public:
    vector< tuple<string, string>> args;
    TableQuery(void);
    void append(string, string);
    void prepareTable();
};
#endif /* tableQuery_hpp */

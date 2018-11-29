//
//  util.cpp
//  Satellites
//
//  Created by Alex Simmons on 23/11/18.
//  Copyright © 2018 Alex Simmons. All rights reserved.
//

#include "util.hpp"

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

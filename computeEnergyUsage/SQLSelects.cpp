//
//  SQLSelects.cpp
//  computeEnergyUsage
//
//  Created by Clifford Campo on 8/1/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
#include <iostream>
#include <string>
#include <string.h>
#include "SQLSelects.hpp"
using namespace std;
SQLSelects::SQLSelects(void) {
    cout << "Just entered class SQLSelects constructor" << endl;
}
SQLSelects::~SQLSelects(void) {
    cout << "Just entered class SQLSelects destructor" << endl;;
}
const char *SQLSelects::returnSQLSelect(int whichSQLSelect) {
    switch (whichSQLSelect ) {
        case FALLWINTERSPRING:
            return (ptrFallWinterAndSpringSeasons);
        case SUMMERONLYSEASON:
            return (ptrSummerOnlySeason);
        default:
            return (nullptr);
    }
}

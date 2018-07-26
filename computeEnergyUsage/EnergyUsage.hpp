//
//  EnergyUsage.hpp
//  computeEnergyUsage
//
//  Created by Clifford Campo on 7/18/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef EnergyUsage_hpp
#define EnergyUsage_hpp
#include <iostream>
#include <sstream>
#include <libpq-fe.h>
#include <regex>
#include <string>
#include "StopWatch.hpp"
#include "DateTimeFields.hpp"
#define NUMBEROFPARAMETERSFORUPDATE 5
#define FLOAT4ARRAYOID 1021
#define FLOAT8ARRAYOID 1022
#define FLOAT4OID 700
#define FLOAT8OID 701
#define TEXTOID 25
#define INT2OID 21
#define DATETIMEBUFFERSIZE 16
#define NUMBEROFPARAMETERS 6
#define DOLLAR1 1-1 // convert sql_prototype replacement place-holder, $1, to param_values array index number
#define DOLLAR2 2-1 // convert sql_prototype replacement place-holder, $2, to param_values array index number
#define DOLLAR3 3-1 // convert sql_prototype replacement place-holder, $3, to param_values array index number
#define DOLLAR4 4-1 // convert sql_prototype replacement place-holder, $4, to param_values array index number
#define DOLLAR5 5-1 // convert sql_prototype replacement place-holder, $5, to param_values array index number
#define DOLLAR6 6-1 // convert sql_prototype replacement place-holder, $6, to param_values array index number
#define SIZEOFSQLBUFFER 200
#define BUFSIZE 32
//Used for indexing into arrays named primaryConnectionString and secondaryConnectionString
#define DATABASENAMEINDEX  0
#define PORTINDEX 1
#define USERINDEX 2
#define HOSTINDEX 3
using namespace std;
class EnergyUsage : public DateTimeFields {
private:
    const char *DatabaseName="LocalWeather";
    const char *UserID[2]={"cjc", "cjc"};
    const char *Port[2] = {"5436", "5435"};
    const char *Host[2] = {"localhost", "localhost"};
    const char *defaultInputFile="/Users/cjc/site7_energyUsage.csv";
    const char *keywords[5] = {"dbname", "port", "user", "host", nullptr};   // The field names associated with the primary and secondary connection strings.
public:
    char *primaryConnectionString[5];  //Dynamically Allocated. Must be destructed.
    char *secondaryConnectionString[5]; //Dynamically Allocated. Must be destructed.
    const char *connectString1[5] = {"LocalWeather", "5436", "cjc", "localhost", nullptr};  //Default connection string for the primary connectio. The primary connection is about the sites close to my home [i.e., Paoli, PA] site.
    const char *connectString2[5] = {"LocalWeather", "5435", "cjc", "localhost", nullptr};  //Default connection string for the second connection. The second connection is about those sites that are not the home [i.e., Paoli] site.
    
    PGconn *conn1; // For connecting to, for now, port 5436 which is used for gatering weather data at my home (Paoli) site. Can change in the future.
    PGresult *rslt1; //For gathering results when connected using conn1, above.
    PGconn *conn2; // For connecting to any other host (local host or foreign) whose siteid is not local to my home site.
    PGresult *rslt2; //For gathering results when using conn2, above, connections.
    const char *ptrID;
    const char *ptrStartDate_Time;
    const char *ptrEndDate_Time;
    const char *ptrSiteID;
    const char *ptrToInputFile;
    const char *ptrDefaultInputFile;
    const char *ptrToSQL;
    short rc;
    short shortSiteId;
    short NBOsiteID;
    short loopCounter;
    StopWatch sw1;  //Represents Construction of the class name StopWatch.
    StopWatch sw2;  //Represents Construction of the class name StopWatch.
    unsigned int lsw1;
    unsigned int lsw2;
    //    double endDateJD;
    //    double NBOendDateJD;
    const char *param_values[NUMBEROFPARAMETERS];
    const char **kw;
    char **cs1;
    char **cs2;
    union {
        const char * const *_paramValues[NUMBEROFPARAMETERS];
        char *paramValues[NUMBEROFPARAMETERS];
    } upv;
    char *ArrayOfPtrTpParamsInNbo[NUMBEROFPARAMETERS];
    const char *defaultDatabaseName;
    const char *defaultUserID;
    const char *defaultPort;
    const char *defaultHost;
    const char *startDateTime;
    const char *endDateTime;
    const char *dateTime_File;
    //   DateTimeFields dTB;  Became this class's base class DateTimeFields
    const char *justDate;
    const char *justTime;
    double coeff[3][4]; //contains the four polynomial coefficients for each of the 3 meters: m1m2kwh, m1kwh, m2kwh, respectively.
    
    int endID;  //row id of the end date&time record.
    int NBOendID;
    int i;
    int j;
    //------------------
    float computedEnergyUsageM1M2;
    float NBOcomputedEnergyUsageM1M2;
    std::stringstream ssM1M2;
    //------------------
    float computedEnergyUsageM1;
    float NBOcomputedEnergyUsageM1;
    std::stringstream ssM1;
    //------------------
    float computedEnergyUsageM2;
    float NBOcomputedEnergyUsageM2;
    std::stringstream ssM2;
    //------------------
    double sumOfWeightedTemperatures;
    double sumOfWeights;
    union {
        double weightedSum;
        double averageTemperature;
    };
    string line;
    smatch mr;   //Used for regular expressions.
    bool debug1;
    bool debug2;
    bool debug3;
    bool debug4;
    bool thread;
    bool reading_date_time_file;
    bool result;
    const char *clSQL;
    bool useKelvin;
    const char  *siteid;
    char *work;
    const char *paramValues[NUMBEROFPARAMETERS];
    char bufM1M2[BUFSIZE];
    char bufM1[BUFSIZE];
    char bufM2[BUFSIZE];
    std::string ID;
    std::string SDT;
    std::string EDT;
    std::string SID;
    char bufferUsedForConstructingAnSQLStatement[SIZEOFSQLBUFFER];
    EnergyUsage(void);
    ~EnergyUsage(void);
    void clearAllBuffers(void);
};

#endif /* EnergyUsage_hpp */

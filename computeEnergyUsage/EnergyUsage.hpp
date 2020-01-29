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
#include <strstream>
#include "StopWatch.hpp"
#include "DateTimeFields.hpp"
#include "SQLSelects.hpp"
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
#define ALLBITSHIGH 0b11111111
#define CONNECTIONTOHOMESITEISOPEN 0b00000001
#define CONNECTIONTOREMOTESITEISOPEN 0b00000010
#define CONN1AND2OPEN 0b00000011
#define CONN1AND2CLOSED 0
#define RSLTHOMESITEACTIVE 0b00000001
#define RSLTREMOTESITEACTIVE 0b00000010
#define RSLT1AND2ACTIVE 0b00000011
#define RSLT1AND2INACTIVE 0
#define SECONDARYSQLCOMMANDFAILURE 1
#define UPDATESQLFAILURE 2
#define PRIMARYSQLCOMMANDFAILURE 3
#define INSERTINTOFAILURE 4
#define PRIMARYCONNECTFAILURE 5
#define SECONDARYCONNECTFAILURE 6
using namespace std;
class EnergyUsage : public DateTimeFields, public SQLSelects {
private:
    const char *DatabaseName="LocalWeather";
    const char *UserID[2]={"cjc", "cjc"};
    const char *Port[2] = {"5436", "5445"}; //2020-01-28T17:46:36 changed "5435" to "5445" bc port 5435 was replaced with port 5445
    const char *Host[2] = {"localhost", "localhost"};
    const char *defaultInputFile="/Users/cjc/site7_energyUsage.csv";
    const char *keywords[5] = {"dbname", "port", "user", "host", nullptr};   // The field names associated with the primary and secondary connection strings.
public:
    char *primaryConnectionString[5];  //Dynamically Allocated. Must be destructed.
    char *secondaryConnectionString[5]; //Dynamically Allocated. Must be destructed.
    const char *connectString1[5] = {"LocalWeather", "5436", "cjc", "localhost", nullptr};  //Default connection string for the primary connection. The primary connection is about the sites close to my home [i.e., Paoli, PA] site.
    const char *connectString2[5] = {"LocalWeather", "5445", "cjc", "localhost", nullptr};  //Default connection string for the second connection. The second connection is about those sites that are not the home [i.e., Paoli] site.
    char  workDOY[12];
    int years;
    short months;
    short days;
    short doy;
    bool leapYear;
    bool summerSeason;
    PGconn *connectionToHomeSite; // For connecting to, for now, port 5436 which is used for gatering weather data at my home (Paoli) site. Can change in the future.
    PGresult *rslt1; //For gathering results when connected using connectionToHomeSite, above.
    PGconn *remoteSiteConnection; // For connecting to any other host (local host or foreign) whose siteid is not local to my home site.
    PGresult *rsltRemoteSite; //For gathering results when using remoteSiteConnection, above, connections.
    const char *ptrID;


    const char *ptrSiteID;
    const char *ptrToInputFile;
    const char *ptrDefaultInputFile;
    const char *ptrToSQL;
    short rc;
    short shortSiteId;
    short NBOsiteID;
    unsigned short extractedMonth;
    unsigned short extractedDay;
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
    int  adjustM2ForTesla; //Added 2020-01-28T17:55:12 – contains the portion of energy, in kWh, used to charge my new Tesla Model 3\
    on this particular day. Default value is zero, as set by this class' constructor (::EnergyUsage). Tesla Model 3 was acquired on\
    2019-09-20.
    char *ArrayOfPtrTpParamsInNbo[NUMBEROFPARAMETERS];
    const char *defaultDatabaseName;
    const char *defaultUserID;
    const char *defaultPort;
    const char *defaultHost;
    char justStartDate[16];
    char justStartTime[16];
    char justEndDate[16];
    char justEndTime[16];
    union {
       const char *startDateTime;
       const char *ptrStartDate_Time;
    } usdt;
    union {
        const char *endDateTime;
        const char *ptrEndDate_Time;
    } uedt;
    const char *dateTime_File;
    //   DateTimeFields dTB;  Became this class's base class DateTimeFields
    const char *justDate;
    const char *justTime;
    double coeff[3][4]; //contains the four polynomial coefficients for each of the 3 meters: m1m2kwh, m1kwh, m2kwh, respectively.
    double meter2;
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
    u_int8_t whichConnection;
    u_int8_t whichActiveRslt;
    string line;
    smatch mr;   //Used for regular expressions.
    bool debug1;
    bool debug2;
    bool debug3;
    bool debug4;
    bool thread;
    bool reading_date_time_file;
    bool result;
    bool resultDay;
    bool resultMonth;
    bool resultYear;
    bool seasonalBasedApproach;
    const char *clSQL;
    bool useKelvin;
    const char  *siteid;
    char *work;
    const char *paramValues[NUMBEROFPARAMETERS];
    char bufM1M2[BUFSIZE];
    char bufM1[BUFSIZE];
    char bufM2[BUFSIZE];
    char *ptrMeter2;
    char holdsTime[11]="'HH:MM:SS'"; //10+1, allowing for a NULL at the end of the string.
    char holdsDate[13]="'YYYY-MM-DD'"; //12+1, allowing for a NULL at the end of the string.
    char *ptrHoldsTime;
    char *ptrHoldsDate;
    std::string ID;
    std::string SDT;
    std::string EDT;
    std::string SID;
    std::stringstream ssInsertInto;
    char bufferUsedForConstructingAnSQLStatement[SIZEOFSQLBUFFER];
    EnergyUsage(void);  
    ~EnergyUsage(void);
    void clearAllBuffers(void);
    void closeConnections(u_int8_t);
    void clearActiveRslt(u_int8_t);
    short doTheWork(bool=true);
};

#endif /* EnergyUsage_hpp */

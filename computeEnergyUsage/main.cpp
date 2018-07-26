//
//  main.cpp
//  computeEnergyUsage
//
//  Created by Clifford Campo on 7/18/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//


#include <AvailabilityMacros.h>
#include <iostream>
#include <fstream> //Needed for file io.
#include <thread>
#define TEXTOID  25
//#include "/usr/local/pgsql95/pgsql090513/include/libpq-fe.h" //change 2018-07-10T14:23:26 from 090224
#include <libpq-fe.h>
//#include <server/catalog/pg_type.h> //Used to define paramTypes's TEXTOID
#include <cstring>
#include <sstream>
#include <string>
#include <regex>
#include <iomanip>
#include <getopt.h>
#include <chrono> //For high resolution timing.
#include <stdio.h>
#include <string.h>
#include "EnergyUsage.hpp"
#include "DateTimeFields.hpp"
#include "makeHelpMessage.hpp"
#include "varyingType.hpp"
#include "StopWatch.hpp"
#define SECONDARYSQLCOMMANDFAILURE 1
#define UPDATESQLFAILURE 2
#define PRIMARYSQLCOMMANDFAILURE 3
#define INSERTINTOFAILURE 4
#define PRIMARYCONNECTFAILURE 5
#define SECONDARYCONNECTFAILURE 6
extern const char *helpMessage[];
extern const char *BFM;
//===========================================================================================
//
const char *bestKind[] = {"SELECT coeff[1], coeff[2], coeff[3], coeff[4]  FROM tbl_poly_fit_with_cov WHERE lt::DATE IN (SELECT lt::DATE from tbl_poly_fit_with_cov where abs(correl)::NUMERIC = ( SELECT max(abs(correl)::NUMERIC ) FROM tbl_poly_fit_with_cov where kind=0)) AND kind=0", \
    "SELECT coeff[1], coeff[2], coeff[3], coeff[4]  FROM tbl_poly_fit_with_cov WHERE lt::DATE IN (SELECT lt::DATE from tbl_poly_fit_with_cov where abs(correl)::NUMERIC = ( SELECT max(abs(correl)::NUMERIC ) FROM tbl_poly_fit_with_cov where kind=0)) AND kind=1", \
    "SELECT coeff[1], coeff[2], coeff[3], coeff[4]  FROM tbl_poly_fit_with_cov WHERE lt::DATE IN (SELECT lt::DATE from tbl_poly_fit_with_cov where abs(correl)::NUMERIC = ( SELECT max(abs(correl)::NUMERIC ) FROM tbl_poly_fit_with_cov where kind=0)) AND kind=2"};
Oid bestKindTypes[] = {INT2OID};
int paramLengthsBestKind[] = {2};
int paramFormatsBestKind[] = {1};
int paramValuesBestKind[1];  // Kind is either 0, 1, or 2 in NetworkByteOrder.
//===========================================================================================
//
const char *sql_INSERT_INTO_prototype = "INSERT INTO tbl_modeled_energy_usage (date, time, temperature, siteid, mm1kwh, mm2kwn, mm1m2kwh) VALUES($1, $2, $3, $4,$5, $6, $7);";
Oid insertIntoTypes[] = {TEXTOID, TEXTOID, FLOAT4OID, INT2OID, FLOAT4OID,FLOAT4OID, FLOAT4OID};
int paramLengthsForInsert[] = {0,0,4,2,4,4,4};
int paramFormatsForInsert[] = {0,0,1,1,1,1,1};
char *paramValuesForInsert[7];  //Values are in network byte order, NBO, which implies BigEndian Format (high-ordered bytes are the left-most bytes).
//===========================================================================================
int four = 4;
int two = 2;
int zero = 0;
int myOids[NUMBEROFPARAMETERSFORUPDATE+1] =  {FLOAT4OID, FLOAT4OID, FLOAT4OID,  INT2OID, TEXTOID, TEXTOID};
const char *sql_UPDATE_prototype = "UPDATE tbl_energy_usage set mm1kwh=$1, mm2kwh=$2, mm1m2kwh=$3 where siteid=$4 AND date::TEXT=$5 AND time::TEXT=$6;";
Oid updateTypes[NUMBEROFPARAMETERSFORUPDATE+1] = {FLOAT4OID, FLOAT4OID,FLOAT4OID, FLOAT4OID, FLOAT4OID, FLOAT4OID};
int paramLengths[NUMBEROFPARAMETERSFORUPDATE+1]= {4,4,4,2,0,0};
int paramFormats[NUMBEROFPARAMETERSFORUPDATE+1]= {1,1,1,1,0,0};
char *paramValues[NUMBEROFPARAMETERSFORUPDATE+1];
//===========================================================================================
//                                              0                                              2                                      3                                                                       4         5
const char *sql_prototype = "SELECT sum((a.date_part - b.date_part)*b.temperature), sum(a.date_part - b.date_part),  (sum((a.date_part - b.date_part)*b.temperature))/(sum(a.date_part - b.date_part))  \
FROM \
(SELECT   lt, temperature, date_part('J', lt) FROM tbl_l where (lt BETWEEN $1 AND $2 ) AND siteid::NUMERIC=$3 ORDER BY lt) a \
INNER JOIN \
(SELECT   lt, temperature, date_part('J', lt) FROM tbl_l where (lt BETWEEN $4 AND $5 ) AND siteid::NUMERIC=$6 ORDER BY lt) b \
ON date_part('J', a.lt::DATE) = 1+date_part('J', b.lt::DATE);";
/*
 SELECT sum((a.date_part - b.date_part)*b.temperature), sum(a.date_part - b.date_part),  (sum((a.date_part - b.date_part)*b.temperature))/(sum(a.date_part - b.date_part))  AS "WeightedAverageTemperature", avg(a.temperature) AS "AverageAtemperature", avg(b.temperature) AS "AverageBtemperature"
 FROM
 (SELECT   lt, temperature, date_part('J', lt) FROM tbl_l where (lt BETWEEN '2017-11-03 06:35:00' AND '2017-11-04 06:35:00' ) AND siteid::NUMERIC=12 ORDER BY lt) a
 INNER JOIN
 (SELECT   lt, temperature, date_part('J', lt) FROM tbl_l where (lt BETWEEN '2017-11-03 06:35:00' AND '2017-11-04 06:35:00' ) AND siteid::NUMERIC=12 ORDER BY lt) b
 ON date_part('J', a.lt::DATE) = 1+date_part('J', b.lt::DATE);
 */
//Oid paramTypes[] = {TEXTOID, TEXTOID, TEXTOID, TEXTOID, TEXTOID, TEXTOID};
Oid paramTypes[] = {TEXTOID, TEXTOID};  //Just the 2 siteid`s
//===========================================================================================
const char *moass[] = {"SELECT DISTINCT coeff[1], coeff[2], coeff[3], coeff[4] FROM tbl_poly_fit_with_cov WHERE kind=0 AND abs(correl) = (SELECT max(abs(correl)) FROM tbl_poly_fit_with_cov where kind=0)", \
    "SELECT DISTINCT coeff[1], coeff[2], coeff[3], coeff[4] FROM tbl_poly_fit_with_cov WHERE kind=1 AND abs(correl) = (SELECT max(abs(correl)) FROM tbl_poly_fit_with_cov where kind=1)", \
    "SELECT DISTINCT coeff[1], coeff[2], coeff[3], coeff[4] FROM tbl_poly_fit_with_cov WHERE kind=2 AND abs(correl) = (SELECT max(abs(correl)) FROM tbl_poly_fit_with_cov where kind=2)"};

const char *simpleMoass = "SELECT DISTINCT coeff[1], coeff[2], coeff[3], coeff[4] FROM tbl_poly_fit_with_cov) WHERE kind=$1 AND abs(correl) = (SELECT max(abs(correl)) FROM tbl_poly_fit_with_cov where kind=$2 AND NOT (coeff[1] = 'NaN' OR coeff[1] = 'NaN' OR coeff[3] = 'NaN'  OR coeff[4] = 'NaN' OR correl = 'NaN') )";
Oid paramTypes1[] = {TEXTOID,TEXTOID, TEXTOID,TEXTOID, TEXTOID,TEXTOID};
// char *param_values[2]={0,0};
//static const char* const static_ArrayOfConnectKeyWords[] = {"dbname", "port", "user", "password", "host" };
//static const char* const static_ArrayOfValues[] = {"LocalWeather", "5436", "cjc", NULL, "localhost"};
extern const char *helpMessageArray[];

short doTheWork(EnergyUsage &, bool=true);
void setUpSiteID(EnergyUsage &, const char *);
void setUpStartDateTime(EnergyUsage &, const char *);
void setUpEndDateTime(EnergyUsage &,const char *);
void setUpSiteID(EnergyUsage &eu, const char *optarg) {
    eu.siteid = optarg;
    eu.shortSiteId = atoi(optarg);
    eu.param_values[DOLLAR3] = optarg; //does $3
    eu.param_values[DOLLAR6] = optarg; //does $6
}
void setUpEndDateTime(EnergyUsage &eu, const char *optarg) {
    eu.endDateTime = optarg;
    eu.param_values[DOLLAR2] = optarg; //does $2
    eu.param_values[DOLLAR5] = optarg; //does $5
    if(eu.debug2) std::cout << "EndDateTime (reporting value found in command line associated with the `-e` switch)[DOLLAR2]: " <<  eu.param_values[DOLLAR2] << " and [DOLLAR5]: " << eu.param_values[DOLLAR5] << std::endl;
}
void setUpStartDateTime(EnergyUsage &eu, const char *optarg) {
    eu.startDateTime = optarg;
    eu.param_values[DOLLAR1] = optarg; //does $1
    eu.param_values[DOLLAR4] = optarg; //does $4
    if(eu.debug2) std::cout << "StartDateTime (reporting value found in command line associated with the `-s` switch)[DOLLAR1]: " <<  eu.param_values[DOLLAR1] << " and [DOLLAR4]: " << eu.param_values[DOLLAR4] << std::endl;
}
int main(int argc,  char *const argv[]) { ///<======= M A I N    E N T R Y     P O I N T           M A I N    E N T R Y     P O I N T           M A I N    E N T R Y     P O I N T
    EnergyUsage eu;
    std::ifstream myInFile; //Declare myInFile as the object for processing the input file, if any.

    using namespace std;
    static const char  *static_OtherArrayOfValues[] = {eu.defaultDatabaseName, eu.defaultPort, eu.defaultUserID, NULL, eu.defaultHost};
    const char **otherArrayOfValues;
    otherArrayOfValues = static_OtherArrayOfValues;
    std::cout << "otherArrayOfValues: " << std::hex << otherArrayOfValues << std::endl;
    if(eu.debug1) {
        for (int i=0; i<5 ; i++) {
            if (*(otherArrayOfValues + i)) {
                std::cout << i << ". *(otherArrayOfValues + i): " << std::hex << (otherArrayOfValues + i) << "\t" << *(otherArrayOfValues + i) << std::endl;
            } else {
                std::cout << i << ". *(otherArrayOfValues + i): " << std::hex << (otherArrayOfValues + i) << "\t NULL" << std::endl;
            }
        }
    }
    const struct option longopts[] = {
        {"help",   no_argument, 0, 'h'},
        {"connectString1", required_argument, 0, 'c'},
        {"connectString2", required_argument, 0, 'C'},
        {"startDateTime", required_argument, 0, 's'},
        {"endDateTime", required_argument, 0, 'e'},
        {"sql",       required_argument, 0, 'S'},
        {"kelvin", no_argument, 0, 'k'},
        {"port1", required_argument, 0, 'p'},
        {"port2", required_argument, 0, 'P'},
        {"host1", required_argument, 0, 'o'},
        {"host2", required_argument, 0, 'O'},
        {"dateTimeFile", required_argument, 0, 'f'},
        {"siteID", required_argument, 0, 'i'},
        {"thread", no_argument, 0, 't'},
        {"debug1", no_argument, 0, '1'},
        {"debug2", no_argument, 0, '2'},
        {"debug3", no_argument, 0, '3'},
        {"debug4", no_argument, 0, '4'},
        {0,0,0,0}, //End of array
    };
    // insert code here...

    std::cout << "Hello, " << *(argv + 0) << std::endl;
    
    const char *const commandLineSwitches = "1234hktp:P:o:O:S:f:i:s:e:c:C:";
    int index;
//    int numberOfMessageSegments = sizeof(helpMessage)/sizeof(eu.defaultUserID);
    
//    while ( *(helpMessageArray + numberOfMessageSegments)  )  {numberOfMessageSegments++;} //Loop until we hit the pointer that points to location 0x'00 00 00 00 00 00 00 00', marking the end of the array.
//    numberOfMessageSegments--; //Back-off 1 becaue the very last entry in the array, that gets counted, is an all zero terminator, but we want our count to indicate the number of valid addresses that point to strings.
    
    char  iarg= NULL; //Jumping though hoops just to make iarg to appear as a character in the debugger.
    
    opterr=1;
    
    while( (iarg = (char)getopt_long(argc, argv, commandLineSwitches, longopts, &index)) != -1)  {  //Jumping though hoops just to make iarg to appear as a character in the debugger.
        if (iarg < 0) break;
        switch (iarg)
        {
            case 'c': //First Connect string which is used to acquired the data in LocalWeather database's table tbl_poly_fit_with_cov
                eu.cs1 = (char **)optarg;
                std::cout << "Non-default Connect String 1, used for accessing each of the 3 energy usage models: " << *eu.cs1 << ", or: " << optarg << std::endl;
                break;
            case 'C':
                eu.cs2 = (char **)optarg;
                std::cout << "Non-default Connect String 2, used for computing, using polynomial energy usage model at a non-local site: " << *eu.cs2 << ", or: " << optarg << std::endl;
                break;
            case 'f': //date&TimeFile
                std::cout << "dateTime File: " << optarg << std::endl;
                eu.dateTime_File = optarg;
                eu.ptrToInputFile = optarg;
                break;
            case 'k': //Use kelvin temperatures as independent variable
                eu.useKelvin = true;
                break;
            case 'U': //Userid
                //              ptrCLA->_clUserID = optarg;
                static_OtherArrayOfValues[2] = optarg;
                std::cout << "Non-default User ID: " << static_OtherArrayOfValues[2] << std::endl;
                break;
            case 'O': //Hostid
                //                ptrCLA->_clHostID = optarg;
                static_OtherArrayOfValues[4] = optarg;
                std::cout << "Non-default HostID: " <<    static_OtherArrayOfValues[4] << std::endl;
                eu.connectString2[3] = optarg;
                eu.secondaryConnectionString[HOSTINDEX] = optarg; //Either localhost, or 127.0.0.1, or some other `dotted decimal` host id number.
                break;
            case 'p':   //Port of the primary connection string
                //               ptrCLA->_clPortID = optarg;
                static_OtherArrayOfValues[1] = optarg;
                eu.primaryConnectionString[PORTINDEX] = optarg;
                std::cout << "Non-default PortID: " << static_OtherArrayOfValues[1]  << std::endl;
                break;
            case 'P':   //Port of the secondary connection string
                //               ptrCLA->_clPortID = optarg;
                static_OtherArrayOfValues[1] = optarg;
                eu.secondaryConnectionString[PORTINDEX] = optarg;
                std::cout << "Non-default PortID: " <<  eu.secondaryConnectionString[PORTINDEX] << std::endl;
                break;
            case 'S':  //SQL
                eu.clSQL = optarg;
                std::cout << "SELECT SQL STATEMENT looks like: " << eu.clSQL << std::endl;
                break;
            case '1':  //Debug stage 1
                eu.debug1=true;
                break;
            case '2': //Debug stage 2
                eu.debug2=true;
                break;
            case '3': //Debug stage 3
                eu.debug3=true;
                break;
            case '4': //Turn on and off the recording of ostringstream data.
                eu.debug4=true;
                break;
            case 't': //If set then doing multithread processing
                eu.thread=true;
                std::cout << BFM <<  "\nRunning in multi-thread mode\n" << BFM << std::endl;
                break;
            case 's':
                setUpStartDateTime(eu, optarg);
                break;
            case 'e':
                setUpEndDateTime(eu, optarg);
                break;
            case 'i':
                setUpSiteID(eu, optarg);
                
                std::cout << "Using siteID [DOLLAR3]: " << eu.param_values[DOLLAR3] << " and [DOLLAR6]" << eu.param_values[DOLLAR6] << std::endl;
                break;
            default:
                std::cerr << "Unknown command line parameter: '" << iarg << "', Skipping." << std::endl;
                break;
            case 'h': //Help
                MakeHelpMessage mhm(*(argv +0) );
                cout << mhm.createMessage(*(argv + 0) ) << endl;
                exit (0);
        } //End of switch
    }  //End of While
    
    

    std::regex justDate_Time("'20[0-9][0-9]-[0-1][0-9]-[0-3][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9]'");
    std::regex justSiteID("[0-9][0-9]*");
    std::regex justCharString("[a-zA-Z][a-zA-Z0-9]*");
    if ( eu.dateTime_File ) {
        eu.reading_date_time_file = true; //
        int lineCounter = 0;
        myInFile.open(eu.ptrToInputFile, std::ios::in);
        getline(myInFile, eu.ID);  //Let's get rid of the header line. Note that at this point eu.ID contains the entire line of input.
        eu.result = regex_match(eu.ID,eu.mr, justCharString);
        cout << "Length of the header line is " << std::dec << eu.ID.length() << " characters. " << endl;
        while( true  ) {
            getline(myInFile, eu.ID, '|');
            if(myInFile.eof()) {
                cout << "Reached end of file on file " << eu.dateTime_File << ", terminating processing." << endl;
                myInFile.close();
                return(0);
            }
            if(eu.debug1) cout << "eu.ID, right after getline===>" << eu.ID << "<===" <<endl;
            eu.result = regex_search(eu.ID, eu.mr, justSiteID);
            if (eu.debug1) {
                cout << "eu.ID: " << eu.ID << endl;
                cout << "mr.str()==>" << eu.mr.str() << "<===" << endl;
                cout << "mr.length(): " << eu.mr.length() << endl;
                cout << "mr.position(): " << eu.mr.position() << endl;
                cout << "mr.prefix():==>" << eu.mr.prefix()<< "<==" << endl;
                cout << "mr.suffix():==>" << eu.mr.suffix()<< "<==" << endl;
            }
            if (eu.mr.position() > 0 ) {
                getline(myInFile, eu.ID); //Skip the rest of this line because it's a comment line.
                continue;                 //Go read the next record.
                
            }
            eu.ID = eu.mr.str();
            getline(myInFile, eu.SDT, '|');
            if(eu.debug1) cout << "eu.SDT, right after getline===>" << eu.SDT << "<=== Has a length of "<< eu.SDT.length() << " characters." <<endl;
            eu.result = regex_search(eu.SDT, eu.mr, justDate_Time);
            if ( eu.mr.length() < 21 ) {
                cerr << "Line " << __LINE__ << " of file " << __FILE__ << ": Skipping this record because the length of StartDate&Time is < 21 characters and is therefore considered malformed" << endl;
                continue;
            }
            if (eu.debug1) {
                cout << "eu.SDT: " << eu.SDT << endl;
                cout << "mr.str()==>" << eu.mr.str() << "<===" << endl;
                cout << "mr.length(): " << eu.mr.length() << endl;
                cout << "mr.position(): " << eu.mr.position() << endl;
                cout << "mr.prefix():==>" << eu.mr.prefix()<< "<==" << endl;
                cout << "mr.suffix():==>" << eu.mr.suffix()<< "<==" << endl;
            }
            eu.SDT = eu.mr.str();
            eu.ptrStartDate_Time = eu.SDT.c_str(); //Take eu.SDT's c++ string and convert it to a C pointer to string because Postgres likes to work with C-type strings!
            setUpStartDateTime(eu, eu.ptrStartDate_Time); //Make it look like EndDate_Time is consistent with having come from the command line.
            // ---------------------------------------------
            getline(myInFile, eu.EDT, '|');
            if(eu.debug1) cout << "eu.EDT, right after getline===>" << eu.EDT << "<=== Has a length of "<< eu.EDT.length() << " characters." <<endl;
            //            eu.mr.~match_results();
            eu.result = regex_search(eu.EDT, eu.mr, justDate_Time);
            if ( eu.mr.length() < 21 ) {
                cerr << "Line " << __LINE__ << " of file " << __FILE__ << ": Skipping this record because the length of EndDate&Time is < 21 characters and is therefore considered malformed" << endl;
                continue;
            }
            if(eu.debug1) {
                cout << "eu.EDT: " << eu.EDT << endl;
                cout << "mr.str(): " << eu.mr.str() << endl;
                cout << "mr.length(): " << eu.mr.length() << endl;
                cout << "mr.position(): " << eu.mr.position() << endl;
                cout << "mr.prefix():==>" << eu.mr.prefix()<< "<==" << endl;
                cout << "mr.suffix():==>" << eu.mr.suffix() << "<==" << endl;
            }
            eu.EDT = eu.mr.str();
            eu.ptrEndDate_Time = eu.EDT.c_str(); //Take eu.SDT's c++ string and convert it to a C pointer to string because Postgres likes to work with C-type strings!
            setUpEndDateTime(eu, eu.ptrEndDate_Time); //Make it look like EndDate_Time is consistent with having come from the command line.
            // ---------------------------------------------
            getline(myInFile, eu.SID);
            //            eu.mr.~match_results();
            //            eu.result = regex_search(eu.SID, eu.mr, justSiteID);
            //            eu.SID = eu.mr.str();
            eu.ptrSiteID = eu.SID.c_str();
            eu.siteid = eu.SID.c_str();
            setUpSiteID(eu, eu.siteid);  //Make it look like siteid is consistent with having come from the command line.
            ++lineCounter;
            //            eu.mr.~match_results();
            // ---------------------------------------------
            eu.rc=doTheWork(eu);
            if(eu.rc >0) {
                if(eu.debug1) cerr << "Bad Return code from doTheWork is " << eu.rc << endl;
            } else {
                if(eu.debug1) cout << "Good Return code from doTheWork. Processing continues … " << endl;
            }
        }  //End of while loop
    } //End of if(eu.dateTime_file)
    
    //   while (eu.reading_date_time_file && (eu.myInFile.rdbuf() >> eu.ptrID >> eu.ptrStartDate_Time >> eu.ptrEndDate_Time >> eu.ptrSiteID )) {}
    return 0;
} //End of main
//
// =======================================================================================================================================================
//
short doTheWork(EnergyUsage &eu, bool doInsertInto) {
    
    //   eu.endDateTime = eu.ptrEndDate_Time;
    //   eu.startDateTime = eu.ptrStartDate_Time;
    if (memcmp(eu.startDateTime, eu.endDateTime, strlen(eu.startDateTime)) > 0) {
        std::cerr << "Interchanging startDateTime with endDateTime because startDateTime > endDateTime. startDateTime must always be less than endDateTime." << std::endl;
        //These 2 should represent the end date
        eu.param_values[DOLLAR2] = eu.param_values[DOLLAR1];
        eu.param_values[DOLLAR5] = eu.param_values[DOLLAR4];
        //These 2 should represent the start date
        eu.param_values[DOLLAR1] = eu.param_values[DOLLAR2];
        eu.param_values[DOLLAR4] = eu.param_values[DOLLAR5];
        
    } else {
        //No need to interchange the start date and end date.
        eu.param_values[DOLLAR1] = eu.param_values[DOLLAR1];
        eu.param_values[DOLLAR4] = eu.param_values[DOLLAR4];
        eu.param_values[DOLLAR2] = eu.param_values[DOLLAR2];
        eu.param_values[DOLLAR5] = eu.param_values[DOLLAR5];
    }

    for (eu.loopCounter=0; eu.loopCounter < (&eu.null0 - &eu.tick0); eu.loopCounter++) {
        *(&eu.tick0 + eu.loopCounter) = *(eu.endDateTime + eu.loopCounter);
    }
    eu.tick1 = eu.tick0; //Using the left-most field that delineates the date field, copy this tick to make the right-most tick that delineates the date field so that the date field is festooned with ticks.
    eu.null0 = NULL; //Turn the ISO 8601 blank into a string-terminating NULL character.
    eu.tick2 = eu.tick1; //The right-most tick delineating the date field.
    eu.null1 = NULL; //Make Sure this is zero, too
    //    memcpy((char *)eu.dTB.h0, eu.endDateTime+11, 9);
    //    short endLC = (&eu.dTB.h0 - &eu.dTB.tick0);
    for (eu.loopCounter = 0; eu.loopCounter < (&eu.null1 - &eu.h0); eu.loopCounter++) {
        //        c = *(eu.endDateTime+i);
        //        *(ptrToChar + i) = c;
        //        *(&eu.dTB.tick0 + i) = c;
        *(&eu.h0 + eu.loopCounter) = *(eu.endDateTime + 12 + eu.loopCounter);
    }
    eu.null1 =NULL; //Force this to a string-terminating null character.
    eu.justDate = &eu.tick0; //The left-most tick marking the start of the date field.
    eu.justTime = &eu.tick2; //The left-most tick marking the start of the time field.
    const char * const *paramValues[] = {(const char * const *)eu.startDateTime, (const char * const *)eu.endDateTime, (const char * const *)eu.siteid,  (const char * const *)eu.startDateTime, (const char * const *)eu.endDateTime, (const char * const *)eu.siteid};
    eu.conn1 = PQconnectdbParams(eu.kw, eu.cs1,0);
    if (PQstatus(eu.conn1) != CONNECTION_OK ) {
        std::cerr << "Line " << __LINE__ << ", in File " << __FILE__ << ": Connection to database " << *(eu.cs1 + 0) << " failed because of " << PQerrorMessage(eu.conn1) << std::endl;
        PQfinish(eu.conn1);
        exit (PRIMARYCONNECTFAILURE);
    }
    for (eu.j=0; eu.j<3; eu.j++) {                      //For each of the three meters, obtain the polynomial coefficients, from, table tbl_poly_fit_with_cov, and store them away for computation.
        eu.rslt1 = PQexec(eu.conn1, bestKind[eu.j] ); //2018-07-26T08:55:46 replaced moass[j] with bestKind[j] to get the best polynomial correlation coefficients for computing energy usage.

        if (PQresultStatus(eu.rslt1) != PGRES_TUPLES_OK) {
            std::cerr << "Line " << __LINE__ << ", in File " << __FILE__ << ": Error message from PQexec: " << PQerrorMessage(eu.conn1) << std::endl;
            PQclear(eu.rslt1);
            PQfinish(eu.conn1);
            return(PRIMARYSQLCOMMANDFAILURE);
        } else {
            std::cout << "Line " << __LINE__ << ", in file " << __FILE__ << ", rslt1 result status is: " << PQresultStatus(eu.rslt1) << std::endl;
        }
        for (eu.i=0; eu.i< 4; eu.i++) {
            eu.work = PQgetvalue(eu.rslt1, 0, eu.i);
            eu.coeff[eu.j][eu.i] = atof( eu.work );
        }
        PQclear(eu.rslt1);
    }

    //======================================================================================

    eu.conn2 = PQconnectdbParams(eu.kw, eu.cs2,0);
    if (PQstatus(eu.conn2) != CONNECTION_OK ) {
        std::cerr << "Connection to database " << *(eu.cs2 + 0) << " failed because of " << PQerrorMessage(eu.conn2) << std::endl;
        PQfinish(eu.conn2);
        return(SECONDARYCONNECTFAILURE);
    }
    eu.rslt2 = PQexecParams(eu.conn2, sql_prototype, NUMBEROFPARAMETERS, nullptr, eu.param_values,  nullptr, nullptr, 0);
    if (PQresultStatus(eu.rslt2) != PGRES_TUPLES_OK) {
        std::cerr << "Line " << __LINE__ << ", in File " << __FILE__ << ": Error message from PQexec: " << PQerrorMessage(eu.conn2) << ", for SQL command that looks likeL " << sql_prototype << std::endl;
        PQclear(eu.rslt2);
        PQfinish(eu.conn2);
        return (SECONDARYSQLCOMMANDFAILURE);
    } else {
        ;
    }
    eu.sumOfWeightedTemperatures = atof(PQgetvalue(eu.rslt2, 0, 0));
    eu.sumOfWeights= atof(PQgetvalue(eu.rslt2, 0, 1));
    eu.work = PQgetvalue(eu.rslt2, 0, 2);
    double weightedSum = atof(eu.work);
    ;
    
    VaryingType<short> s;
    s.in64.d64 = eu.shortSiteId;
    s.toNetworkByteOrder();
    eu.NBOsiteID = s.out64.d64;
    eu.param_values[3] = (const char *)&eu.NBOsiteID;

    VaryingType<float> f;
    PQclear(eu.rslt2);
    std::cout << "For start date&time: " << eu.startDateTime << ", end date&time: " << eu.endDateTime << "sum Of Weighted Temperatures: " << eu.sumOfWeightedTemperatures << ", sumOfWeights: " << eu.sumOfWeights << "; average Temperature: " << eu.work << std::endl;
    eu.computedEnergyUsageM1M2 = (eu.coeff[0][0] + (eu.coeff[0][1] + (eu.coeff[0][2] + eu.coeff[0][3] * weightedSum ) * weightedSum ) * weightedSum);
    f.in64.d64 = (float)eu.computedEnergyUsageM1M2;
    f.toNetworkByteOrder(); //Put the computed (modeled) energy usage represented by meter m1m2kwh into NBO.
    eu.NBOcomputedEnergyUsageM1M2 = f.out64.d64; //Extract the M1M2 energy usage, in NBO and ready for PostgreSQL processing of binary data.
    eu.param_values[2] = (const char *)&eu.NBOcomputedEnergyUsageM1M2;
    
    //    eu.ssM1M2 << eu.computedEnergyUsageM1M2;
    sprintf(eu.bufM1M2, "%f", eu.computedEnergyUsageM1M2 );
    // ------------------------------
    eu.computedEnergyUsageM1 =   (eu.coeff[1][0] + (eu.coeff[1][1] + (eu.coeff[1][2] + eu.coeff[1][3] * weightedSum ) * weightedSum ) * weightedSum);
    f.in64.d64 = (float)eu.computedEnergyUsageM1;
    f.toNetworkByteOrder(); //Put the computed (modeled) energy usage represented by meter m1kwh into NBO.
    eu.NBOcomputedEnergyUsageM1 = f.out64.d64; //Extract the M1 energy usage, in NBO and ready for PostgreSQL processing of binary data.
    eu.param_values[0] = (const char *)&eu.NBOcomputedEnergyUsageM1;
    //    eu.ssM1 << eu.computedEnergyUsageM1 ;
    sprintf(eu.bufM1, "%f", eu.computedEnergyUsageM1 );
    // ------------------------------
    eu.computedEnergyUsageM2 =  (eu.coeff[2][0] + (eu.coeff[2][1] + (eu.coeff[2][2] + eu.coeff[2][3] * weightedSum ) * weightedSum ) * weightedSum);
    f.in64.d64 = (float)eu.computedEnergyUsageM2;
    f.toNetworkByteOrder(); //Put the computed (modeled) energy usage represented by meter m2kwh into NBO.
    eu.NBOcomputedEnergyUsageM2 = f.out64.d64; //Extract the M2 energy usage, in NBO and ready for PostgreSQL processing of binary data.
    eu.param_values[1] = (const char *)&eu.NBOcomputedEnergyUsageM2;
    sprintf(eu.bufM2, "%f", eu.computedEnergyUsageM2 );
    //    std::stringstream ssM2;
    //    eu.ssM2 << eu.computedEnergyUsageM2;
    // ------------------------------
    std::cout << "computedEnergyUsageM1M2: " << eu.computedEnergyUsageM1M2 << " kwh; computedEnergyUsageM1: " << eu.computedEnergyUsageM1  << "kwh; computedEnergyUsageM2: " << eu.computedEnergyUsageM1 << " kwh." <<std::endl;
    //====================================================================================== NOW UPDATE tbl_energy_usage to represent in columns mm1kwh, mm2kwh, and mm1m2kwh the modeled energy usage as we computed, above

    if(doInsertInto) {
        eu.sw1.Restart();
        std::stringstream ssInsertInto;
        ssInsertInto << "INSERT INTO tbl_modeled_energy_usage (date, time, temperature, siteid, mm1kwh, mm2kwh, mm1m2kwh) VALUES(" << eu.justDate << ", " << eu.justTime << ", " << eu.work << ", " << eu.siteid << ", " << eu.bufM1 << ", " << eu.bufM2 << ", " << eu.bufM1M2 << ");";
        eu.lsw1 = (int)eu.sw1.ElapsedNs();
        if (eu.debug2) std::cout << "Insert INTO SQL command looks like:\n" << ssInsertInto.str().c_str() << "\nIt took " << eu.lsw1 << " nanoseconds to build this SQL command using c++ ostringstream techniques." << std::endl;
        eu.ptrToSQL= ssInsertInto.str().c_str();
        eu.rslt1 = PQexecParams(eu.conn1, eu.ptrToSQL, 0, nullptr, nullptr, nullptr, nullptr, 0);
        if (  (PQresultStatus(eu.rslt1) != PGRES_COMMAND_OK)  &&  (PQresultStatus(eu.rslt1) != PGRES_TUPLES_OK)  ) {
            std::cerr << "Error message from PQresultStatus driven by PQexecParams: " << PQerrorMessage(eu.conn1) << std::endl;
            std::cerr << "Line " << __LINE__ << ", in File " << __FILE__ << ": Message from PQresultStatus driven by PQexecParams: " << PQresStatus(PQresultStatus(eu.rslt1) ) << std::endl;
            PQclear(eu.rslt1);
            PQfinish(eu.conn1);
            return(INSERTINTOFAILURE);
        } else {
            if(eu.debug2) {
                std::cout << "Didn't Fail! Message from PQresultStatus driven by PQexecParams: " << PQresStatus(PQresultStatus(eu.rslt1) ) << "\n";
                std::cout << "Value returned by this INSERT INTO command: " << PQgetResult(eu.conn1) << std::endl;
            }
            PQclear(eu.rslt1);
        }
    } else {   ///Come here to set up the update sql command and then execute it.
        eu.sw1.Restart();
        std::ostringstream ssUpdate;
        ssUpdate << "UPDATE tbl_energy_usage will set mm1kwh=" << eu.bufM1 << ", mm2kwh=" << eu.bufM2 << ", mm1m2kwh="  << eu.bufM1M2 << " where siteid=" << eu.siteid << " AND date=" << eu.justDate << " AND time::TEXT=" << eu.justTime;
        eu.lsw1 = (int)eu.sw1.ElapsedNs();

        eu.sw2.Restart();
        char *offset;
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, "UPDATE tbl_energy_usage set mm1kwh=");
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, eu.bufM1 );
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, ", mm2kwh=" );
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, eu.bufM2 );
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, ", mm1m2kwh=" );
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, eu.bufM1M2 );
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, " where siteid=");
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, eu.siteid);
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, " AND date=");
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, eu.justDate);
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, " AND time::TEXT=");
        offset=strcat(eu.bufferUsedForConstructingAnSQLStatement, eu.justTime);
        eu.lsw2 = (int)eu.sw2.ElapsedNs();
        const char *strUpdate;
        eu.ptrToSQL = ssUpdate.str().c_str();
        ; //strUpdate = ssUpdate;
        std::cout << "It took " << (int)eu.lsw1 << " nanoseconds to build the UPDATE SQL command using the c++ `stringstream` technique,\n and it took " << (int)eu.lsw2 << " nanoseconds to build the UPDATE SQL command using the convention `C-like` strcat method." << "\nthe command build by the string stream method looks like: " << ssUpdate.str() << "\n while the command built by the conventional `C` strcat method looks like: " << eu.bufferUsedForConstructingAnSQLStatement << ".\nTherefore we can state that the `C` stringcat method is " << (double)eu.lsw1/(double)eu.lsw2 << " times faster than the c++ ostringstream method."  << std::endl;
        //    param_values[3] = (const char *)&eu.NBOsiteID;
        eu.rslt2 = PQexecParams(eu.conn2, eu.ptrToSQL, 0, nullptr, nullptr, nullptr, nullptr, 0);
        if (  (PQresultStatus(eu.rslt2) != PGRES_COMMAND_OK)  &&  (PQresultStatus(eu.rslt2) != PGRES_TUPLES_OK)  ) {
            std::cout << "Error message from PQresultStatus driven by PQexecParams: " << PQerrorMessage(eu.conn2) << std::endl;
            std::cout << "Line " << __LINE__ << ", in File " << __FILE__ << ": Message from PQresultStatus driven by PQexecParams: " << PQresStatus(PQresultStatus(eu.rslt2) ) << std::endl;
            PQclear(eu.rslt2);
            PQfinish(eu.conn2);
            return(UPDATESQLFAILURE);
        } else {
            if(eu.debug2) {
                std::cout << "Didn't Fail! Message from PQresultStatus driven by PQexecParams: " << PQresStatus(PQresultStatus(eu.rslt2) ) << std::endl;
                std::cout << "Value returned by this Update command: " << PQgetResult(eu.conn2) << std::endl;
            }
        }
    }
//    eu.param_values[4] = (const char *)&eu.justDate;
//    eu.param_values[5] = (const char *)&eu.justTime;

    eu.clearAllBuffers();  //Just in case we have to come back again.
//   PQclear(eu.rslt2); Doing this results in ABNORMAL TERMINATION
//   PQclear(eu.rslt1); Doing this results in ABNORMAL TERMINATION
    PQfinish(eu.conn2);
    PQfinish(eu.conn1);
//  d.VaryingType::~VaryingType(); //Don't need this.
    f.VaryingType::~VaryingType();
    s.VaryingType::~VaryingType();
    return 0;
}

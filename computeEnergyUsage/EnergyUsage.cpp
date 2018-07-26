//
//  EnergyUsage.cpp
//  computeEnergyUsage
//
//  Created by Clifford Campo on 7/18/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#include "EnergyUsage.hpp"
EnergyUsage::EnergyUsage() {
/* Zero-out the various buffers so we know they're clean
    memset(bufferUsedForConstructingAnSQLStatement, NULL, SIZEOFbufferUsedForConstructingAnSQLStatement);
    memset(bufM1, NULL, BUFSIZE);
    memset(bufM2, NULL, BUFSIZE);
    memset(bufM1M2, NULL, BUFSIZE);
*/
 //   char *primaryConnectionString = new char [5];
    *(primaryConnectionString + 0) = (char *)DatabaseName;
    *(primaryConnectionString + 1) = (char *)Port[0];
    *(primaryConnectionString + 2) = (char *)UserID[0];
    *(primaryConnectionString + 3) = NULL;
    *(primaryConnectionString + 4) = (char *)Host[0];
//    char *secondaryConnectionString = new char [5];
    *(secondaryConnectionString + 0) = (char *)DatabaseName;
    *(secondaryConnectionString + 1) = (char *)Port[1];
    *(secondaryConnectionString + 2) = (char *)UserID[1];
    *(secondaryConnectionString + 3) = NULL;
    *(secondaryConnectionString + 4) = (char *)Host[1];
    this->cs1 = primaryConnectionString;     //Use the default connection string representing the primary, home-site, connection.
    this->cs2 = secondaryConnectionString;   //Use the default connection string for the secondary, away from home, connection.
    clearAllBuffers();
    defaultDatabaseName = DatabaseName;
    defaultUserID = *(UserID + 0 );
    defaultPort = *(Port +0);
    defaultHost = *(Host +0 );
    useKelvin = false;
    reading_date_time_file =false;
    dateTime_File = nullptr;
  //  results_File = "/tmp/EnergyUsage.txt";

    this->kw = this->keywords;
    ptrDefaultInputFile = defaultInputFile;
    ptrToInputFile = nullptr;
    result = false;
    conn1 = nullptr;  //PostgreSQL DBMS Primary connection object
    conn2 = nullptr;  //PostgreSQL DBMS Secondary connection object
    rslt1 = nullptr;  //Result from query upon PostgreSQL database's primary conneciton
    rslt2 = nullptr;  //Result from query upon PostgreSQL database's secondary connection


}
EnergyUsage::~EnergyUsage() {
    ;
}
void EnergyUsage::clearAllBuffers(void) {
    memset(bufferUsedForConstructingAnSQLStatement, NULL, SIZEOFSQLBUFFER);
    memset(bufM1, NULL, BUFSIZE);
    memset(bufM2, NULL, BUFSIZE);
    memset(bufM1M2, NULL, BUFSIZE);
}

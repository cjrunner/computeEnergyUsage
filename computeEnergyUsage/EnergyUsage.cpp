//
//  EnergyUsage.cpp
//  computeEnergyUsage
//
//  Created by Clifford Campo on 7/18/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
#include <libpq-fe.h>
#include "EnergyUsage.hpp"
#include "varyingType.hpp"
extern const char *sql_prototype[];
extern const char *ptrBestCoefficientsArray[];
extern const char *ptrSQLForM2Kwh[];
extern const int numberOfArrayElements;
EnergyUsage::EnergyUsage() : SQLSelects( ) {
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
    seasonalBasedApproach = true;
    whichConnection = CONN1AND2CLOSED;
  //  results_File = "/tmp/EnergyUsage.txt";

    this->kw = this->keywords;
    ptrDefaultInputFile = defaultInputFile;
    ptrToInputFile = nullptr;
    result = false;
    leapYear = false;
    summerSeason=false;
    connectionToHomeSite = nullptr;  //PostgreSQL DBMS Primary connection object
    remoteSiteConnection = nullptr;  //PostgreSQL DBMS Secondary connection object
    rslt1 = nullptr;  //Result from query upon PostgreSQL database's primary conneciton
    rsltRemoteSite = nullptr;  //Result from query upon PostgreSQL database's secondary connection


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
void EnergyUsage::closeConnections(u_int8_t whichOpenConnection) {
    if( (whichOpenConnection & CONNECTIONTOHOMESITEISOPEN) && (whichConnection & CONNECTIONTOHOMESITEISOPEN) ) { //If a request is made to close connection 1 and connection 1 is really open then close connection 1.
        PQfinish(connectionToHomeSite);
        whichConnection &=  (ALLBITSHIGH ^ CONNECTIONTOHOMESITEISOPEN); //Mark connection 1 closed.
    }
    if( (whichOpenConnection & CONNECTIONTOREMOTESITEISOPEN) && (whichConnection & CONNECTIONTOREMOTESITEISOPEN) ) { //If a request is made to close connection 2 and connection 2 is really open then close connection 2.
        PQfinish(remoteSiteConnection);
        whichConnection &=  (ALLBITSHIGH ^ CONNECTIONTOREMOTESITEISOPEN); //Mark connection 2 closed.
    }
}
void EnergyUsage::clearActiveRslt(u_int8_t whichActive) {
    if( (whichActive & RSLTREMOTESITEACTIVE) && (whichActiveRslt & RSLTREMOTESITEACTIVE)   ) { //If a request is made to deactivate rsltRemoteSite and rsltRemoteSite is really active, then deactivate rsltRemoteSite.
        PQclear(rsltRemoteSite);
        whichActiveRslt &= (ALLBITSHIGH ^ RSLTREMOTESITEACTIVE); //Mark rsltRemoteSite inactive.
    }
    if( (whichActive & RSLTHOMESITEACTIVE)  && (whichActiveRslt & RSLTHOMESITEACTIVE) ) { //If a request is made to deactivate rsltRemoteSite and rsltRemoteSite is really active, then deactivate rsltRemoteSite.
        PQclear(rslt1);
        whichActiveRslt &= (ALLBITSHIGH ^ RSLTHOMESITEACTIVE); //Mark rslt1 inactive.
    }
}
//
// =======================================================================================================================================================
//
short EnergyUsage::doTheWork(bool doInsertInto) {
    
    //   endDateTime = ptrEndDate_Time;
    //   startDateTime = ptrStartDate_Time;
    if (memcmp(usdt.startDateTime,  uedt.endDateTime, strlen(usdt.startDateTime)) > 0) {
        std::cerr << "Interchanging startDateTime with endDateTime because startDateTime > endDateTime. startDateTime must always be less than endDateTime." << std::endl;
        //These 2 should represent the end date
        param_values[DOLLAR2] = param_values[DOLLAR1];
        param_values[DOLLAR5] = param_values[DOLLAR4];
        //These 2 should represent the start date
        param_values[DOLLAR1] = param_values[DOLLAR2];
        param_values[DOLLAR4] = param_values[DOLLAR5];
        
    } else {
        //No need to interchange the start date and end date.
        param_values[DOLLAR1] = param_values[DOLLAR1];
        param_values[DOLLAR4] = param_values[DOLLAR4];
        param_values[DOLLAR2] = param_values[DOLLAR2];
        param_values[DOLLAR5] = param_values[DOLLAR5];
    }
    
    for (loopCounter=0; loopCounter < (&null0 - &tick0); loopCounter++) {
        *(&tick0 + loopCounter) = *(uedt.endDateTime + loopCounter);
    }
    tick1 = tick0; //Using the left-most field that delineates the date field, copy this tick to make the right-most tick that delineates the date field so that the date field is festooned with ticks.
    null0 = NULL; //Turn the ISO 8601 blank into a string-terminating NULL character.
    tick2 = tick1; //The right-most tick delineating the date field.
    null1 = NULL; //Make Sure this is zero, too
    //    memcpy((char *)dTB.h0, endDateTime+11, 9);
    //    short endLC = (&dTB.h0 - &dTB.tick0);
    for (loopCounter = 0; loopCounter < (&null1 - &h0); loopCounter++) {
        //        c = *(endDateTime+i);
        //        *(ptrToChar + i) = c;
        //        *(&dTB.tick0 + i) = c;
        *(&h0 + loopCounter) = *(uedt.endDateTime + 12 + loopCounter);
    }
    null1 =NULL; //Force this to a string-terminating null character.
    justDate = &tick0; //The left-most tick marking the start of the date field.
    justTime = &tick2; //The left-most tick marking the start of the time field.
//    const char * const *paramValues[] = {(const char * const *)startDateTime, (const char * const *)endDateTime, (const char * const *)siteid,  (const char * const *)startDateTime, (const char * const *)endDateTime, (const char * const *)siteid};  U N U S E D        V A R I A B L E
    connectionToHomeSite = PQconnectdbParams(kw, cs1,0);
    if (PQstatus(connectionToHomeSite) != CONNECTION_OK ) {
        std::cerr << "Line " << __LINE__ << ", in File " << __FILE__ << ": Connection to database " << *(cs1 + 0) << " failed because of " << PQerrorMessage(connectionToHomeSite) << std::endl;
        //        PQfinish(connectionToHomeSite);
        closeConnections(CONNECTIONTOHOMESITEISOPEN);
        exit (PRIMARYCONNECTFAILURE);
    } else {
        whichConnection |= CONNECTIONTOHOMESITEISOPEN; //Indicate Connection 1 is open and active
    }
    for (j=0; j < numberOfArrayElements; j++) {  //For each of the three meters, obtain the polynomial coefficients, from, table tbl_poly_fit_with_cov, and store them away for computation.
        rslt1 = PQexec(connectionToHomeSite, ptrBestCoefficientsArray[j] ); //2018-07-26T08:55:46 replaced moass[j] with bestKind[j] to get the best polynomial correlation coefficients for computing energy usage.
        
        if (PQresultStatus(rslt1) != PGRES_TUPLES_OK) {
            std::cerr << "Line " << __LINE__ << ", in File " << __FILE__ << ": Error message from PQexec: " << PQerrorMessage(connectionToHomeSite) << std::endl;
            //            PQclear(rslt1);
            clearActiveRslt(RSLTHOMESITEACTIVE);
            //            PQfinish(connectionToHomeSite);
            closeConnections(CONNECTIONTOHOMESITEISOPEN);
            clearAllBuffers();  //Just in case we have to come back again.
            return(PRIMARYSQLCOMMANDFAILURE);
        } else {
            whichActiveRslt |= RSLTHOMESITEACTIVE; //Mark rslt1 as active
            std::cout << "Line " << __LINE__ << ", in file " << __FILE__ << ", rslt1 result status is: " << PQresultStatus(rslt1) << std::endl;
        }
        for (i=0; i< 4; i++) {
            work = PQgetvalue(rslt1, 0, i);
            coeff[j][i] = atof( work );
        }
        //        PQclear(rslt1);
        clearActiveRslt(RSLTHOMESITEACTIVE);
    }
    
    //======================================================================================
    
    remoteSiteConnection = PQconnectdbParams(kw, cs2,0);
    if (PQstatus(remoteSiteConnection) != CONNECTION_OK ) {
        std::cerr << "Connection to database " << *(cs2 + 0) << " failed because of " << PQerrorMessage(remoteSiteConnection) << std::endl;
        //        PQfinish(remoteSiteConnection);
        closeConnections(CONNECTIONTOREMOTESITEISOPEN);
        clearAllBuffers();  //Just in case we have to come back again.
        return(SECONDARYCONNECTFAILURE);
    } else {
        whichConnection |= CONNECTIONTOREMOTESITEISOPEN; //Indicate connection2  is open and active.
    }
    rsltRemoteSite = PQexecParams(remoteSiteConnection, *sql_prototype, NUMBEROFPARAMETERS, nullptr, param_values,  nullptr, nullptr, 0);
    if (PQresultStatus(rsltRemoteSite) != PGRES_TUPLES_OK) {
        std::cerr << "Line " << __LINE__ << ", in File " << __FILE__ << ": Error message from PQexec: " << PQerrorMessage(remoteSiteConnection) << ", for SQL command that looks likeL " << sql_prototype << std::endl;
        //        PQclear(rsltRemoteSite);
        clearActiveRslt(RSLTREMOTESITEACTIVE);
        //        PQfinish(remoteSiteConnection);
        closeConnections(CONNECTIONTOREMOTESITEISOPEN);
        clearAllBuffers();  //Just in case we have to come back again.
        return (SECONDARYSQLCOMMANDFAILURE);
    } else {
        whichActiveRslt |= RSLTREMOTESITEACTIVE; //Mark rsltRemoteSite as active
    }
    sumOfWeightedTemperatures = atof(PQgetvalue(rsltRemoteSite, 0, 0));
    sumOfWeights= atof(PQgetvalue(rsltRemoteSite, 0, 1));
    work = PQgetvalue(rsltRemoteSite, 0, 2);
    this->averageTemperature = (double)atof(work);
    //
    VaryingType<short> s;
    s.in64.d64 = shortSiteId;
    s.toNetworkByteOrder();
    NBOsiteID = s.out64.d64;
    param_values[3] = (const char *)&NBOsiteID;
    
    VaryingType<float> f;
    //    PQclear(rsltRemoteSite);
    clearActiveRslt(RSLTREMOTESITEACTIVE);
    std::cout << "For start date&time: " << usdt.startDateTime << ", end date&time: " << uedt.endDateTime << "sum Of Weighted Temperatures: " << sumOfWeightedTemperatures << ", sumOfWeights: " << sumOfWeights << "; average Temperature: " << this->averageTemperature << std::endl;
    computedEnergyUsageM1M2 = (coeff[0][0] + (coeff[0][1] + (coeff[0][2] + coeff[0][3] * weightedSum ) * weightedSum ) * weightedSum);
    f.in64.d64 = (float)computedEnergyUsageM1M2;
    f.toNetworkByteOrder(); //Put the computed (modeled) energy usage represented by meter m1m2kwh into NBO.
    NBOcomputedEnergyUsageM1M2 = f.out64.d64; //Extract the M1M2 energy usage, in NBO and ready for PostgreSQL processing of binary data.
    param_values[2] = (const char *)&NBOcomputedEnergyUsageM1M2;
    
    //    ssM1M2 << computedEnergyUsageM1M2;
    sprintf(bufM1M2, "%f", computedEnergyUsageM1M2 );
    // ------------------------------
    computedEnergyUsageM1 =   (coeff[1][0] + (coeff[1][1] + (coeff[1][2] + coeff[1][3] * weightedSum ) * weightedSum ) * weightedSum);
    f.in64.d64 = (float)computedEnergyUsageM1;
    f.toNetworkByteOrder(); //Put the computed (modeled) energy usage represented by meter m1kwh into NBO.
    NBOcomputedEnergyUsageM1 = f.out64.d64; //Extract the M1 energy usage, in NBO and ready for PostgreSQL processing of binary data.
    param_values[0] = (const char *)&NBOcomputedEnergyUsageM1;
    //    ssM1 << computedEnergyUsageM1 ;
    sprintf(bufM1, "%f", computedEnergyUsageM1 );
    // ------------------------------
// ===========================> Here we need to use the seasonal based approach <=============> Here we need to use the seasonal based approach <================>
    if(seasonalBasedApproach) {
        //Need to use connectionToHomeSite because connectionToHomeSite is associated with the home-site's LocalWeather database and it is only in the \
        home-site's LocalWeather database where we will find the seasonal energy usage as measured by energy (kwh) meter M2.
        if ( !(whichConnection &= CONNECTIONTOHOMESITEISOPEN) ) { // if remoteSiteConnection is not open -- which would be most unusal at this point in the program -- then open it
            connectionToHomeSite = PQconnectdb("dbname=LocalWeather user=cjc port=5436 host=localhost"); //Make a simple and fast connection to our home site.
            if (PQstatus(connectionToHomeSite) != CONNECTION_OK ) {
                std::cerr << "Line " << __LINE__ << " in file " << __FILE__ << ", Connection to database " << *(cs2 + 0) << " failed because of " << PQerrorMessage(remoteSiteConnection) << std::endl;;
                closeConnections(CONNECTIONTOHOMESITEISOPEN);
                clearAllBuffers();  //Just in case we have to come back again.
                return(PRIMARYCONNECTFAILURE);
            } else {
                whichConnection |= CONNECTIONTOHOMESITEISOPEN; //Indicate connection2  is open and active.
            }
        }
        const char *thisSQL = SQLSelects::returnSQLSelect(SUMMERONLYSEASON);
        if (debug2) cout << "ptrSQLForM2Kwh[" << (int)summerSeason << "] Looks like:\n" << thisSQL<< endl;
        rslt1 = PQexec(connectionToHomeSite,thisSQL );
        if (PQresultStatus(rslt1) != PGRES_TUPLES_OK) {
            std::cerr << "Line " << __LINE__ << ", in File " << __FILE__ << ": Error message from PQexec: " << PQerrorMessage(connectionToHomeSite) << std::endl;
            //            PQclear(rslt1);
            clearActiveRslt(RSLTHOMESITEACTIVE);
            //            PQfinish(connectionToHomeSite);
            closeConnections(CONNECTIONTOHOMESITEISOPEN);
            clearAllBuffers();  //Just in case we have to come back again.
            return(PRIMARYSQLCOMMANDFAILURE);
        } else {
            whichActiveRslt |= RSLTHOMESITEACTIVE; //Mark rsltRemoteSite as active
            std::cout << "Line " << __LINE__ << ", in file " << __FILE__ << ", rslt1 result status is: " << PQresultStatus(rslt1) << std::endl;
        }
        work = PQgetvalue(rslt1, 0, 0);
        computedEnergyUsageM2 = atof(work);
        computedEnergyUsageM1M2  = computedEnergyUsageM2 + computedEnergyUsageM1;
        sprintf(bufM2, "%f", computedEnergyUsageM2 );
        sprintf(bufM1M2, "%f", computedEnergyUsageM1M2 );
        std::cout << "derived Energy Usage M1+M2: " << computedEnergyUsageM1M2 << " kwh; computedEnergyUsageM1: " << computedEnergyUsageM1  << " kwh; seasonal based M2 energy usage is: " << computedEnergyUsageM2 << " kwh." <<std::endl;
        clearActiveRslt(RSLTHOMESITEACTIVE);
//        closeConnections(CONNECTIONTOREMOTESITEISOPEN);
    } else {
        
        computedEnergyUsageM2 =  (coeff[2][0] + (coeff[2][1] + (coeff[2][2] + coeff[2][3] * weightedSum ) * weightedSum ) * weightedSum);
        f.in64.d64 = (float)computedEnergyUsageM2;
        f.toNetworkByteOrder(); //Put the computed (modeled) energy usage represented by meter m2kwh into NBO.
        NBOcomputedEnergyUsageM2 = f.out64.d64; //Extract the M2 energy usage, in NBO and ready for PostgreSQL processing of binary data.
        param_values[1] = (const char *)&NBOcomputedEnergyUsageM2;
        sprintf(bufM2, "%f", computedEnergyUsageM2 );
        //    std::stringstream ssM2;
        //    ssM2 << computedEnergyUsageM2;
        std::cout << "computedEnergyUsageM1M2: " << computedEnergyUsageM1M2 << " kwh; computedEnergyUsageM1: " << computedEnergyUsageM1  << "kwh; computedEnergyUsageM2: " << computedEnergyUsageM2 << " kwh." <<std::endl;
    }
    // ------------------------------

    //====================================================================================== NOW UPDATE tbl_energy_usage to represent in columns mm1kwh, mm2kwh, and mm1m2kwh the modeled energy usage as we computed, above
    
    if(doInsertInto) {
        sw1.Restart();

        ssInsertInto << "INSERT INTO tbl_modeled_energy_usage (date, time, temperature, siteid, mm1kwh, mm2kwh, mm1m2kwh) VALUES(" << justDate << ", " << justTime << ", " << this->averageTemperature << ", " << siteid << ", " << bufM1 << ", " << bufM2 << ", " << bufM1M2 << ");";
        lsw1 = (int)sw1.ElapsedNs();
        if (debug2) std::cout << "Insert INTO SQL command looks like:\n" << ssInsertInto.str().c_str() << "\nIt took " << lsw1 << " nanoseconds to build this SQL command using c++ ostringstream techniques." << std::endl;
        ptrToSQL= ssInsertInto.str().c_str();
        rslt1 = PQexecParams(connectionToHomeSite, ptrToSQL, 0, nullptr, nullptr, nullptr, nullptr, 0);
        if (  (PQresultStatus(rslt1) != PGRES_COMMAND_OK)  &&  (PQresultStatus(rslt1) != PGRES_TUPLES_OK)  ) {
            std::cerr << "Error message from PQresultStatus driven by PQexecParams: " << PQerrorMessage(connectionToHomeSite) << std::endl;
            std::cerr << "Line " << __LINE__ << ", in File " << __FILE__ << ": Message from PQresultStatus driven by PQexecParams: " << PQresStatus(PQresultStatus(rslt1) ) << std::endl;
            //            PQclear(rslt1);
            clearActiveRslt(RSLTHOMESITEACTIVE);
            closeConnections(CONNECTIONTOREMOTESITEISOPEN + CONNECTIONTOHOMESITEISOPEN);
            clearAllBuffers();  //Just in case we have to come back again.
            //            PQfinish(remoteSiteConnection); //Close the secondary connection
            //            PQfinish(connectionToHomeSite); //Close the primary connection
            return(INSERTINTOFAILURE);
        } else {
            whichActiveRslt |= RSLTHOMESITEACTIVE; //Indicate rslt1 is active
            if(debug2) {
                std::cout << "Didn't Fail! Message from PQresultStatus driven by PQexecParams: " << PQresStatus(PQresultStatus(rslt1) ) << "\n";
                std::cout << "Value returned by this INSERT INTO command: " << PQgetResult(connectionToHomeSite) << std::endl;
            }
            //           PQclear(rslt1);
            clearActiveRslt(RSLTHOMESITEACTIVE); //Clear rslt1
        }
    } else {   ///Come here to set up the update sql command and then execute it.
        sw1.Restart();
        std::ostringstream ssUpdate;
        ssUpdate << "UPDATE tbl_energy_usage will set mm1kwh=" << bufM1 << ", mm2kwh=" << bufM2 << ", mm1m2kwh="  << bufM1M2 << " where siteid=" << siteid << " AND date=" << justDate << " AND time::TEXT=" << justTime;
        lsw1 = (int)sw1.ElapsedNs();
        
        sw2.Restart();
        char *offset;
        offset=strcat(bufferUsedForConstructingAnSQLStatement, "UPDATE tbl_energy_usage set mm1kwh=");
        offset=strcat(bufferUsedForConstructingAnSQLStatement, bufM1 );
        offset=strcat(bufferUsedForConstructingAnSQLStatement, ", mm2kwh=" );
        offset=strcat(bufferUsedForConstructingAnSQLStatement, bufM2 );
        offset=strcat(bufferUsedForConstructingAnSQLStatement, ", mm1m2kwh=" );
        offset=strcat(bufferUsedForConstructingAnSQLStatement, bufM1M2 );
        offset=strcat(bufferUsedForConstructingAnSQLStatement, " where siteid=");
        offset=strcat(bufferUsedForConstructingAnSQLStatement, siteid);
        offset=strcat(bufferUsedForConstructingAnSQLStatement, " AND date=");
        offset=strcat(bufferUsedForConstructingAnSQLStatement, justDate);
        offset=strcat(bufferUsedForConstructingAnSQLStatement, " AND time::TEXT=");
        offset=strcat(bufferUsedForConstructingAnSQLStatement, justTime);
        lsw2 = (int)sw2.ElapsedNs();
//        const char *strUpdate;  UNUSED VARIABLE               U N U S E D        V A R I A B L E
        ptrToSQL = ssUpdate.str().c_str();
        ; //strUpdate = ssUpdate;
        std::cout << "It took " << (int)lsw1 << " nanoseconds to build the UPDATE SQL command using the c++ `stringstream` technique,\n and it took " << (int)lsw2 << " nanoseconds to build the UPDATE SQL command using the convention `C-like` strcat method." << "\nthe command build by the string stream method looks like: " << ssUpdate.str() << "\n while the command built by the conventional `C` strcat method looks like: " << bufferUsedForConstructingAnSQLStatement << ".\nTherefore we can state that the `C` stringcat method is " << (double)lsw1/(double)lsw2 << " times faster than the c++ ostringstream method."  << std::endl;
        //    eu.param_values[3] = (const char *)&NBOsiteID;
        rsltRemoteSite = PQexecParams(remoteSiteConnection, ptrToSQL, 0, nullptr, nullptr, nullptr, nullptr, 0);
        if (  (PQresultStatus(rsltRemoteSite) != PGRES_COMMAND_OK)  &&  (PQresultStatus(rsltRemoteSite) != PGRES_TUPLES_OK)  ) {
            std::cout << "Error message from PQresultStatus driven by PQexecParams: " << PQerrorMessage(remoteSiteConnection) << std::endl;
            std::cout << "Line " << __LINE__ << ", in File " << __FILE__ << ": Message from PQresultStatus driven by PQexecParams: " << PQresStatus(PQresultStatus(rsltRemoteSite) ) << std::endl;
            //            PQclear(rsltRemoteSite);
            clearActiveRslt(RSLTREMOTESITEACTIVE);
            //            PQfinish(remoteSiteConnection);
            closeConnections(CONNECTIONTOREMOTESITEISOPEN);
            clearAllBuffers();  //Just in case we have to come back again.
            return(UPDATESQLFAILURE);
        } else {
            whichActiveRslt |= RSLTREMOTESITEACTIVE; //Indicate rsltRemoteSite is active
            if(debug2) {
                std::cout << "Didn't Fail! Message from PQresultStatus driven by PQexecParams: " << PQresStatus(PQresultStatus(rsltRemoteSite) ) << std::endl;
                std::cout << "Value returned by this Update command: " << PQgetResult(remoteSiteConnection) << std::endl;
            }
        }
        clearActiveRslt(RSLTREMOTESITEACTIVE);
    }
    //    eu.param_values[4] = (const char *)&justDate;
    //    eu.param_values[5] = (const char *)&justTime;
    
    clearAllBuffers();  //Just in case we have to come back again.
    //   PQclear(rsltRemoteSite); Doing this results in ABNORMAL TERMINATION
    //   PQclear(rslt1); Doing this results in ABNORMAL TERMINATION
    //    PQfinish(remoteSiteConnection);
    //    PQfinish(connectionToHomeSite);
    closeConnections(CONNECTIONTOREMOTESITEISOPEN + CONNECTIONTOHOMESITEISOPEN);
    //  d.VaryingType::~VaryingType(); //Don't need this anymore
    f.VaryingType::~VaryingType();
    s.VaryingType::~VaryingType();
    return 0;
}

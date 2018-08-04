//
//  arraysOfConstCharacters.cpp
//  computeEnergyUsage
//
//  Created by Clifford Campo on 7/31/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
#include "arraysOfConstCharacters.hpp"
extern const char *ptrSQLForM2Kwh[];
extern const char *ptrReturnCodes[];
extern const char *ptrBestCoefficients;
extern const char *ptrBestCoefficientsArray[];
extern const char *bestKind[];
extern const char *sql_prototype;
extern const char *moass[];
extern const char *simpleMoass;
extern const char *sql_INSERT_INTO_prototype;
extern const char *sql_UPDATE_prototype;
extern const int numberOfArrayElements;
//
//===========================================================================================
//
const char *ptrActualM2="SELECT y.date, y.time, dby.date, dby.time, ((y.m2kwh-dby.m2kwh)/(y.jd-dby.jd)) AS \"M2KWH USAGE\" FROM \
    (SELECT date, time, m2kwh, jd \
        FROM tbl_energy_usage ) y INNER JOIN \
    (SELECT date, time, m2kwh, jd \
        FROM tbl_energy_usage ) dby \
            ON dby.jd::INTEGER + 1 = y.jd::INTEGER ORDER BY y.jd desc limit 24;";
//When we make this a class the Xcode compiler turns stupid and can't determine the size of the array for definitions like this ---+
//                        +--------------------------------------------------------------------------------------------------------+
//                        |
//                        V  So we won't get fancy and leave this like it is in it's C-like configuration.
const char *ptrSQLForM2Kwh[] = { \
    "SELECT sum(c.\"Convolution\")/( (SELECT sum(d.count) " /* Provides ther fall, winter and spring M2 Energy Usage */ \
    "                               FROM (SELECT count(*) " \
    "                                     FROM  (SELECT date, time, m2kwh, date_part('DOY', date) "\
    "                                            FROM tbl_energy_usage order by date) e " \
    "                                     INNER JOIN  " \
    "                                     (SELECT date, time, m2kwh, date_part('DOY', date) " \
    "                                      FROM tbl_energy_usage order by date) f " \
    "                                     ON date_part('J', e.date)+1 = date_part('J', f.date) " \
    "                                     WHERE date_part('DOY',f.date)   NOT (BETWEEN (31+28+31+30+31) AND (31+28+31+30+31+30+31+31+15) ) " \
    "                                     GROUP BY  (f.m2kwh-e.m2kwh) " \
    "                                     ORDER BY  (f.m2kwh-e.m2kwh) ) d )   ) " \
    "FROM (SELECT  ((b.m2kwh-a.m2kwh) * count(*)) AS \"Convolution\" " \
    "      FROM  (SELECT date, time, m2kwh, date_part('DOY', date) " \
    "             FROM tbl_energy_usage order by date) a " \
    "      INNER JOIN  (SELECT date, time, m2kwh, date_part('DOY', date) " \
    "                   FROM tbl_energy_usage order by date) b " \
    "      ON date_part('J', a.date)+1 = date_part('J', b.date) " \
    "      WHERE date_part('DOY',b.date)   NOT (BETWEEN (31+28+31+30+31) AND (31+28+31+30+31+30+31+31+15) )  "  /* Gets us outside the summer season where we use the basement dehumidifier */ \
    "      GROUP BY  (b.m2kwh-a.m2kwh) " \
    "      ORDER BY  (b.m2kwh-a.m2kwh) )  c", \
    "SELECT sum(c.\"Convolution\")/( (SELECT sum(d.count)" /* Provides the Summer Season M2 Energy Usage  */ \
    "                               FROM (SELECT count(*)" \
    "                                     FROM  (SELECT date, time, m2kwh, date_part('DOY', date)" \
    "                                            FROM tbl_energy_usage order by date) e" \
    "                                     INNER JOIN" \
    "                                     (SELECT date, time, m2kwh, date_part('DOY', date)" \
    "                                      FROM tbl_energy_usage order by date) f" \
    "                                     ON date_part('J', e.date)+1 = date_part('J', f.date)" \
    "                                     WHERE date_part('DOY',f.date) BETWEEN (31+28+31+30+31) AND (31+28+31+30+31+30+31+31+15) " \
    "                                         AND date_part('Year', f.date) NOT IN (2015, 2016, 2017)" /* The summers of 2015, 2015, and 2017 had unreasonably high m2 energy usage due to broken dehumidifier*/ \
    "                                     GROUP BY  (f.m2kwh-e.m2kwh)" \
    "                                     ORDER BY  (f.m2kwh-e.m2kwh) ) d )   )" \
    "      FROM (SELECT  ((b.m2kwh-a.m2kwh) * count(*)) AS \"Convolution\" " \
    "      FROM  (SELECT date, time, m2kwh, date_part('DOY', date) " \
    "                    FROM tbl_energy_usage order by date) a "  \
    "      INNER JOIN  (SELECT date, time, m2kwh, date_part('DOY', date) " \
    "                   FROM tbl_energy_usage order by date) b " \
    "      ON date_part('J', a.date)+1 = date_part('J', b.date) " \
    "      WHERE date_part('DOY',b.date)   BETWEEN (31+28+31+30+31) AND (31+28+31+30+31+30+31+31+15) " \
    "         AND date_part('Year', b.date) NOT IN (2015, 2016, 2017) " \
    "      GROUP BY  (b.m2kwh-a.m2kwh) " \
    "      ORDER BY  (b.m2kwh-a.m2kwh) )  c"};
//
//===========================================================================================
//
const char *ptrReturnCodes[] = {"Good return code", "Secondary SQL Command Failure", "UPDATE SQL Failure", "Primary SQL Command Failure", "INSERT INTO Failure", "Primary Connect Failure", "Secondary Connect Failure"};
//
//===========================================================================================
//
const char *ptrBestCoefficients = "SELECT * FROM (SELECT (a.lt)::DATE,  sqrt((a.correl*a.correl + b.correl*b.correl + c.correl*c.correl)) AS \"ResultantVectorOfBestSetOfCorrelationCoefficients\", a.coeff AS \"BestKind0Coeff\", b.coeff AS \"BestKind1Coeff\", c.coeff AS \"BestKind2Coeff\"  FROM (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=0) a INNER JOIN (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=1) b ON a.lt::DATE=b.lt::DATE INNER JOIN  (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=2) c ON a.lt::DATE=c.lt::DATE ORDER BY  (a.correl*a.correl + b.correl*b.correl + c.correl*c.correl) desc LIMIT 1 ) e WHERE lt::DATE = e.lt::DATE;";
///Using a vector based concept for determining the best set of polynomial coefficients by using the three (the correlation coefficient for m1m2kwh, the correlation coefficient for m1kwn, and the correlation coefficient for m2kwh) correlation coefficients associated with each of the three meter readings. This array of SQL SELECT commands is an improvement upon the array of SQL SELECT commands presented by the array named `bestKind`, below, because that criteria was driven solely by the m1m2kwh (kind=0) readings whereas this takes into account all three meter readings.
const char *ptrBestCoefficientsArray[] = {"SELECT e.\"BestKind0Coeff\"[1], e.\"BestKind0Coeff\"[2], e.\"BestKind0Coeff\"[3], e.\"BestKind0Coeff\"[4] FROM (SELECT * FROM (SELECT (a.lt)::DATE,  sqrt((a.correl*a.correl + b.correl*b.correl + c.correl*c.correl)) AS \"ResultantVectorOfBestSetOfCorrelationCoefficients\", a.coeff AS \"BestKind0Coeff\", b.coeff AS \"BestKind1Coeff\", c.coeff AS \"BestKind2Coeff\"  FROM (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=0) a INNER JOIN (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=1) b ON a.lt::DATE=b.lt::DATE INNER JOIN  (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=2) c ON a.lt::DATE=c.lt::DATE ORDER BY  (a.correl*a.correl + b.correl*b.correl + c.correl*c.correl) desc LIMIT 1 ) e WHERE lt::DATE = e.lt::DATE) e;", \
    "SELECT e.\"BestKind1Coeff\"[1], e.\"BestKind1Coeff\"[2], e.\"BestKind1Coeff\"[3], e.\"BestKind1Coeff\"[4] FROM (SELECT * FROM (SELECT (a.lt)::DATE,  sqrt((a.correl*a.correl + b.correl*b.correl + c.correl*c.correl)) AS \"ResultantVectorOfBestSetOfCorrelationCoefficients\", a.coeff AS \"BestKind0Coeff\", b.coeff AS \"BestKind1Coeff\", c.coeff AS \"BestKind2Coeff\"  FROM (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=0) a INNER JOIN (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=1) b ON a.lt::DATE=b.lt::DATE INNER JOIN  (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=2) c ON a.lt::DATE=c.lt::DATE ORDER BY  (a.correl*a.correl + b.correl*b.correl + c.correl*c.correl) desc LIMIT 1 ) e WHERE lt::DATE = e.lt::DATE) e;", \
    "SELECT e.\"BestKind2Coeff\"[1], e.\"BestKind2Coeff\"[2], e.\"BestKind2Coeff\"[3], e.\"BestKind2Coeff\"[4] FROM (SELECT * FROM (SELECT (a.lt)::DATE,  sqrt((a.correl*a.correl + b.correl*b.correl + c.correl*c.correl)) AS \"ResultantVectorOfBestSetOfCorrelationCoefficients\", a.coeff AS \"BestKind0Coeff\", b.coeff AS \"BestKind1Coeff\", c.coeff AS \"BestKind2Coeff\"  FROM (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=0) a INNER JOIN (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=1) b ON a.lt::DATE=b.lt::DATE INNER JOIN  (SELECT lt, correl, coeff FROM tbl_poly_fit_with_cov where kind=2) c ON a.lt::DATE=c.lt::DATE ORDER BY  (a.correl*a.correl + b.correl*b.correl + c.correl*c.correl) desc LIMIT 1 ) e WHERE lt::DATE = e.lt::DATE) e;"};
const int numberOfArrayElements = sizeof(ptrBestCoefficientsArray)/sizeof(ptrBestCoefficientsArray[0]);
//
//===========================================================================================
//
const char *bestKind[] = {"SELECT coeff[1], coeff[2], coeff[3], coeff[4]  FROM tbl_poly_fit_with_cov WHERE lt::DATE IN (SELECT lt::DATE from tbl_poly_fit_with_cov where abs(correl)::NUMERIC = ( SELECT max(abs(correl)::NUMERIC ) FROM tbl_poly_fit_with_cov where kind=0)) AND kind=0", \
    "SELECT coeff[1], coeff[2], coeff[3], coeff[4]  FROM tbl_poly_fit_with_cov WHERE lt::DATE IN (SELECT lt::DATE from tbl_poly_fit_with_cov where abs(correl)::NUMERIC = ( SELECT max(abs(correl)::NUMERIC ) FROM tbl_poly_fit_with_cov where kind=0)) AND kind=1", \
    "SELECT coeff[1], coeff[2], coeff[3], coeff[4]  FROM tbl_poly_fit_with_cov WHERE lt::DATE IN (SELECT lt::DATE from tbl_poly_fit_with_cov where abs(correl)::NUMERIC = ( SELECT max(abs(correl)::NUMERIC ) FROM tbl_poly_fit_with_cov where kind=0)) AND kind=2"};
Oid bestKindTypes[] = {INT2OID};
int paramLengthsBestKind[] = {2};
int paramFormatsBestKind[] = {1};
int paramValuesBestKind[1];  // Kind is either 0, 1, or 2 in NetworkByteOrder (0 looks like x'0000', 1 looks like x'0100', 2 looks like x'0200'_

short nbo[] = {0x0000, 0x0100, 0x0200}; //A by-hand rendering of the numbers 0, 1, and 2 into network byte orderer.
short *ptrnbo = &nbo[0];
//===========================================================================================
//
const char *sql_INSERT_INTO_prototype = "INSERT INTO tbl_modeled_energy_usage (date, time, temperature, siteid, mm1kwh, mm2kwn, mm1m2kwh) VALUES($1, $2, $3, $4,$5, $6, $7);";
Oid insertIntoTypes[] = {TEXTOID, TEXTOID, FLOAT4OID, INT2OID, FLOAT4OID,FLOAT4OID, FLOAT4OID};
int paramLengthsForInsert[] = {0,0,4,2,4,4,4};
int paramFormatsForInsert[] = {0,0,1,1,1,1,1};
char *paramValuesForInsert[7];  //Values are in network byte order, NBO, which implies BigEndian Format (high-ordered bytes are the left-most bytes).
//===========================================================================================
 
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
const char *moass[] = {"SELECT DISTINCT coeff[1], coeff[2], coeff[3], coeff[4] FROM tbl_poly_fit_with_cov WHERE kind=0 AND abs(correl) = (SELECT max(abs(correl)) FROM tbl_poly_fit_with_cov where kind=0)", \
    "SELECT DISTINCT coeff[1], coeff[2], coeff[3], coeff[4] FROM tbl_poly_fit_with_cov WHERE kind=1 AND abs(correl) = (SELECT max(abs(correl)) FROM tbl_poly_fit_with_cov where kind=1)", \
    "SELECT DISTINCT coeff[1], coeff[2], coeff[3], coeff[4] FROM tbl_poly_fit_with_cov WHERE kind=2 AND abs(correl) = (SELECT max(abs(correl)) FROM tbl_poly_fit_with_cov where kind=2)"};

const char *simpleMoass = "SELECT DISTINCT coeff[1], coeff[2], coeff[3], coeff[4] FROM tbl_poly_fit_with_cov) WHERE kind=$1 AND abs(correl) = (SELECT max(abs(correl)) FROM tbl_poly_fit_with_cov where kind=$2 AND NOT (coeff[1] = 'NaN' OR coeff[1] = 'NaN' OR coeff[3] = 'NaN'  OR coeff[4] = 'NaN' OR correl = 'NaN') )";

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

Oid paramTypes1[] = {TEXTOID,TEXTOID, TEXTOID,TEXTOID, TEXTOID,TEXTOID};
// char *param_values[2]={0,0};
//static const char* const static_ArrayOfConnectKeyWords[] = {"dbname", "port", "user", "password", "host" };
//static const char* const static_ArrayOfValues[] = {"LocalWeather", "5436", "cjc", NULL, "localhost"};

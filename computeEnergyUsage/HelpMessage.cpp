//
//  HelpMessage.cpp
//  computeEnergyUsage
//
//  Created by Clifford Campo on 7/26/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
extern const char *help_Message[];
extern const char *BFM;
extern int nOE;
const char *BFM = " ================================================================================================================================================================== ";
const char *help_Message[] = {"\nThis program, ", \
    ",\nis used to obtain a mathematical polynomial function of energy usage wrt temperature with data obtained from the PostgreSQL database, \n", \
    "named LocalWeather, using table, tbl_energy_usage. \nThe command line parameters for this program are as follows:", \
    "\n\t--dbname or -D: provides the name of the database [[default `LocalWeather`]] which is the same for the primay and secondary connections;", \
    "\n\t--help or -h: provides this help message;", \
    "\n\t--userid or -U: provides the database name's userid [[default `cjc`]] which is the same for the primay and secondary connections;", \
    "\n\t--hostid1 or -o: provides the IP address of the database server [[default `localhost`]] for the primary connection;", \
    "\n\t--hostid2 or -O: provides the IP address of the database server [[default `localhost`]] for the seconday connection;", \
    "\n\t--port1 or -p: provides the database's port id [[default `5436`]] for the primary connection; ", \
    "\n\t--port2 or -P: provides the database's port id [[default `5435`]] for the secondary connection; ", \
    "\n\t--kelvin or -k: if set then we will be processing temperature data on the Kelvin scale. Default [[ NOT set, using Farenheit scale]];", \
    "\n\t--debug1 or -1: provides debugging output of intermediate results in the main.cpp module;", \
    "\n\t--debug2 or -2: provides debugging output of the intermediate results in the selectFromTable.cpp module; this is where the gsl magic happens;", \
    "\n\t--debug3 or -3: provides debugging output of the intermediate results in the insertBucketizedEnergyVariablesIntoTable.cpp module;", \
    "\n\t--debug4 or -4: when present in command line provides the capability of recording ostringstream performance data. Default off.", \
    "\n\t--sql or -S: provides SQL statement [[ ***no default***, test purposes only ]];", \
    "\n\t--col or -t: the presence of the `-t` switch indicates that we should use multi-thread, rather than single thread processing techniques. ; ", \
    "\n\t THIS MULTI-THREAD CAPABILIY HAS NOT YET BEEN IMPLEMENTED!"
    "\n\t--dateTimeFile or -f: provides the name of a text file, in csv format, where we obtain the start date&time and the end date&time. \n", \
    "      The presence of this file negates and overrides any parameters supplied by the -s and -e command line switches. Each record [line] in \n", \
    "      this file consists of four fields separated using the '|' character: \n", \
    "          a) an id field which contains a numeric field. If this field starts with a `#` character then the line is considered a comment an is ignored;\n", \
    "          b) the start time field in `YYYY-MM-DD hh:mm:ss` format;\n", \
    "          c) the end time field also in `YYYY-MM-DD hh:mm:ss` format which must be greater than the value in the start time field;\n", \
    "          d) the site id which is a decimal nember as defined by the siteid field in table tbl_site_names of port 5436's LocalWeather database.\n", \
    "              NB: In the future, consider adding a fifth and sixth field  so that each record is self contained and can  specify the port and hostid \n", \
    "              along with the start date&time, the end date&time and the numeric siteid.", \
    "\n\t--siteID or -i: provides, for the secondary connection, the siteid as defined in the primary database's LocalWeather's table, tbl_site_names.\n" \
    "      This is a decimal numeric value presented on the command line as a character string;",\
    "\n\t--startDateTime or -s: provides the start date&time of the period of energy use; Format is YYYY-MM-DD hh:mm:ss; MANDATORY, no default;",\
    "\n\t--endDateTime or -e: provides the end date&time of the period of energy use; Format is YYYY-MM-DD hh:mm:ss; MANDATORY, no default; \n"
};
int nOE = sizeof( help_Message )/sizeof(BFM);

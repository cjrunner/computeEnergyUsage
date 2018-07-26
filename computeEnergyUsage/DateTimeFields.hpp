//
//  DateTimeFields.h
//  computeEnergyUsage
//
//  Created by Clifford Campo on 7/23/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef DateTimeFields_h
#define DateTimeFields_h
class DateTimeFields {
    //address name   offset
public:
    char tick0;  //00
    char y0;     //01
    char y1;     //02
    char y2;     //03
    char y3;     //04
    char dash1;  //05
    char M0;     //06
    char M1;     //07
    char dash2;  //08
    char D0;     //09
    char D1;     //10
    char tick1;  //11
    char null0;  //12
//======================Date ^^^^^^^^ Field
//======================Time VVVVVVVV Field
    char tick2;  //13
    char h0;     //14
    char h1;     //15
    char colon0; //16
    char m0;     //17
    char m1;     //18
    char colon1; //19
    char s0;     //20
    char s1;     //21
    char tick3;  //22
    char null1;  //23
    DateTimeFields(void);
    ~DateTimeFields(void);
};

#endif /* DateTimeFields_h */

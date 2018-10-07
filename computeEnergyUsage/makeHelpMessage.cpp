//
//  MakeHelpMessage.cpp
//  energyUsageWithoutThe_PQXX_Bullshit
//
//  Created by Clifford Campo on 5/17/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//
#include <iostream>

#include "makeHelpMessage.hpp"
extern int nOE;
extern const char *help_Message[];
extern const char *BFM;
MakeHelpMessage::MakeHelpMessage(const char *p) {
    this->numberOfEntries = nOE;
    this->helpMessageArray = &help_Message[0];
    yourMessage = this->message;
    programName = p;
    yourProgramName = p;
    this->ptrToMessage = NULL;
}
MakeHelpMessage::~MakeHelpMessage() {
    std::cout << "Line " << __LINE__ << " of file " << __FILE__  << ", In MakeHelpMessage destructor" << std::endl;
    if (this->ptrToMessage != nullptr) {
        delete[] (this->ptrToMessage); //If we acquired storage, clean up the acquired storage
        this->ptrToMessage = nullptr;
    }
}
char *MakeHelpMessage::createMessage (const char *nameOfThisProgram) { 
    long lengthOfHelpMessage = strlen(nameOfThisProgram);
    for (int i = 0; i < this->numberOfEntries; i++) {
        lengthOfHelpMessage += strlen(this->helpMessageArray[i]);
    }
    lengthOfHelpMessage  += 2*(strlen(BFM));
    

    this->ptrToMessage = new char[lengthOfHelpMessage];
    *(this->ptrToMessage + 0) = NULL; //We need to do this so strcat instruction will start moving charactersi into the beginning of our dynamically acquired buffer, otherwise, if the first character is not NULL, strcat will go searching for the first NULL character it encounters and will start putting the characters of our message there. This means, at a minimum, that our message will begin with som number of junk character followed by our message. At worst we could over-run our allocated memory space and cause other more sever problems, like abnormal termination. NB: we could have done memcpy or strcpy, rather than strcat, and avoided all this complexity.
    std::strcat(this->ptrToMessage, BFM);
    std::strcat(this->ptrToMessage, helpMessageArray[0]);
    std::strcat(this->ptrToMessage, nameOfThisProgram);
    for (int i = 1; i < numberOfEntries; i++) {
       std::strcat(this->ptrToMessage, helpMessageArray[i]);
    }
    std::strcat(this->ptrToMessage, BFM);
    return this->ptrToMessage;
}


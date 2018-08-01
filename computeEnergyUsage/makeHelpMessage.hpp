//
//  helpFile.hpp
//  c++Postgres
//
//  Created by Clifford Campo on 5/1/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef helpFile_hpp
#define helpFile_hpp
extern const char *helpMessageArray[];
class MakeHelpMessage {
private:
    
    const char *programName; //Name of the executable program as obtained, via rvalue *(argv+0), from the command line
    char *ptrToMessage; //Points to the calloced memory where we constructed the help message.
    /* =========================================================== */
    const char *const message =  "==================================================================";
    /* =========================================================== */
public:
    int numberOfEntries;
    const char **helpMessageArray;
    const char *yourMessage;
    const char *yourProgramName;
    char *acquiredStorage;
    
    MakeHelpMessage(const char *);
    ~MakeHelpMessage();
    char *createMessage (const char *);

    
};


#endif /* helpFile_hpp */


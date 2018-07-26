//
//  varyingType.hpp
//  storageOrderBigEndianLittleEndian
//
//  Created by Clifford Campo on 6/16/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef varyingType_hpp
#define varyingType_hpp

#include <stdio.h>
#include <iostream>


template<class T> //Class T can be long double (80-bits), double (64 bits), float (32 bits), integer (32 bits), short integer (16 bits).
class VaryingType {
private:
    size_t szofit = sizeof(T); //Constructor sets this to the size of our template-defining type, `T`.
public:
    union {
        T objOfInterest;
        T d64;
        uint8_t  arr[sizeof(T)];  //An Array of size sizeof(T), eight-bit bytes.
    } in64;
    union {
        T objOfInterest;
        T d64;
        uint8_t  arr[sizeof(T)];  //An Array of sizeof(T), eight-bit, bytes.
    } out64;
    VaryingType( void );
    ~VaryingType( void );
    void toNetworkByteOrder(void );
};

template<class T>
VaryingType<T>::VaryingType(void ) {
    szofit = sizeof(T);
    in64.d64 = 0;
    out64.d64 = 0;

}
template<class T>
VaryingType<T>::~VaryingType(void) {
    ;
}
template<class T>
void VaryingType<T>::toNetworkByteOrder(void ) {
    const int szof = (int)(szofit - 1);
    for (int i=0; i<=szof; i++) {
        this->out64.arr[szof-i] = this->in64.arr[i]; //This works!
    }
}

#endif /* varyingType_hpp */



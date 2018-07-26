//
//  varyingType.cpp
//  storageOrderBigEndianLittleEndian
//
//  Created by Clifford Campo on 6/16/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#include "varyingType.hpp"
/*
template<class T>
VaryingType<T>::VaryingType(void ) {
    szofit = sizeof(T);
    in64.d64 = 0;
    out64.d64 = 0;
}
template<class T>
VaryingType<T>::~VaryingType(void) {
    
}
template<class T>
void VaryingType<T>::toNetworkByteOrder(void ) {
    const int szof = szofit - 1;
    for (int i=0; i<=szof; i++) {
        this->out64.arr[szof-i] = this->in64.arr[i]; //This works!
    }
}
*/

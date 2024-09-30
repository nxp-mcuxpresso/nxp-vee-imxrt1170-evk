/*
 * C
 *
 * Copyright 2018-2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "sni.h"

jfloat Java_com_microej_core_tests_MicroejCoreValidation_testFloat(jfloat a,
        jfloat b){
    return a * b;
}

jdouble Java_com_microej_core_tests_MicroejCoreValidation_testDouble(jdouble a,
        jdouble b){
    return a * b;
}

jint Java_com_microej_core_tests_MicroejCoreValidation_testNativeArguments01(
        jint i1, jint i2, jint i3, jint i4, jint i5, jint i6, jint i7,
        jint i8, jint i9, jint i10){
    if(i1==0x01020304 &&
            i2==0x05060708 &&
            i3==0x090A0B0C &&
            i4==0x0D0E0F10 &&
            i5==0x11121314 &&
            i6==0x15161718 &&
            i7==0x191A1B1C &&
            i8==0x1D1E1F20 &&
            i9==0x21222324 &&
            i10==0x25262728){
        return 0x292A2B2C;
    }
    else {
        return 0;
    }
}

jlong Java_com_microej_core_tests_MicroejCoreValidation_testNativeArguments02(
        jlong l1, jlong l2, jlong l3, jlong l4, jlong l5, jlong l6, jlong l7,
        jlong l8, jlong l9, jlong l10){
    if(l1==0x2D2E2F3031323334ll &&
            l2==0x35363738393A3B3Cll &&
            l3==0x3D3E3F4041424344ll &&
            l4==0x45464748494A4B4Cll &&
            l5==0x4D4E4F5051525354ll &&
            l6==0x55565758595A5B5Cll &&
            l7==0x5D5E5F6061626364ll &&
            l8==0x65666768696A6B6Cll &&
            l9==0x6D6E6F7071727374ll &&
            l10==0x75767778797A7B7Cll){
        return 0x7D7E7F8081828384ll;
    }
    else {
        return 0ll;
    }
}

jlong Java_com_microej_core_tests_MicroejCoreValidation_testNativeArguments03(
        jint i1, jlong l2, jint i3, jlong l4, jint i5, jlong l6, jint i7,
        jlong l8, jint i9, jlong l10){
    if(i1==0x85868788 &&
            l2==0x898A8B8C8D8E8F90ll &&
            i3==0x91929394 &&
            l4==0x95969798999A9B9Cll &&
            i5==0x9D9E9FA0 &&
            l6==0xA1A2A3A4A5A6A7A8ll &&
            i7==0xA9AAABAC &&
            l8==0xADAEAFB0B1B2B3B4ll &&
            i9==0xB5B6B7B8 &&
            l10==0xB9BABBBCBDBEBFC0ll){
        return 0xC1C2C3C4C5C6C7C8ll;
    }
    else {
        return 0ll;
    }
}

jfloat Java_com_microej_core_tests_MicroejCoreValidation_testNativeArguments04(
        jfloat f1, jfloat f2, jfloat f3, jfloat f4, jfloat f5, jfloat f6,
        jfloat f7, jfloat f8, jfloat f9, jfloat f10){
    if(f1==1.0f &&
            f2==1.1f &&
            f3==1.2f &&
            f4==1.3f &&
            f5==1.4f &&
            f6==1.5f &&
            f7==1.6f &&
            f8==1.7f &&
            f9==1.8f &&
            f10==1.9f){
        return 2.0f;
    }
    else {
        return 0.0f;
    }
}

jdouble Java_com_microej_core_tests_MicroejCoreValidation_testNativeArguments05(
        jdouble d1, jdouble d2, jdouble d3, jdouble d4, jdouble d5, jdouble d6,
        jdouble d7, jdouble d8, jdouble d9, jdouble d10){
    if(d1==2.0 &&
            d2==2.1 &&
            d3==2.2 &&
            d4==2.3 &&
            d5==2.4 &&
            d6==2.5 &&
            d7==2.6 &&
            d8==2.7 &&
            d9==2.8 &&
            d10==2.9){
        return 3.0;
    }
    else {
        return 0.0;
    }
}

jdouble Java_com_microej_core_tests_MicroejCoreValidation_testNativeArguments06(
        jfloat f1, jdouble d2, jfloat f3, jdouble d4, jfloat f5, jdouble d6,
        jfloat f7, jdouble d8, jfloat f9, jdouble d10){
    if(f1==3.0f &&
            d2==3.1 &&
            f3==3.2f &&
            d4==3.3 &&
            f5==3.4f &&
            d6==3.5 &&
            f7==3.6f &&
            d8==3.7 &&
            f9==3.8f &&
            d10==3.9){
        return 4.0;
    }
    else {
        return 0.0;
    }
}

/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include "TestCNatives_impl.h"
#include "LLEVENT.h"
#include "sni.h"


#ifdef __cplusplus
extern "C" {
#endif

struct testOfferExtendedBoolean {
	jboolean a;
	jboolean b;
	jboolean c;
	jboolean d;
	jboolean e;
	jboolean f;
};

struct testOfferExtendedByte {
	jbyte a;
	jbyte b;
	jbyte c;
	jbyte d;
	jbyte e;
	jbyte f;
};

struct testOfferExtendedChar {
	jchar a;
	jchar b;
	jchar c;
};

struct testOfferExtendedDouble {
	jdouble a;
	jdouble b;
	jdouble c;
};

struct testOfferExtendedFloat {
	jfloat a;
	jfloat b;
	jfloat c;
};

struct testOfferExtendedInt {
	jint a;
	jint b;
	jint c;
	jint d;
};

struct testOfferExtendedLong {
	jlong a;
	jlong b;
	jlong c;
};

struct testOfferExtendedShort {
	jshort a;
	jshort b;
	jshort c;
};

struct testOfferExtendedUnsignedByte {
	jboolean a;
	jboolean b;
	jboolean c;
};

struct testOfferExtendedUnsignedShort {
	jchar a;
	jchar b;
	jchar c;
};

struct testOfferExtendedAlignment {
	jbyte a;
	jint b;
	jchar c;
	jdouble d;
	jboolean e;
	jchar f;
	jfloat g;
	jboolean h;
	jlong i;
};

struct testNotConsumeAllEventBytes {
    jbyte a;
    jbyte b;
    jbyte c;
    jbyte d;
    jbyte e;
    jbyte f;
    jbyte g;
    jbyte h;
    jbyte i;
};

int32_t Java_ej_event_utils_TestCNatives_offerEventCApi(uint32_t type, int32_t data) {
	return LLEVENT_offerEvent(type, data);
}

int32_t Java_ej_event_utils_TestCNatives_offerExtendedEventCApi(int32_t type, int8_t* data, uint32_t data_length){
	return LLEVENT_offerExtendedEvent(type, data, data_length);
}

void Java_ej_event_utils_TestCNatives_offerExtendedBoolean(int32_t type){
	struct testOfferExtendedBoolean pt;
	pt.a = JTRUE;
	pt.b = JFALSE;
	pt.c = JTRUE;
	pt.d = JTRUE;
	pt.e = JFALSE;
	pt.f = JTRUE;
	LLEVENT_offerExtendedEvent(type, (void*)&pt, sizeof(pt));
}

void Java_ej_event_utils_TestCNatives_offerExtendedByte(int32_t type){
	struct testOfferExtendedByte pt;
	pt.a = 1;
	pt.b = 2;
	pt.c = 3;
	pt.d = 4;
	pt.e = 5;
	pt.f = -6;
	LLEVENT_offerExtendedEvent(type, (void*)&pt, sizeof(pt));
}

void Java_ej_event_utils_TestCNatives_offerExtendedChar(int32_t type){
	struct testOfferExtendedChar pt;
	pt.a = 'a';
	pt.b = 'b';
	pt.c = 'c';
	LLEVENT_offerExtendedEvent(type, (void*)&pt, sizeof(pt));
}

void Java_ej_event_utils_TestCNatives_offerExtendedDouble(int32_t type){
	struct testOfferExtendedDouble pt;
	pt.a = 1.54;
	pt.b = 3.2;
	pt.c = -7.35;
	LLEVENT_offerExtendedEvent(type, (void*)&pt, sizeof(pt));
}

void Java_ej_event_utils_TestCNatives_offerExtendedFloat(int32_t type){
	struct testOfferExtendedFloat pt;
	pt.a = 1.54f;
	pt.b = 3.2f;
	pt.c = -7.35f;
	LLEVENT_offerExtendedEvent(type, (void*)&pt, sizeof(pt));
}

void Java_ej_event_utils_TestCNatives_offerExtendedInt(int32_t type){
	struct testOfferExtendedInt pt;
	pt.a = 1;
	pt.b = 2;
	pt.c = 3;
	pt.d = -6;
	LLEVENT_offerExtendedEvent(type, (void*)&pt, sizeof(pt));
}

void Java_ej_event_utils_TestCNatives_offerExtendedLong(int32_t type){
	struct testOfferExtendedLong pt;
	pt.a = 0x100000000;
	pt.b = 0x200000000;
	pt.c = -0x300000000;
	LLEVENT_offerExtendedEvent(type, (void*)&pt, sizeof(pt));
}

void Java_ej_event_utils_TestCNatives_offerExtendedShort(int32_t type){
	struct testOfferExtendedShort pt;
	pt.a = 1;
	pt.b = 2;
	pt.c = -6;
	LLEVENT_offerExtendedEvent(type, (void*)&pt, sizeof(pt));
}

void Java_ej_event_utils_TestCNatives_offerExtendedUnsignedByte(int32_t type){
	struct testOfferExtendedUnsignedByte pt;
	pt.a = 128;
	pt.b = 129;
	pt.c = 130;
	LLEVENT_offerExtendedEvent(type, (void*)&pt, sizeof(pt));
}

void Java_ej_event_utils_TestCNatives_offerExtendedUnsignedShort(int32_t type){
	struct testOfferExtendedUnsignedShort pt;
	pt.a = 32768;
	pt.b = 32769;
	pt.c = 32770;
	LLEVENT_offerExtendedEvent(type, (void*)&pt, sizeof(pt));
}

void Java_ej_event_utils_TestCNatives_testAlignment(int32_t type){
	struct testOfferExtendedAlignment pt;
	pt.a = 1;
	pt.b = 42;
	pt.c = 'e';
	pt.d = 4.45;
	pt.e = JTRUE;
	pt.f = 32769;
	pt.g = 5.42f;
	pt.h = 130;
	pt.i = 0x300000000;
	LLEVENT_offerExtendedEvent(type, (void*)&pt, sizeof(pt));
}

void Java_ej_event_utils_TestCNatives_testNotConsumeAllEventBytes(int32_t type){
    struct testNotConsumeAllEventBytes pt1;
    pt1.a = 4;
    pt1.b = 5;
    pt1.c = 6;
    pt1.d = 7;
    pt1.e = 8;
    pt1.f = 9;
    pt1.g = 10;
    pt1.h = 11;
    pt1.i = 12;
    LLEVENT_offerExtendedEvent(type, (void*)&pt1, sizeof(pt1));
    struct testOfferExtendedByte pt2;
    pt2.a = 1;
    pt2.b = 2;
    pt2.c = 3;
    pt2.d = 4;
    pt2.e = 5;
    pt2.f = -6;
    LLEVENT_offerExtendedEvent(type, (void*)&pt2, sizeof(pt2));
}




#ifdef __cplusplus
}
#endif

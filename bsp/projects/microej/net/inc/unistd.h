/*
 * C
 *
 * Copyright 2017-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef __UNISTD_H
#define __UNISTD_H

/**
 * @file
 * @brief BSD-like API stub.
 * @author MicroEJ Developer Team
 * @version 0.1.0
 * @date 21 December 2017
 */

// Not implemented: LLNET_NETWORKADDRESS implementation will use a default hostname.
#define gethostname(name, len)	(-1)

#endif // __UNISTD_H

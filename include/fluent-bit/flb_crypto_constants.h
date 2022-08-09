/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2019-2022 The Fluent Bit Authors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef FLB_CRYPTO_CONSTANTS_H
#define FLB_CRYPTO_CONSTANTS_H

#include <openssl/opensslv.h>

#ifndef OPENSSL_VERSION_PREREQ
#define FLB_CRYPTO_OPENSSL_LEGACY_MODE
#else
#if OPENSSL_VERSION_NUMBER < OPENSSL_VERSION_PREREQ(3, 0)
#define FLB_CRYPTO_OPENSSL_LEGACY_MODE
#endif
#endif


#define FLB_CRYPTO_SUCCESS            0
#define FLB_CRYPTO_BACKEND_ERROR      1
#define FLB_CRYPTO_INVALID_STATE      2
#define FLB_CRYPTO_INVALID_ARGUMENT   3

#define FLB_CRYPTO_PUBLIC_KEY         1
#define FLB_CRYPTO_PRIVATE_KEY        2

#define FLB_CRYPTO_PADDING_NONE       0
#define FLB_CRYPTO_PADDING_PKCS1      1
#define FLB_CRYPTO_PADDING_PKCS1_OEAP 2
#define FLB_CRYPTO_PADDING_PKCS1_X931 3
#define FLB_CRYPTO_PADDING_PKCS1_PSS  4

#define FLB_DIGEST_NONE               0
#define FLB_DIGEST_MD5                1
#define FLB_DIGEST_SHA256             2
#define FLB_DIGEST_SHA512             3

#define FLB_CRYPTO_OPERATION_NONE     0
#define FLB_CRYPTO_OPERATION_SIGN     1
#define FLB_CRYPTO_OPERATION_ENCRYPT  2
#define FLB_CRYPTO_OPERATION_DECRYPT  3

#endif
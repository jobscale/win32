///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2007 Plustar Corporation. All rights reserved. プラスター

#ifndef __BASE64_H__
#define __BASE64_H__

#include <string.h>

size_t base64_needed_encoded_length(size_t length_of_data);
size_t base64_needed_decoded_length(size_t length_of_encoded_data);
size_t base64_encode(const void *src, size_t src_len, char *dst, int tbl = 1);
size_t base64_decode(const char *src, size_t size, void *dst, int tbl = 1);

#endif // __BASE64_H__

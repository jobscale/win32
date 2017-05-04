///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2007 Plustar Corporation. All rights reserved.

#ifndef _AFXDLL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#else
#include <afxwin.h>
#endif

#include <math.h>

#include "base64.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//

// 暗号キー

static const char* base64_table_select[] = {
    "\0 ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/ \0 =" + 2, // 0 非暗号
    "\0 PLUSTAR+Version6/plutaHmO8w4Ky0CfNc2GbFXvYxJ3EjWzQd1Mg5hI7B9kDqZ \0 %" + 2, // 1 ネットワーク通信暗号
    "\0 plutaHmO8w4Ky0CfNc2GbFXvYxJ3EjWzQd1Mg5hI7B9kDqZ/PLUSTAR+Version6 \0 &" + 2, // 2 ライセンスファイル暗号１
    "\0 GbFXvYxJ3EjWzQd1Mg5plutaHmO8w4Ky0Cf/PLUSTAR+Version69kDqZNc2hI7B \0 ?" + 2, // 3 ライセンスファイル暗号２
	"\0 HLONo2lWKy0CwP7fimcGb3trFsXVYxJEjU6zQd1MghIB9Rkp/nDS+eTuavZq85A4 \0 @" + 2, // 4 サーバ通信暗号
	"\0 /+lJEjU6zQd1MghIB9RkpnDSeTuao2WKy0CwP7vZq85A4HLONfimcGb3trFsXVYx \0 !" + 2, // 5 ローカル保存暗号
  };

static const char* base64_change_table(int select)
{
  return base64_table_select[select];
}

size_t base64_needed_encoded_length(size_t length_of_data)
{
  return (length_of_data + 2) / 3 * 4 + 1;
}

size_t base64_needed_decoded_length(size_t length_of_encoded_data)
{
  return (int)ceil((double)length_of_encoded_data * 3 / 4);
}

/*
  Encode a data as base64.

  Note: We require that dst is pre-allocated to correct size.
        See base64_needed_encoded_length().
*/

size_t base64_encode(const void *src, size_t src_len, char *dst, int tbl/* = 1*/)
{
	const char* base64_table = (char*)base64_change_table(tbl);

  const unsigned char *s= (const unsigned char*)src;
  size_t i= 0;

  for (; i < src_len; )
  {
    unsigned c = s[i++];
    c <<= 8;

    if (i < src_len)
      c += s[i];
    c <<= 8;
    i++;

    if (i < src_len)
      c += s[i];
    i++;

    *dst++= base64_table[(c >> 18) & 0x3f];
    *dst++= base64_table[(c >> 12) & 0x3f];

    if (i > (src_len + 1))
      *dst++ = base64_table[67];
    else
      *dst ++= base64_table[(c >> 6) & 0x3f];

    if (i > src_len)
      *dst++ = base64_table[67];
    else
      *dst ++= base64_table[(c >> 0) & 0x3f];
  }
  *dst = '\0';

  return 0;
}

static inline unsigned pos(unsigned char c, int tbl)
{
	const char* base64_table = (char*)base64_change_table(tbl);
#if 0
  return strchr(base64_table, c) - base64_table;
#else
  const char* findpos = strchr(base64_table, c);
  if (findpos == NULL)
  {
	  return 0;
  }
  return (unsigned)(findpos - base64_table);
#endif
}

/*
  Decode a base64 string

  Note: We require that dst is pre-allocated to correct size.
        See base64_needed_decoded_length().

  RETURN  Number of bytes produced in dst or -1 in case of failure
*/
size_t base64_decode(const char *src, size_t size, void *dst, int tbl/* = 1*/)
{
	const char* base64_table = (char*)base64_change_table(tbl);

  char b[3];
  size_t i= 0;
  char *dst_base= (char *)dst;
  char *d= dst_base;
  size_t j;

  for (; i < size; )
  {
    unsigned c= 0;
    size_t mark= 0;
    for (; ; )
    {
      c += pos(*src++, tbl);
      c <<= 6;
      i++;

      c += pos(*src++, tbl);
      c <<= 6;
      i++;

      if (* src != base64_table[67])
        c += pos(*src++, tbl);
      else
      {
        i= size;
        mark= 2;
        c <<= 6;
        break;
      }
      c <<= 6;
      i++;

      if (*src != base64_table[67])
        c += pos(*src++, tbl);
      else
      {
        i= size;
        mark= 1;
        break;
      }
      i++;
      break;
    }

    b[0]= (c >> 16) & 0xff;
    b[1]= (c >>  8) & 0xff;
    b[2]= (c >>  0) & 0xff;

    for (j=0; j<3-mark; j++)
      *d++= b[j];
  }

  return (size_t)(d - dst_base);
}

#define require(b) { \
    if (!(b)) { \
      printf("Require failed at %s:%d\n", __FILE__, __LINE__); \
      abort(); \
    } \
  }

int sample_test()
{
  int i;
  size_t k, l;
  size_t dst_len;
  size_t needed_length;

  for (i= 0; i < 50; i++)
  {
    char src[]= "あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをん！？";
	const size_t src_len = strlen(src);
    char * str;
    char * dst;

    /* Encode */
    needed_length = base64_needed_encoded_length(src_len);
    str= (char *) malloc(needed_length);
    for (k= 0; k < needed_length; k++)
      str[k]= (unsigned char)0xff; /* Fill memory to check correct NUL termination */
    require(base64_encode(src, src_len, str) == 0);
    require(needed_length == strlen(str) + 1);
	printf("%s", str);
	printf("\n");

    /* Decode */
    dst= (char *) malloc(base64_needed_decoded_length(strlen(str)));
    dst_len= base64_decode(str, strlen(str), dst);
    require(dst_len == src_len);

    if (memcmp(src, dst, src_len) != 0)
    {
      printf("       --------- src ---------   --------- dst ---------\n");
      for (k= 0; k<src_len; k+=8)
      {
        printf("%.4x   ", (unsigned int) k);
        for (l=0; l<8 && k+l<src_len; l++)
        {
          unsigned char c= src[k+l];
          printf("%.2x ", (unsigned)c);
        }

        printf("  ");

        for (l=0; l<8 && k+l<dst_len; l++)
        {
          unsigned char c= dst[k+l];
          printf("%.2x ", (unsigned)c);
        }
        printf("\n");
      }
      printf("src length: %.8x, dst length: %.8x\n",
             (unsigned int) src_len, (unsigned int) dst_len);
	  printf("%s", dst);
      require(0);
    }
  }
  printf("Test succeeded.\n");
  return 0;
}

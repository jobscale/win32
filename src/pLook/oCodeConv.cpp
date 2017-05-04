///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#define SUBST_CHAR      0x5f;

#define issjiskanji1(c) \
        ((((c) & 0xff) >= 0x81 && ((c) & 0xff) <= 0x9f) || \
         (((c) & 0xff) >= 0xe0 && ((c) & 0xff) <= 0xfc))
#define issjiskanji2(c) \
        ((((c) & 0xff) >= 0x40 && ((c) & 0xff) <= 0x7e) || \
         (((c) & 0xff) >= 0x80 && ((c) & 0xff) <= 0xfc))
#define issjishwkana(c) \
        (((c) & 0xff) >= 0xa1 && ((c) & 0xff) <= 0xdf)

#define IS_ASCII(c) (((unsigned char) c) <= 0177 ? 1 : 0)

void sjistoeuc(char *outbuf, int outlen, const char *inbuf)
{
        const unsigned char *in = reinterpret_cast<const unsigned char *>(inbuf);
        unsigned char *out = reinterpret_cast<unsigned char *>(outbuf);

        while (*in != '\0') {
                if (IS_ASCII(*in)) {
                        *out++ = *in++;
                } else if (issjiskanji1(*in)) {
                        if (issjiskanji2(*(in + 1))) {
                                unsigned char out1 = *in;
                                unsigned char out2 = *(in + 1);
                                unsigned char row;

                                row = out1 < 0xa0 ? 0x70 : 0xb0;
                                if (out2 < 0x9f) {
                                        out1 = (out1 - row) * 2 - 1;
                                        out2 -= out2 > 0x7f ? 0x20 : 0x1f;
                                } else {
                                        out1 = (out1 - row) * 2;
                                        out2 -= 0x7e;
                                }

                                *out++ = out1 | 0x80;
                                *out++ = out2 | 0x80;
                                in += 2;
                        } else {
                                *out++ = SUBST_CHAR;
                                in++;
                                if (*in != '\0' && !IS_ASCII(*in)) {
                                        *out++ = SUBST_CHAR;
                                        in++;
                                }
                        }
                } else if (issjishwkana(*in)) {
                        *out++ = 0x8e;
                        *out++ = *in++;
                } else {
                        *out++ = SUBST_CHAR;
                        in++;
                }
        }

        *out = '\0';
}

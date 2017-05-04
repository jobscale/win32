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

/*
    MIME Decord Sub routine
 */

// 漢字コードの種別を表す定数
#define ASCII    0x00
#define EUC      0x01
#define SJIS     0x02
#define JIS      0x04
#define JAPANESE 0xff

typedef unsigned char u_char; 

char mimehead[] = "=?ISO-2022-JP?B?";
char mimehead2[] = "=?iso-2022-jp?B?";
char reverse64[256];
char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// 初期化
int MIMEInit(void)
{
  int i;
  for (i=0;i<(int)strlen(base64);i++)
      reverse64[base64[i]] = i;
  return 0;
}

static int isOK = MIMEInit();

/*
     Base64 -> JISにデコードする
     (かなり手抜き^^;)
*/
void mimedec(char *in, char *out)
{
    char *p,*bp;
    char *q;
    int mimein;
    int mimebuf[4];
    int mimepos;
    p = in;

    mimein = 0;
    while(*p != (char)NULL)
    {
        if (*p == '=' && *(p+1) == '?' && mimein == 0)
        {
            mimein = 1;
            bp = p;
            p = strstr(bp, mimehead);
            q = strstr(bp, mimehead2);
            if (p == NULL && q == NULL) {
                // not Japanese MIME
                printf("not Japanese\n");
                p = bp;
                mimein = 0;
            }
			else
			{
              if (p == NULL) p = q;

              p += strlen(mimehead);
              mimepos = 0;
            }
        }
        if (mimein)
        {
            if (*p == '?' && *(p+1) == '=')
			{
                // MIME terminated.
                mimein = 0;
                p++;
            }
			else
			{
                // decode
                mimebuf[mimepos++] = reverse64[*p];
                if (mimepos == 4)
				{
                    *(out++) = (char)((mimebuf[0] << 2) | (mimebuf[1] >> 4));
                    *(out++) = (char)((mimebuf[1] << 4) | (mimebuf[2] >> 2));
                    *(out++) = (char)((mimebuf[2] << 6) | (mimebuf[3]    ));
                    mimepos = 0;
                }
            }
        }
		else
		{
            *out = *p;
            out++;
        }
        p++;
    }

    *(out+1) = (char)NULL;
}

//---- 半角カナの存在を無視した漢字コードの判定関数
static int KanjiCode(char* text)
{
	for (u_char* ptr = (u_char*)text; *ptr; ptr++)
	{
	if (*ptr == 0x1b && *(ptr+1) == '$')
		return  JIS;
	if (_ismbblead(*ptr) && _ismbbtrail(*(ptr + 1)))
		return SJIS;
	if (0xA1 <= *ptr && *ptr <= 0xDF)
		return  EUC;
	if (*ptr < 0x80)
		continue; // ASCII
	}
	return ASCII;
}

//---- JIS文字をSJIS文字に変換する関数
static inline void JIStoSJIS(u_char& knj1, u_char& knj2)
{
	if (knj1 & 0x01)
	{
	knj1 >>= 1;
	if (knj1 < 0x2F)
		knj1 += 0x71;
	else
		knj1 -= 0x4F;
	if (knj2 > 0x5F)
		knj2 += 0x20;
	else
		knj2 += 0x1F;
	}
	else
	{
	knj1 >>= 1;
	if (knj1 < 0x2F)
	{
		knj1 += 0x70;
	}
	else
	{
		knj1 -= 0x50;
	}
	knj2 += 0x7E;
	}
}
//---- JISコードをSJISコードに変換する関数
static void JIStoSJIS(char* text)
{
	int mode = ASCII;

	u_char *wr, *re;
	for (wr=re=(u_char*)text; *re; re++)
	{
	if ((re[0]=='\x1b' && re[1]=='$' && re[2] == 'B') || (re[0]=='\x1b' && re[1]=='$' && re[2] == '@'))
	{
	  re+=2; 
	  mode = JAPANESE;
	  continue;
	}
	else if ((re[0]=='\x0f') || (re[0]=='\x1b' && re[1]=='(' && re[2] == 'B') || (re[0]=='\x1b' && re[1]=='(' && re[2] == 'J'))
	{
	  re+=2; 
	  mode = ASCII;
	  continue;
	}
	else if ((re[0]=='\x0e') || (re[0]=='\x1b' && re[1]=='(' && re[2] == 'I'))
	{
	  re+=2; 
	  mode = ASCII; // hankaku IGNORE
	  continue;
	}

	if (mode == ASCII)
	{
	  *wr++ = *re;
	  continue;
	}
	*wr++ = *re;
	if (!(*wr = *++re))
	{
		break;
	}
	JIStoSJIS(*(wr-1), *wr);
	wr++;
	}
	*wr='\0';
}

//---- EUC文字をSJIS文字に変換する関数
inline void EUCtoSJIS(u_char& knj1, u_char& knj2)
{
	if (knj1 & 0x01)
	{
	knj1 >>= 1;
	if (knj1 < 0x6F)
	{
		knj1 += 0x31;
	}
	else
	{
		knj1 += 0x71;
	}
	if (knj2 > 0xDF)
	{
		knj2 -= 0x60;
	}
	else
	{
		knj2 -= 0x61;
	}
	}
	else
	{
	knj1 >>= 1;
	if (knj1 < 0x6F)
		knj1 += 0x30;
	else
		knj1 += 0x70;
	knj2 -= 0x02;
	}
}

//---- EUCコードをSJISコードに変換する関数
static void EUCtoSJIS(char* text)
{
	for (u_char* ptr=(u_char*)text; *ptr; ptr++)
	{
		if (*ptr < 0x80) continue;
		if (!(*++ptr)) break;
		EUCtoSJIS(*(ptr-1), *ptr);
	}
}

//---- 任意のコードをSJISに変換する関数
void ANYtoSJIS(char* text)
{
	switch (KanjiCode(text))
	{
	case JIS:
	  JIStoSJIS(text);
	  break;
	case EUC:
	  EUCtoSJIS(text);
	  break;
	case SJIS:
	  break;
	case ASCII:
	  break;
	}
}

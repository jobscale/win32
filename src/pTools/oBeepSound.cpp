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

#include "../oCommon/oTools.h"

#define _OFC_EXPORT_
#include "../ofc/oCriticalTrace.h"

#include "../pLook/pUtility.h"

#include <math.h>

namespace beepSound
{

#define KEY_UP    72
#define KEY_DOWN  80
#define KEY_RIGHT 77
#define KEY_LEFT  75
#define KEY_SPACE 32
#define KEY_ENTER 13
#define KEY_ESC   27
#define KEY_CTRLZ 26
#define KEY_CTRLX 24
#define KEY_CTRLC  3
#define KEY_CRATZ 44
#define KEY_CRATX 45
#define KEY_CRATC 46

const LPCTSTR soundData[] = {
//	"@TEMPO 95",
	"@TEMPO 160",
	"@OCTAVE 3",
	"@LENGTH 1",
	"@GATETIME 95",
	"a2f+ga2f+ga>ab<c+def+gf+2def+2>f+gabagaf+gagr",
	"bag2f+ef+edef+gabgrbab2<c+d>ab<c+def+ga",
	"f+2def+2edec+def+edc+d2>b<c+d2>def+gf+ef+<dc+d>br",
	"<dc+>b2agagf+gab<c+d>br<dc+d2c+>b<c+dedc+d>b<c+d8",
	NULL
};

#define LV_HZ 55.0
#define HV_HZ 32767.0
#define MAX_SECOND 30000.0
#define MAX_LEN 100
#define OCT_CNT 12 // 1オクターブの鍵盤数

typedef struct step_t{
	long hz;
	long len;
	struct step_t* np;
}STEP;

static STEP* top;
static STEP* next;

const static double keyUp = pow(2.0, (1.0/12.0));
static int task;
static long* hzArray; // 配列のように使う

static int prm_maxOct; // 基本パラメータ類
static int prm_maxLen;
static int prm_oct;
static int prm_len;
static int prm_gtime = 100; // デフォルト、入力されなかったときの対処
static int prm_defLen;

/*----------------------------*/
/*           makeHz           */
/*----------------------------*/
int makeHz(char* keyWord_c, long* hz)
{
	int keyWord_i[2];
	int oct = prm_oct;
	
	switch (keyWord_c[1])
	{
	case '+':
		keyWord_i[1] =  1;
		break;
	case '-':
		keyWord_i[1] = -1;
		break;
	case '0':
		keyWord_i[1] =  0;
		break;
	}
	switch(keyWord_c[0])
	{
	case 'a':
		++oct;
		keyWord_i[0] =  0;
		break;
	case 'b':
		++oct;
		keyWord_i[0] =  2;
		break;
	case 'c':
		keyWord_i[0] =  3;
		break;
	case 'd':
		keyWord_i[0] =  5;
		break;
	case 'e':
		keyWord_i[0] =  7;
		break;
	case 'f':
		keyWord_i[0] =  8;
		break;
	case 'g':
		keyWord_i[0] = 10;
		break;
	case 'r':
		*hz = 32767;
		return 0;
	}
	keyWord_i[0] += keyWord_i[1];
	if (keyWord_i[0] <  0)
	{
		--oct;
		keyWord_i[0] = 11;
	}
	else if (keyWord_i[0] > 11)
	{
		++oct;
		keyWord_i[0] = 0;
	}
	*hz = hzArray[oct * 12 + keyWord_i[0]];
	return 0;
}

/************************************************************/
/*                                                          */
/*                         taskInit                         */
/*                                                          */
/************************************************************/
int taskInit()
{
	int hzMax;
	int hzIdx;
	double powHz;

	// STEP* 型の変数topの領域を確保
	top = (STEP*)malloc(sizeof STEP);
	if (top == NULL)
	{
		DEBUG_OUT_DEFAULT("メモリ領域が不足しています\n");
		return -1;
	}
	else
	{
		top->np = NULL;
	}

	// LV_HZからHV_HZの間にある音階をカウント
	powHz = LV_HZ;
	for (hzMax = 0; powHz < HV_HZ ; ++hzMax)
	{
		powHz *= keyUp;
	}

	// オクターブ分(12で割り切れる数)の領域を確保
	prm_maxOct = hzMax / OCT_CNT;
	hzMax = (prm_maxOct * OCT_CNT) - 1; // -1 は、ズレ修正

	hzArray = (long*)malloc(sizeof (long) * (hzMax + 1)); // r (休止)の+1
	powHz = LV_HZ;

	// 確保した領域に実際の数字を格納
	for (hzIdx = 0; hzIdx <= hzMax; ++hzIdx)
	{
		hzArray[hzIdx] = (long)powHz;

        powHz *= keyUp;
	}
	--hzIdx; // ズレ修正

	top = (STEP*)malloc(sizeof STEP); // top に領域を確保
	top->np = NULL;

	return 0;
}

DWORD dataRead(LPTSTR buffer, DWORD size, int dp)
{
	if (soundData[dp] == NULL)
	{
		return 0;
	}
	strncpy(buffer, soundData[dp], size);
	return strlen(buffer);
}

/************************************************************/
/*                                                          */
/*                         taskRead                         */
/*                                                          */
/************************************************************/
int taskRead()
{
	char readLine[1024];
	int  lineCount;
	int  rIdx=0;
	int  commentFlag = 0;
	int  tempo;
	int  log;
	int  log_work;
	char mhWrd[2];
	int  length;

	for (lineCount = 0; dataRead(readLine, sizeof readLine, lineCount) != NULL; ++lineCount)
	{
		for (rIdx = 0; readLine[rIdx] != '\n' && readLine[rIdx] != '\0';)
		{
			if (commentFlag == 0)
			{
				switch (readLine[rIdx])
				{
				case ' ':
					++rIdx;
					break;
				case '@':
					++rIdx;
					if (strncmp(&readLine[rIdx], "TEMPO ", (int)strlen("TEMPO ")) == 0)
					{
						rIdx += (int)strlen("TEMPO ");
						tempo = atoi(&readLine[rIdx]);
						if (tempo <= 0 || 512 < tempo)
						{
							return -1;
						}
						prm_len = 60000 / (tempo * 4);
						prm_len = (prm_len * 100) / prm_gtime;
						log_work = tempo;
						for(log = 0; log_work > 0; ++log){
							log_work /= 10;
						}
						rIdx += log;
					}
					else if (strncmp(&readLine[rIdx], "OCTAVE ", (int)strlen("OCTAVE ")) == 0)
					{
						rIdx += (int)strlen("OCTAVE ");
						prm_oct = atoi(&readLine[rIdx]);
						if (prm_oct <= 0 || prm_maxOct < prm_oct)
						{
							return -1;
						}
						log_work = prm_oct;
						for(log = 0; log_work > 0; ++log){
							log_work /= 10;
						}
						rIdx += log;
					}
					else if (strncmp(&readLine[rIdx], "GATETIME ", (int)strlen("GATETIME ")) == 0)
					{
						rIdx += (int)strlen("GATETIME ");
						prm_gtime = atoi(&readLine[rIdx]);
						if (prm_gtime <= 0 || 100 < prm_gtime)
						{
							return -1;
						}
						log_work = prm_gtime;
						for(log = 0; log_work > 0; ++log){
							log_work /= 10;
						}
						rIdx += log;
					}
					else if (strncmp(&readLine[rIdx], "LENGTH ", (int)strlen("LENGTH ")) == 0)
					{
						rIdx += (int)strlen("LENGTH ");
						prm_defLen = atoi(&readLine[rIdx]);
						if (prm_defLen < 0 || MAX_LEN < prm_defLen)
						{
							return -1;
						}
						log_work = prm_defLen;
						for (log = 0; log_work > 0; ++log)
						{
							log_work /= 10;
						}
						rIdx += log;
					}
					else
					{
						return -1;
					}
					break;
				case '/':
					++rIdx;
					switch(readLine[rIdx])
					{
					case '/':
						++rIdx;
						goto break2; // switch と for をbreakする
					case '*':
						++rIdx;
						commentFlag = 1;
						break;
					default:
						return -1;
						break;
					}
					break;
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'g':
				case 'r':
					length = prm_defLen; // lengthがない場合のデフォルト値
					mhWrd[0] = readLine[rIdx];
					mhWrd[1] = '0'; // 半音がない場合
					++rIdx;
					switch(readLine[rIdx])
					{
					case '+':
					case '-':
						mhWrd[1] = readLine[rIdx];
						++rIdx;
					default:
						length = prm_defLen;
						switch(readLine[rIdx])
						{
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
							length = atoi(&readLine[rIdx]);
							log_work = length;
							for(log = 0; log_work > 0; ++log){
								log_work /= 10;
							}
							rIdx += log;
						default:
							if (length < 1  || (int)(MAX_SECOND / prm_len) < length)
							{
								return -1;
							}
							if (top->np == NULL)
							{
								next = (STEP*)malloc(sizeof STEP);
								if (next == NULL)
								{
									DEBUG_OUT_DEFAULT("メモリ不足です");
									return -1;
								}
								top->len = ((prm_len * length) * prm_gtime) / 100;
								makeHz(mhWrd, &top->hz);
								if (prm_gtime < 100)
								{
									top->np = next;
									next->np = (STEP*)malloc(sizeof STEP);
									if (next->np == NULL)
									{
										DEBUG_OUT_DEFAULT("メモリ不足です");
										return -1;
									}
									next->len = ((prm_len * length) * (100 - prm_gtime)) / 100;
									mhWrd[0] = 'r';
									mhWrd[1] = '0';
									makeHz(mhWrd, &next->hz);
									next = next->np;
								}
								else{
									top->np = next;
								}
							}
							else{
								next->np = (STEP*)malloc(sizeof STEP);
								if (next->np == NULL)
								{
									DEBUG_OUT_DEFAULT("メモリ不足です");
									return -1;
								}
								next->len = ((prm_len * length) * prm_gtime) /100;
								makeHz(mhWrd, &next->hz);
								if (prm_gtime < 100)
								{
									next = next->np;
									next->np = (STEP*)malloc(sizeof STEP);
									if (next->np == NULL)
									{
										DEBUG_OUT_DEFAULT("メモリ不足です");
										return -1;
									}
									next->len = ((prm_len * length) * (100 - prm_gtime)) / 100;
									mhWrd[0] = 'r';
									mhWrd[1] = '0';
									makeHz(mhWrd, &next->hz);
								}
								next = next->np;
							}
							break;
						}
						break;
					}
					break;
				case '<':
					++rIdx;
					++prm_oct;
					if (prm_oct > prm_maxOct)
					{
						return -1;
					}
					break;
				case '>':
					++rIdx;
					--prm_oct;
					if (prm_oct < 0)
					{
						return -1;
					}
					break;
				default:
					return -1;
				}
			}
			else
			{
				if (readLine[rIdx] == '*')
				{
					++rIdx;
					if (readLine[rIdx] == '/')
					{
						++rIdx;
						commentFlag = 0;
					}
				}
				else
				{
					++rIdx;
				}
			}
		}
break2:
		;
	}
	next->np = NULL;
	return 0;
}

/************************************************************/
/*                                                          */
/*                         taskPlay                         */
/*                                                          */
/************************************************************/

int taskPlay()
{
	for (next = top; next->np != NULL; next = next->np)
	{
		Beep(next->hz, next->len);
	}

	//---------演奏し終わった後の処理
	for (next = top; next->np != NULL; )
	{
		top = next->np;
		free(next);
		next = top;
	}

	return 0;
}

HRESULT PlayBeepSound()
{
	if (taskInit() != 0)
	{
		return -1;
	}
	if (taskRead() != 0)
	{
		return -1;
	}
	if (taskPlay() != 0)
	{
		return -1;
	}
	return 0;
}

} // namespace beepSound

DWORD WINAPI PlayBeepSoundThread(LPVOID pVoid)
{
	DWORD times = (DWORD)pVoid;

	for (DWORD i = 0; i < times; i++)
	{
		if (beepSound::PlayBeepSound() != 0)
		{
			return 0;
		}
	}

	return 0;
}

HRESULT PlayBeepSound(DWORD times = 1)
{
	HANDLE hThread = CreateThread(NULL, 0, PlayBeepSoundThread, (LPVOID)times, 0, NULL);
	CloseHandle(hThread);

	return 0;
}

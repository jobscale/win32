#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#define P(x) "\0 " x + 2
#define KB *1000

//#include "adlWord.inl"

static const char * adl_target_s[] = {
	P("porn"),
	P("sex "), //�m�y sex machine, ���b�N�o���h sex pistols, ��w HIV (no sex ...), HIV (safer sex ...) etc...
	P("adult"),
	P("fuck"),
	P("penis"),
	P("erotic"),
	P("vagina"),
	P("lesbian"),
	P("risque"),
	P("pornographic"),
	0};

class CText
{
private:
	char* _str;
	int _len;
public:
	CText(const char* str, int len = -1)
	{
		if (len == -1)
		{
			len = strlen(str);
		}
		_str = (char*)malloc(len + 1);
		if (!_str)
		{
			_len = 0;
			return;
		}
		_len = len;
		memcpy(_str, str, _len);
		_str[_len] = 0;
	}
	virtual ~CText()
	{
		if (_str)
		{
			free(_str);
		}
	}
	char* ToLower()
	{
		for (char* p = _str; *p; p++) *p = tolower(*p);
		return _str;
	}
	char* ToUpper()
	{
		for (char* p = _str; *p; p++) *p = toupper(*p);
		return _str;
	}
	operator char*() const
	{
		return _str;
	}
	int GetLength()
	{
		return _len;
	}
};

char* ToLower(char* s)
{
    for (char* p = s; *p; p++) *p = tolower(*p);
    return s;
}

char* ToUpper(char* s)
{
    for (char* p = s; *p; p++) *p = toupper(*p);
    return s;
}

char* real_get(char* str, const char* substr)
{
	char* ret = strstr(str, substr);
	if (!ret)
	{
		return ret;
	}

	char* dum_start = strstr(str, "\"");
	char* dum_end = dum_start ? strstr(dum_start + 1, "\"") : 0;

	if (dum_end && ret > dum_start)
	{
		return real_get(dum_end + 1, substr);
	}

	return ret;
}


int DeleteTag(char* real, const char* st, const char* ed)
{
	int result = 0;

	char* dup = 0;
	int length = 0;

	char* ena = real;
	for (; ; )
	{
		char* start = real_get(ena, st);
		if (!start)
		{
			break;
		}

		char* end = real_get(start + strlen(st), ed);
		if (!end)
		{
			break;
		}

		if (!dup)
		{
			dup = (char*)malloc(strlen(real));
		}

		result++;

		end += strlen(ed);
		int len = end - start;
		int enalen = start - ena;
		if (!enalen)
		{
			ena += len;
			continue;
		}

		memcpy(dup + length, ena, enalen);
		length += enalen;
		ena = end;
	}

	if (dup)
	{
		if (length)
		{
			memcpy(real, dup, length);
		}

		memcpy(real + length, ena, strlen(ena));
		real[length + strlen(ena)] = 0;

		free(dup);
	}

	return result;
}

int RemoveWord(char* real, const char* st)
{
	int result = 0;

	char* dup = 0;
	int length = 0;

	char* ena = real;
	for (; ; )
	{
		char* start = strstr(ena, st);
		if (!start)
		{
			break;
		}

		char* end = start + strlen(st);

		if (!dup)
		{
			dup = (char*)malloc(strlen(real));
		}

		result++;

		int len = end - start;
		int enalen = start - ena;
		if (!enalen)
		{
			ena += len;
			continue;
		}

		memcpy(dup + length, ena, enalen);
		length += enalen;
		ena = end;
	}

	if (dup)
	{
		if (length)
		{
			memcpy(real, dup, length);
		}

		memcpy(real + length, ena, strlen(ena));
		real[length + strlen(ena)] = 0;

		free(dup);
	}

	return result;
}

int DeleteTagSkeleton(char* real)
{
	int result = 0;

	// �͂��^�O
	result += DeleteTag(real, "<script", "/script>");
	result += DeleteTag(real, "<noscript", "/noscript>");
	result += DeleteTag(real, "<style", "/style>");
	result += DeleteTag(real, "<embed", "/embed>");
	result += DeleteTag(real, "<noembed", "/noembed>");
	result += DeleteTag(real, "<object", "/object>");
	result += DeleteTag(real, "<iframe ", "/iframe>");
	result += DeleteTag(real, "<frameset", "/frameset>");
	result += DeleteTag(real, "<!--", "-->");

	// youtube.com �Ή�
	result += DeleteTag(real, "<button", ">");
	result += DeleteTag(real, "</button", ">");

	result += DeleteTag(real, "<input ", ">");

	/*
	 * �ӊO��select�̓T�C�Y�H��
	 * <select>
	 *  <option value="/contact.php?t=booksearch_ques" selected>���₢���킹</option>
	 * </select>
	 */
	result += DeleteTag(real, "<select ", "/select>");

	return result;
}

int RemoveWordTrush(char* real)
{
	int result = 0;

	result += RemoveWord(real, " ");
	result += RemoveWord(real, "\t");
	result += RemoveWord(real, "\r");
	result += RemoveWord(real, "\n");

	// ��\�I�ȗ\���
	result += RemoveWord(real, "&nbsp;");
	result += RemoveWord(real, "&shy;");
	result += RemoveWord(real, "&quot;");
	result += RemoveWord(real, "&amp;");
	result += RemoveWord(real, "&lt;");
	result += RemoveWord(real, "&gt;");
	result += RemoveWord(real, "&copy;");

	// ����ȗ\���
	result += RemoveWord(real, "&middot;");

	// ����
	result += RemoveWord(real, "0");
	result += RemoveWord(real, "1");
	result += RemoveWord(real, "2");
	result += RemoveWord(real, "3");
	result += RemoveWord(real, "4");
	result += RemoveWord(real, "5");
	result += RemoveWord(real, "6");
	result += RemoveWord(real, "7");
	result += RemoveWord(real, "8");
	result += RemoveWord(real, "9");

	// ��\�w�ȍL������
	result += RemoveWord(real, "click");
	result += RemoveWord(real, "here");
	result += RemoveWord(real, "move");

	return result;
}

int main()
{
	char* real = (char*)malloc(30 * 1000 * 1000); // 30MB
	for (char* next = real; next; next += next ? strlen(next) : 0)
	{
		next = fgets(next, 30 * 1000 * 1000, stdin);
	}
	ToLower(real);

	//////////////////////////////////////////////////////////////////////////
	// ��芸�����ۑ����Ă���
	// �㑱�����Ŏg�p
	CText real_save(real);

	int size_len = strlen(real);

	// 656159
	// 458885
	// 238857
	// �������x���̂Ńf�J�C�͔̂�����
	//if(size_len > 100000)
	//{
	//	fprintf(stdout, "size_len=%d&body_len=%d&nood_len=%d&natu_len=%d&take_len=%d&count_a=%d&bt=%f&nt=%f&count_href=%d&count_img=%d",
	//		size_len,0,0,0,0,0,
	//		0,
	//		0,
	//		0,0);
	//	fprintf(stdout, "&result=99\n");
	//	return 0;
	//}

	char* startbody = strstr(real, "<body");
	startbody = startbody ? strstr(startbody, ">") : NULL;
	if (!startbody)
	{
		fprintf(stderr, "%d: %s\n", __LINE__, real, "not found <body>");
		return -1;
	}

	startbody++;
	real = startbody;

	char* endbody = strstr(real, "</body>");
	if (endbody)
	{
		endbody[0] = 0;
	}

	int body_len = strlen(real);

	//////////////////////////////////////////////////////////////////////////
	// ���������ŏo��
	fprintf(stdout, "size_len=%d&body_len=%d",size_len,body_len);
	//////////////////////////////////////////////////////////////////////////

	CText real_a(real);
	int count_a   = DeleteTag(real_a, "<a ", "/a>");
	int count_img = DeleteTag(real_a, "<img ", ">");

	//////////////////////////////////////////////////////////////////////////
	// ���������ŏo��
	fprintf(stdout, "&count_a=%d&count_img=%d",count_a,count_img);
	//////////////////////////////////////////////////////////////////////////

	if(body_len > 200 KB)
	{
		fprintf(stdout, "nood_len=0&natu_len=0&take_len=0&count_a2=0&bt=0&nt=0&result=99\n");
		return 0;
	}

	DeleteTagSkeleton(real);
	int nood_len = strlen(real);

	CText nature(real);

	DeleteTag(real, "<a href=\"http://www.amazon.co.jp/exec/obidos/", "/a>");
	DeleteTag(real, "<", ">");
	RemoveWordTrush(real);

	count_a = DeleteTag(nature, "<a ", "/a>");
	DeleteTag(nature, "<", ">");
	RemoveWordTrush(nature);

	int natu_len = strlen(nature);
	int take_len = strlen(real);

	//////////////////////////////////////////////////////////////////////////
	// ���������ŏo��
	fprintf(stdout, "&nood_len=%d&natu_len=%d&take_len=%d&count_a2=%d&bt=%f&nt=%f",
		nood_len, natu_len,take_len,count_a,
		(double)take_len / (body_len ? body_len : 1),
		(double)take_len / (nood_len ? nood_len : 1));
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// �ۑ��������̂��ȉ��Ŏg�p����
	real = real_save;
	int min_take = 100;
	double min_perf = 0.08;

	// description ���L�������\���̋K�����ɘa
	char* meta_desc = strstr(real, "<meta name=\"description\"");
	if (meta_desc)
	{
		min_take -= 20;
		min_perf -= 0.01;
	}

	char* meta_key = strstr(real, "<meta name=\"keywords\"");
	if (meta_key)
	{
		min_take -= 20;
		min_perf -= 0.01;
	}

	// �^�C�g���̎擾
	// ������댯��ȁA�A�u<title></title>�v���Ē��g�Ȃ��̂����\����
	char* meta_title = strstr(real, "<title>");
	if (!meta_title)
	{
		// http://blog.goo.ne.jp/�̃^�C�g���Ɏg�p����Ă���`���ɑΉ�����
		// <title lang="ja" xml:lang="ja"></title>
		meta_title = strstr(real, "<title ");
	}

	// �����_�ł͍L���Aiframe�Ƃ̑��ւ����肻���ȋC������B
	// ��{�I�ɍL�����Z��
	if (meta_title)
	{
		// �^�C�g���̒������Z�o����
	}

	/* 
	 * �ʏ��title,description,keyword���ɂȂ��̃p�^�[�������؂������ʂقƂ�ǂ�
	 * �L���A�������̓K�W�F�b�g�ł������B
	 *
	 *             �b 000�b 100�b 110�b 111�b
	 * --------------------------------------
	 * 0:�ʏ�      �b 454�b7639�b1171�b6984�b
	 * 1:�L��      �b5045�b1127�b  22�b  47�b
	 * 2:�K�W�F�b�g�b 209�b3173�b 482�b   4�b
	 * 3:iframe    �b  34�b  33�b   1�b   7�b
	 *
	 * ����Ό���قǃp�^�[���u000�v�̒ʏ�y�[�W���Ă̂͂Ȃ��ȁB�B
	 *
	 * �����&(=)�̐��̔��菈�����Ȃ���title,description,keyword�̗L���Ŕ��f����B
	 *
	 *             �b 000�b 100�b 110�b 111�b
	 * --------------------------------------
	 * 0:�ʏ�      �b 429�b7608�b1169�b6958�b
	 * 1:�L��      �b5044�b1114�b  22�b  39�b
	 * 2:�K�W�F�b�g�b 208�b3156�b 482�b   4�b
	 * 3:iframe    �b  34�b  33�b    �b   7�b
	 *
	 * a�^�O�̐���10�����ōi��
	 *             �b 000�b 100�b 110�b 111�b
	 * --------------------------------------
	 * 0:�ʏ�      �b 369�b1730�b 104�b 265�b
	 * 1:�L��      �b5026�b1001�b  20�b  19�b
	 * 2:�K�W�F�b�g�b 204�b2962�b 113�b   1�b
	 * 3:iframe    �b  32�b  30�b    �b   4�b
	 */
	int title = meta_title ? 1 : 0;
	int description = meta_desc ? 1 : 0;
	int kyeword = meta_key ? 1 : 0;

	//////////////////////////////////////////////////////////////////////////
	// ���������ŏo��
	fprintf(stdout, "&tdk=%d%d%d",title,description,kyeword);
	//////////////////////////////////////////////////////////////////////////

	if(size_len < 300)
	{
		// 21 ... �S�̃T�C�Y��������(350Bytes����)
		fprintf(stdout, "&result=21\n");
		return 0;
	}

	if(!meta_title && !meta_desc && !meta_key)
	{
		// 19 ... title,description,keyword�Ȃ�
		fprintf(stdout, "&result=19\n");
		return 0;
	}

	// <body �c </body>��Ώۂɂ���
	startbody = strstr(real_save, "<body");
	startbody = startbody ? strstr(startbody, ">") : NULL;
	if (!startbody)
	{
		// 9 ... <body �^�O���Ȃ�
		fprintf(stdout, "&result=9\n");
		return 0;
	}

	startbody++;
	real = startbody;

	endbody = strstr(real, "</body>");
	if (endbody)
	{
		endbody[0] = 0;
	}

	body_len = strlen(real);

	if (body_len < 200)
	{
		// 13 ... <body>�̒������Ȃ� 200bytes����
		fprintf(stdout, "&result=13\n");
		return 0;
	}
	else if (body_len > 200 KB)
	{
		// 14 ... <body>�̒��������� 200Kbytes���傫��
		fprintf(stdout, "&result=14\n");
		return 0;
	}
	else
	{

		CText real_a(real);
		count_a   = DeleteTag(real_a, "<a ", "/a>");
		count_img = DeleteTag(real_a, "<img ", ">");

		// img�^�O��a�^�O�̍�
		int diff_img_a = count_img - count_a ;
		//diff_img_a = (diff_img_a > 0) ? diff_img_a : diff_img_a * (-1);

		/*
		 * �p�^�[��1
		 * �Ea�^�O�̐�  �F10����
		 * �Ebody_len   �F-
		 * �Emeta_title �F-
		 * �Emeta_desc  �F�Ȃ�
		 * �Emeta_key   �F�Ȃ�
		 * �Econtent_len�F-
		 * �Eec_count   �F-
		 */
		if(count_a == 0)
		{
			// 24 ... �����N���Ȃ�
			fprintf(stdout, "&result=24\n");
			return 0;
		}
		/*
		 * �p�^�[��2
		 * �Ea�^�O�̐�  �F10����
		 * �Ebody_len   �F-
		 * �Emeta_title �F-
		 * �Emeta_desc  �F�Ȃ�
		 * �Emeta_key   �F�Ȃ�
		 * �Econtent_len�F-
		 * �Eec_count   �F-
		 */
		else if(!meta_desc && !meta_key && count_a < 10)
		{
			// 23 ... description,keyword�Ȃ��Ń����N����10����
			fprintf(stdout, "&result=23\n");
			return 0;
		}
		/*
		 * �p�^�[��3
		 * �Ea�^�O�̐�  �F10����
		 * �Eimg�^�O�̐��F-
		 * �Ebody_len   �F-
		 * �Emeta_title �F-
		 * �Emeta_desc  �F�Ȃ�
		 * �Emeta_key   �F�Ȃ�
		 * �Econtent_len�F-
		 * �Eec_count   �F-
		 *
		 * a�^�O��img�^�O�̊֌W
		 *
		 * ----------------------------------------------------
		 * ���o�����b   desc�Akey�Ȃ��b�b  title,desc�Akey����b
		 * ----------------------------------------------------
		 *         �b a>i�b a=i�b a<i�b�b   a>i�b  a=i�b   a<i�b
		 * ----------------------------------------------------
		 *       10�b  87�b   -�b   6�b�b    36�b    -�b     9�b
		 *       11�b   2�b   1�b   1�b�b     9�b    1�b     3�b
		 *       13�b  23�b 377�b  10�b�b     2�b   17�b     -�b
		 *       15�b  49�b   -�b   2�b�b    60�b    6�b     6�b
		 *       16�b 230�b   1�b  31�b�b   374�b    7�b    49�b
		 *       19�b1870�b1106�b 377�b�b     -�b    -�b     -�b
		 *       21�b   4�b  74�b   5�b�b     -�b    -�b     -�b
		 *       23�b3834�b 100�b 159�b�b     -�b    -�b     -�b
		 *       24�b   -�b 445�b 130�b�b     -�b   17�b    27�b
		 *       25�b3083�b   2�b  64�b�b  5889�b    3�b    93�b
		 * ----------------------------------------------------
		 *
		 * ����
		 * �@[�ʏ�y�[�W]
		 * �@�@�Ea�^�O��img�^�O�������炩�ɑ���
		 * �@[�L���Aiframe]
		 * �@�@�Ea�^�O=img�^�O�̕s���R�����ʂɑ��݂���
		 * �@�@�E�����N�����C���[�W������
		 * �@�@ �ʏ�̓����N(���������N�A�O�������N)�����肫�̃C���[�W�ł���
		 * �@�@�� a�^�O > img�^�O
		 * �@�@
		 * -------------------------------------------------------
		 * ���o�����b       desc�Akey�Ȃ��b�b title,desc�Akey����b
		 * -------------------------------------------------------
		 *         �b   i/a�b a/i�b   a/i�b�b   i/a�b a/i�b   a/i�b
		 * -------------------------------------------------------
		 *       10�b0.1034�b   -�b0.5696�b�b0.1717�b   -�b0.5323�b
		 *       11�b0.2583�b1.00�b0.6875�b�b0.3095�b1.00�b0.1369�b
		 *       13�b0.0000�b1.00�b0.0000�b�b0.0000�b1.00�b     -�b
		 *       15�b0.1347�b   -�b0.5208�b�b0.1236�b   -�b0.4523�b
		 *       16�b0.1928�b1.00�b0.5949�b�b0.1369�b1.00�b0.5317�b
		 *       19�b0.2726�b1.00�b0.3468�b�b     -�b1.00�b     -�b
		 *       21�b0.0000�b1.00�b0.0000�b�b     -�b1.00�b     -�b
		 *       23�b0.0358�b1.00�b0.4190�b�b     -�b1.00�b     -�b
		 *       24�b     -�b1.00�b0.0000�b�b     -�b1.00�b0.0000�b
		 *       25�b0.0735�b1.00�b0.6941�b�b0.1233�b1.00�b0.7028�b
		 * -------------------------------------------------------
		 *
		 * ----------------------------------------------------------
		 * ���o�����b       desc�Akey�Ȃ��b�b    title,desc�Akey����b
		 * ----------------------------------------------------------
		 *         �b     a-i�b a-i�b i-a�b�b     a-i�b a-i�b    i-a�b
		 * ----------------------------------------------------------
		 *       10�b 11.3563�b   -�b14.5�b�b 11.0000�b   -�b 6.3333�b
		 *       11�b  8.0000�b   0�b 5.0�b�b  5.0000�b   0�b 8.6667�b
		 *       13�b  1.0435�b   0�b 1.0�b�b  1.0000�b   0�b      -�b
		 *       15�b 12.0000�b   -�b13.5�b�b  6.6167�b   -�b 7.5000�b
		 *       16�b 11.1609�b   0�b21.7�b�b 11.3209�b   0�b11.0408�b
		 *       19�b  2.0299�b   0�b 2.3�b�b       -�b   0�b      -�b
		 *       21�b  1.0000�b   0�b 1.0�b�b       -�b   0�b      -�b
		 *       23�b  2.5527�b   0�b 9.2�b�b       -�b   0�b      -�b
		 *       24�b       -�b   0�b 2.2�b�b       -�b   0�b 6.1852�b
		 *       25�b109.4674�b   0�b40.5�b�b136.8119�b   0�b30.8387�b
		 * ----------------------------------------------------------
		 */
		else if(!meta_desc && !meta_key && diff_img_a > 1)
		{
			// 26  ... description,keyword�Ȃ���img�^�O��a�^�O�̍�����2�ȏ�
			fprintf(stdout, "&result=26\n");
			return 0;
		}
		/*
		 * �p�^�[��3
		 * �Ea�^�O�̐�  �F15�ȏ�
		 * �Ebody_len   �F1000bytes�ȏ�
		 * �Emeta_title �F-
		 * �Emeta_desc  �F-
		 * �Emeta_key   �F-
		 * �Econtent_len�F-
		 * �Eec_count   �F-
		 */
		//else if( count_a >= 15 && page->body_len >33)
		//{
		//	
		//}
		/*
		 * �p�^�[��4[�e�X�g�I]
		 * �Ea�^�O�̐�  �F20�ȏ�
		 * �Ebody_len   �F-
		 * �Emeta_title �F����
		 * �Emeta_desc  �F-
		 * �Emeta_key   �F-
		 * �Econtent_len�F-
		 * �Eec_count   �F0
		 */
		else if(meta_title && count_a >= 20 /*&& ec_count == 0*/)
		{
			// 25 ... �����N����20�ȏ゠��AURL���P���ł���̂Œʏ�y�[�W�̉\��������
			fprintf(stdout, "&result=25\n");
			return 0;
		}

		DeleteTagSkeleton(real);

		// Javascript�𔲂������̂��T�C�Y�Ƃ���B
		body_len = strlen(real);

		CText nature(real);

		DeleteTag(real, "<a href=\"http://www.amazon.co.jp/exec/obidos/", "/a>");
		DeleteTag(real, "<", ">");
		RemoveWordTrush(real);

		DeleteTag(nature, "<a ", "/a>");
		DeleteTag(nature, "<", ">");
		RemoveWordTrush(nature);

		natu_len = strlen(nature);
		if (natu_len < 20)
		{
			// 15 ... <body>�̒����قƂ�ǋ�
			fprintf(stdout, "&result=15\n");
			return 0;
		}
	}

	take_len = strlen(real);

	if (take_len < min_take)
	{
		//  11 ... ���g�����Ȃ� 100bytes����
		fprintf(stdout, "&result=11\n");
		return 0;
	}
	else if (take_len > 500)
	{
		// 16 ... ���g������ 500bytes���傫��
		fprintf(stdout, "&result=16\n");
		return 0;
	}
	else
	{
		if (body_len)
		{
			double perc = (double)take_len / body_len;
			if (perc < min_perf)
			{
				//  10 ... ���g��0.08
				fprintf(stdout, "&result=10\n");
				return 0;
			}
		}
	}
	
	// �ʏ픻��
	fprintf(stdout, "&result=0\n");

	free(real);
	free(nature);

  return 0;
}

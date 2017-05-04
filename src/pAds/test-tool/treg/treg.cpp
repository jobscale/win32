#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#define P(x) "\0 " x + 2
#define KB *1000

//#include "adlWord.inl"

static const char * adl_target_s[] = {
	P("porn"),
	P("sex "), //—mŠy sex machine, ƒƒbƒNƒoƒ“ƒh sex pistols, ˆãŠw HIV (no sex ...), HIV (safer sex ...) etc...
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

	// ˆÍ‚¢ƒ^ƒO
	result += DeleteTag(real, "<script", "/script>");
	result += DeleteTag(real, "<noscript", "/noscript>");
	result += DeleteTag(real, "<style", "/style>");
	result += DeleteTag(real, "<embed", "/embed>");
	result += DeleteTag(real, "<noembed", "/noembed>");
	result += DeleteTag(real, "<object", "/object>");
	result += DeleteTag(real, "<iframe ", "/iframe>");
	result += DeleteTag(real, "<frameset", "/frameset>");
	result += DeleteTag(real, "<!--", "-->");

	// youtube.com ‘Î‰
	result += DeleteTag(real, "<button", ">");
	result += DeleteTag(real, "</button", ">");

	result += DeleteTag(real, "<input ", ">");

	/*
	 * ˆÓŠO‚Æselect‚ÍƒTƒCƒYH‚¤
	 * <select>
	 *  <option value="/contact.php?t=booksearch_ques" selected>‚¨–â‚¢‡‚í‚¹</option>
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

	// ‘ã•\“I‚È—\–ñŒê
	result += RemoveWord(real, "&nbsp;");
	result += RemoveWord(real, "&shy;");
	result += RemoveWord(real, "&quot;");
	result += RemoveWord(real, "&amp;");
	result += RemoveWord(real, "&lt;");
	result += RemoveWord(real, "&gt;");
	result += RemoveWord(real, "&copy;");

	// “Áê‚È—\–ñŒê
	result += RemoveWord(real, "&middot;");

	// ”š
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

	// ‘ã•\‰w‚ÈL•¶š
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
	// æ‚èŠ¸‚¦‚¸•Û‘¶‚µ‚Ä‚¨‚­
	// Œã‘±ˆ—‚Åg—p
	CText real_save(real);

	int size_len = strlen(real);

	// 656159
	// 458885
	// 238857
	// ˆ—‚ª’x‚¢‚Ì‚ÅƒfƒJƒC‚Ì‚Í”²‚¯‚é
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
	// î•ñ‚ğ‚±‚±‚Åo‚·
	fprintf(stdout, "size_len=%d&body_len=%d",size_len,body_len);
	//////////////////////////////////////////////////////////////////////////

	CText real_a(real);
	int count_a   = DeleteTag(real_a, "<a ", "/a>");
	int count_img = DeleteTag(real_a, "<img ", ">");

	//////////////////////////////////////////////////////////////////////////
	// î•ñ‚ğ‚±‚±‚Åo‚·
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
	// î•ñ‚ğ‚±‚±‚Åo‚·
	fprintf(stdout, "&nood_len=%d&natu_len=%d&take_len=%d&count_a2=%d&bt=%f&nt=%f",
		nood_len, natu_len,take_len,count_a,
		(double)take_len / (body_len ? body_len : 1),
		(double)take_len / (nood_len ? nood_len : 1));
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// •Û‘¶‚µ‚½‚à‚Ì‚ğˆÈ‰º‚Åg—p‚·‚é
	real = real_save;
	int min_take = 100;
	double min_perf = 0.08;

	// description ‚ª—L‚Á‚½‚ç”ñ•\¦‚Ì‹K§‚ğŠÉ˜a
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

	// ƒ^ƒCƒgƒ‹‚Ìæ“¾
	// ‚±‚ê‚àŠëŒ¯‚â‚ÈAAu<title></title>v‚Á‚Ä’†g‚È‚¢‚Ì‚ªŒ‹\‚ ‚é
	char* meta_title = strstr(real, "<title>");
	if (!meta_title)
	{
		// http://blog.goo.ne.jp/‚Ìƒ^ƒCƒgƒ‹‚Ég—p‚³‚ê‚Ä‚¢‚éŒ`®‚É‘Î‰‚·‚é
		// <title lang="ja" xml:lang="ja"></title>
		meta_title = strstr(real, "<title ");
	}

	// Œ»“_‚Å‚ÍLAiframe‚Æ‚Ì‘ŠŠÖ‚ª‚ ‚è‚»‚¤‚È‹C‚ª‚·‚éB
	// Šî–{“I‚ÉL‚ª’Z‚¢
	if (meta_title)
	{
		// ƒ^ƒCƒgƒ‹‚Ì’·‚³‚ğZo‚·‚é
	}

	/* 
	 * ’Êí‚Åtitle,description,keyword‹¤‚É‚È‚µ‚Ìƒpƒ^[ƒ“‚ğŒŸØ‚µ‚½Œ‹‰Ê‚Ù‚Æ‚ñ‚Ç‚ª
	 * LA‚à‚µ‚­‚ÍƒKƒWƒFƒbƒg‚Å‚ ‚Á‚½B
	 *
	 *             b 000b 100b 110b 111b
	 * --------------------------------------
	 * 0:’Êí      b 454b7639b1171b6984b
	 * 1:L      b5045b1127b  22b  47b
	 * 2:ƒKƒWƒFƒbƒgb 209b3173b 482b   4b
	 * 3:iframe    b  34b  33b   1b   7b
	 *
	 * Œ©‚ê‚ÎŒ©‚é‚Ù‚Çƒpƒ^[ƒ“u000v‚Ì’Êíƒy[ƒW‚Á‚Ä‚Ì‚Í‚È‚¢‚ÈBB
	 *
	 * ‚æ‚Á‚Ä&(=)‚Ì”‚Ì”»’èˆ—‚ğ‚È‚­‚µtitle,description,keyword‚Ì—L–³‚Å”»’f‚·‚éB
	 *
	 *             b 000b 100b 110b 111b
	 * --------------------------------------
	 * 0:’Êí      b 429b7608b1169b6958b
	 * 1:L      b5044b1114b  22b  39b
	 * 2:ƒKƒWƒFƒbƒgb 208b3156b 482b   4b
	 * 3:iframe    b  34b  33b    b   7b
	 *
	 * aƒ^ƒO‚Ì”‚ğ10–¢–‚Åi‚é
	 *             b 000b 100b 110b 111b
	 * --------------------------------------
	 * 0:’Êí      b 369b1730b 104b 265b
	 * 1:L      b5026b1001b  20b  19b
	 * 2:ƒKƒWƒFƒbƒgb 204b2962b 113b   1b
	 * 3:iframe    b  32b  30b    b   4b
	 */
	int title = meta_title ? 1 : 0;
	int description = meta_desc ? 1 : 0;
	int kyeword = meta_key ? 1 : 0;

	//////////////////////////////////////////////////////////////////////////
	// î•ñ‚ğ‚±‚±‚Åo‚·
	fprintf(stdout, "&tdk=%d%d%d",title,description,kyeword);
	//////////////////////////////////////////////////////////////////////////

	if(size_len < 300)
	{
		// 21 ... ‘S‘ÌƒTƒCƒY‚ª¬‚³‚¢(350Bytes–¢–)
		fprintf(stdout, "&result=21\n");
		return 0;
	}

	if(!meta_title && !meta_desc && !meta_key)
	{
		// 19 ... title,description,keyword‚È‚µ
		fprintf(stdout, "&result=19\n");
		return 0;
	}

	// <body c </body>‚ğ‘ÎÛ‚É‚·‚é
	startbody = strstr(real_save, "<body");
	startbody = startbody ? strstr(startbody, ">") : NULL;
	if (!startbody)
	{
		// 9 ... <body ƒ^ƒO‚ª‚È‚¢
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
		// 13 ... <body>‚Ì’†‚ª­‚È‚¢ 200bytes–¢–
		fprintf(stdout, "&result=13\n");
		return 0;
	}
	else if (body_len > 200 KB)
	{
		// 14 ... <body>‚Ì’†‚ª‘½‚·‚¬ 200Kbytes‚æ‚è‘å‚«‚¢
		fprintf(stdout, "&result=14\n");
		return 0;
	}
	else
	{

		CText real_a(real);
		count_a   = DeleteTag(real_a, "<a ", "/a>");
		count_img = DeleteTag(real_a, "<img ", ">");

		// imgƒ^ƒO‚Æaƒ^ƒO‚Ì·
		int diff_img_a = count_img - count_a ;
		//diff_img_a = (diff_img_a > 0) ? diff_img_a : diff_img_a * (-1);

		/*
		 * ƒpƒ^[ƒ“1
		 * Eaƒ^ƒO‚Ì”  F10–¢–
		 * Ebody_len   F-
		 * Emeta_title F-
		 * Emeta_desc  F‚È‚µ
		 * Emeta_key   F‚È‚µ
		 * Econtent_lenF-
		 * Eec_count   F-
		 */
		if(count_a == 0)
		{
			// 24 ... ƒŠƒ“ƒN‚ª‚È‚¢
			fprintf(stdout, "&result=24\n");
			return 0;
		}
		/*
		 * ƒpƒ^[ƒ“2
		 * Eaƒ^ƒO‚Ì”  F10–¢–
		 * Ebody_len   F-
		 * Emeta_title F-
		 * Emeta_desc  F‚È‚µ
		 * Emeta_key   F‚È‚µ
		 * Econtent_lenF-
		 * Eec_count   F-
		 */
		else if(!meta_desc && !meta_key && count_a < 10)
		{
			// 23 ... description,keyword‚È‚µ‚ÅƒŠƒ“ƒN”‚ª10–¢–
			fprintf(stdout, "&result=23\n");
			return 0;
		}
		/*
		 * ƒpƒ^[ƒ“3
		 * Eaƒ^ƒO‚Ì”  F10–¢–
		 * Eimgƒ^ƒO‚Ì”F-
		 * Ebody_len   F-
		 * Emeta_title F-
		 * Emeta_desc  F‚È‚µ
		 * Emeta_key   F‚È‚µ
		 * Econtent_lenF-
		 * Eec_count   F-
		 *
		 * aƒ^ƒO‚Æimgƒ^ƒO‚ÌŠÖŒW
		 *
		 * ----------------------------------------------------
		 * ’ŠoğŒb   descAkey‚È‚µbb  title,descAkey‚ ‚èb
		 * ----------------------------------------------------
		 *         b a>ib a=ib a<ibb   a>ib  a=ib   a<ib
		 * ----------------------------------------------------
		 *       10b  87b   -b   6bb    36b    -b     9b
		 *       11b   2b   1b   1bb     9b    1b     3b
		 *       13b  23b 377b  10bb     2b   17b     -b
		 *       15b  49b   -b   2bb    60b    6b     6b
		 *       16b 230b   1b  31bb   374b    7b    49b
		 *       19b1870b1106b 377bb     -b    -b     -b
		 *       21b   4b  74b   5bb     -b    -b     -b
		 *       23b3834b 100b 159bb     -b    -b     -b
		 *       24b   -b 445b 130bb     -b   17b    27b
		 *       25b3083b   2b  64bb  5889b    3b    93b
		 * ----------------------------------------------------
		 *
		 * “Á’¥
		 * @[’Êíƒy[ƒW]
		 * @@Eaƒ^ƒO‚ªimgƒ^ƒO‚æ‚è‚à–¾‚ç‚©‚É‘½‚¢
		 * @[LAiframe]
		 * @@Eaƒ^ƒO=imgƒ^ƒO‚Ì•s©‘R‚ª•’Ê‚É‘¶İ‚·‚é
		 * @@EƒŠƒ“ƒN‚æ‚è‚àƒCƒ[ƒW‚ª‘½‚¢
		 * @@ ’Êí‚ÍƒŠƒ“ƒN(“à•”ƒŠƒ“ƒNAŠO•”ƒŠƒ“ƒN)‚ª‚ ‚è‚«‚ÌƒCƒ[ƒW‚Å‚ ‚é
		 * @@¨ aƒ^ƒO > imgƒ^ƒO
		 * @@
		 * -------------------------------------------------------
		 * ’ŠoğŒb       descAkey‚È‚µbb title,descAkey‚ ‚èb
		 * -------------------------------------------------------
		 *         b   i/ab a/ib   a/ibb   i/ab a/ib   a/ib
		 * -------------------------------------------------------
		 *       10b0.1034b   -b0.5696bb0.1717b   -b0.5323b
		 *       11b0.2583b1.00b0.6875bb0.3095b1.00b0.1369b
		 *       13b0.0000b1.00b0.0000bb0.0000b1.00b     -b
		 *       15b0.1347b   -b0.5208bb0.1236b   -b0.4523b
		 *       16b0.1928b1.00b0.5949bb0.1369b1.00b0.5317b
		 *       19b0.2726b1.00b0.3468bb     -b1.00b     -b
		 *       21b0.0000b1.00b0.0000bb     -b1.00b     -b
		 *       23b0.0358b1.00b0.4190bb     -b1.00b     -b
		 *       24b     -b1.00b0.0000bb     -b1.00b0.0000b
		 *       25b0.0735b1.00b0.6941bb0.1233b1.00b0.7028b
		 * -------------------------------------------------------
		 *
		 * ----------------------------------------------------------
		 * ’ŠoğŒb       descAkey‚È‚µbb    title,descAkey‚ ‚èb
		 * ----------------------------------------------------------
		 *         b     a-ib a-ib i-abb     a-ib a-ib    i-ab
		 * ----------------------------------------------------------
		 *       10b 11.3563b   -b14.5bb 11.0000b   -b 6.3333b
		 *       11b  8.0000b   0b 5.0bb  5.0000b   0b 8.6667b
		 *       13b  1.0435b   0b 1.0bb  1.0000b   0b      -b
		 *       15b 12.0000b   -b13.5bb  6.6167b   -b 7.5000b
		 *       16b 11.1609b   0b21.7bb 11.3209b   0b11.0408b
		 *       19b  2.0299b   0b 2.3bb       -b   0b      -b
		 *       21b  1.0000b   0b 1.0bb       -b   0b      -b
		 *       23b  2.5527b   0b 9.2bb       -b   0b      -b
		 *       24b       -b   0b 2.2bb       -b   0b 6.1852b
		 *       25b109.4674b   0b40.5bb136.8119b   0b30.8387b
		 * ----------------------------------------------------------
		 */
		else if(!meta_desc && !meta_key && diff_img_a > 1)
		{
			// 26  ... description,keyword‚È‚µ‚Åimgƒ^ƒO‚Æaƒ^ƒO‚Ì·•ª‚ª2ˆÈã
			fprintf(stdout, "&result=26\n");
			return 0;
		}
		/*
		 * ƒpƒ^[ƒ“3
		 * Eaƒ^ƒO‚Ì”  F15ˆÈã
		 * Ebody_len   F1000bytesˆÈã
		 * Emeta_title F-
		 * Emeta_desc  F-
		 * Emeta_key   F-
		 * Econtent_lenF-
		 * Eec_count   F-
		 */
		//else if( count_a >= 15 && page->body_len >33)
		//{
		//	
		//}
		/*
		 * ƒpƒ^[ƒ“4[ƒeƒXƒgI]
		 * Eaƒ^ƒO‚Ì”  F20ˆÈã
		 * Ebody_len   F-
		 * Emeta_title F‚ ‚è
		 * Emeta_desc  F-
		 * Emeta_key   F-
		 * Econtent_lenF-
		 * Eec_count   F0
		 */
		else if(meta_title && count_a >= 20 /*&& ec_count == 0*/)
		{
			// 25 ... ƒŠƒ“ƒN”‚ª20ˆÈã‚ ‚èAURL‚à’Pƒ‚Å‚ ‚é‚Ì‚Å’Êíƒy[ƒW‚Ì‰Â”\«‚ª‚‚¢
			fprintf(stdout, "&result=25\n");
			return 0;
		}

		DeleteTagSkeleton(real);

		// Javascript‚ğ”²‚¢‚½‚à‚Ì‚ğƒTƒCƒY‚Æ‚·‚éB
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
			// 15 ... <body>‚Ì’†‚ª‚Ù‚Æ‚ñ‚Ç‹ó
			fprintf(stdout, "&result=15\n");
			return 0;
		}
	}

	take_len = strlen(real);

	if (take_len < min_take)
	{
		//  11 ... ’†g‚ª­‚È‚¢ 100bytes–¢–
		fprintf(stdout, "&result=11\n");
		return 0;
	}
	else if (take_len > 500)
	{
		// 16 ... ’†g‚ª‘½‚¢ 500bytes‚æ‚è‘å‚«‚¢
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
				//  10 ... ’†g‚ª0.08
				fprintf(stdout, "&result=10\n");
				return 0;
			}
		}
	}
	
	// ’Êí”»’è
	fprintf(stdout, "&result=0\n");

	free(real);
	free(nature);

  return 0;
}

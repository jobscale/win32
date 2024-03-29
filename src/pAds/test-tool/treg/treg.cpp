#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#define P(x) "\0 " x + 2
#define KB *1000

//#include "adlWord.inl"

static const char * adl_target_s[] = {
	P("porn"),
	P("sex "), //洋楽 sex machine, ロックバンド sex pistols, 医学 HIV (no sex ...), HIV (safer sex ...) etc...
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

	// 囲いタグ
	result += DeleteTag(real, "<script", "/script>");
	result += DeleteTag(real, "<noscript", "/noscript>");
	result += DeleteTag(real, "<style", "/style>");
	result += DeleteTag(real, "<embed", "/embed>");
	result += DeleteTag(real, "<noembed", "/noembed>");
	result += DeleteTag(real, "<object", "/object>");
	result += DeleteTag(real, "<iframe ", "/iframe>");
	result += DeleteTag(real, "<frameset", "/frameset>");
	result += DeleteTag(real, "<!--", "-->");

	// youtube.com 対応
	result += DeleteTag(real, "<button", ">");
	result += DeleteTag(real, "</button", ">");

	result += DeleteTag(real, "<input ", ">");

	/*
	 * 意外とselectはサイズ食う
	 * <select>
	 *  <option value="/contact.php?t=booksearch_ques" selected>お問い合わせ</option>
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

	// 代表的な予約語
	result += RemoveWord(real, "&nbsp;");
	result += RemoveWord(real, "&shy;");
	result += RemoveWord(real, "&quot;");
	result += RemoveWord(real, "&amp;");
	result += RemoveWord(real, "&lt;");
	result += RemoveWord(real, "&gt;");
	result += RemoveWord(real, "&copy;");

	// 特殊な予約語
	result += RemoveWord(real, "&middot;");

	// 数字
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

	// 代表駅な広告文字
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
	// 取り敢えず保存しておく
	// 後続処理で使用
	CText real_save(real);

	int size_len = strlen(real);

	// 656159
	// 458885
	// 238857
	// 処理が遅いのでデカイのは抜ける
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
	// 情報をここで出す
	fprintf(stdout, "size_len=%d&body_len=%d",size_len,body_len);
	//////////////////////////////////////////////////////////////////////////

	CText real_a(real);
	int count_a   = DeleteTag(real_a, "<a ", "/a>");
	int count_img = DeleteTag(real_a, "<img ", ">");

	//////////////////////////////////////////////////////////////////////////
	// 情報をここで出す
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
	// 情報をここで出す
	fprintf(stdout, "&nood_len=%d&natu_len=%d&take_len=%d&count_a2=%d&bt=%f&nt=%f",
		nood_len, natu_len,take_len,count_a,
		(double)take_len / (body_len ? body_len : 1),
		(double)take_len / (nood_len ? nood_len : 1));
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 保存したものを以下で使用する
	real = real_save;
	int min_take = 100;
	double min_perf = 0.08;

	// description が有ったら非表示の規制を緩和
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

	// タイトルの取得
	// これも危険やな、、「<title></title>」って中身ないのが結構ある
	char* meta_title = strstr(real, "<title>");
	if (!meta_title)
	{
		// http://blog.goo.ne.jp/のタイトルに使用されている形式に対応する
		// <title lang="ja" xml:lang="ja"></title>
		meta_title = strstr(real, "<title ");
	}

	// 現時点では広告、iframeとの相関がありそうな気がする。
	// 基本的に広告が短い
	if (meta_title)
	{
		// タイトルの長さを算出する
	}

	/* 
	 * 通常でtitle,description,keyword共になしのパターンを検証した結果ほとんどが
	 * 広告、もしくはガジェットであった。
	 *
	 *             ｜ 000｜ 100｜ 110｜ 111｜
	 * --------------------------------------
	 * 0:通常      ｜ 454｜7639｜1171｜6984｜
	 * 1:広告      ｜5045｜1127｜  22｜  47｜
	 * 2:ガジェット｜ 209｜3173｜ 482｜   4｜
	 * 3:iframe    ｜  34｜  33｜   1｜   7｜
	 *
	 * 見れば見るほどパターン「000」の通常ページってのはないな。。
	 *
	 * よって&(=)の数の判定処理をなくしtitle,description,keywordの有無で判断する。
	 *
	 *             ｜ 000｜ 100｜ 110｜ 111｜
	 * --------------------------------------
	 * 0:通常      ｜ 429｜7608｜1169｜6958｜
	 * 1:広告      ｜5044｜1114｜  22｜  39｜
	 * 2:ガジェット｜ 208｜3156｜ 482｜   4｜
	 * 3:iframe    ｜  34｜  33｜    ｜   7｜
	 *
	 * aタグの数を10未満で絞る
	 *             ｜ 000｜ 100｜ 110｜ 111｜
	 * --------------------------------------
	 * 0:通常      ｜ 369｜1730｜ 104｜ 265｜
	 * 1:広告      ｜5026｜1001｜  20｜  19｜
	 * 2:ガジェット｜ 204｜2962｜ 113｜   1｜
	 * 3:iframe    ｜  32｜  30｜    ｜   4｜
	 */
	int title = meta_title ? 1 : 0;
	int description = meta_desc ? 1 : 0;
	int kyeword = meta_key ? 1 : 0;

	//////////////////////////////////////////////////////////////////////////
	// 情報をここで出す
	fprintf(stdout, "&tdk=%d%d%d",title,description,kyeword);
	//////////////////////////////////////////////////////////////////////////

	if(size_len < 300)
	{
		// 21 ... 全体サイズが小さい(350Bytes未満)
		fprintf(stdout, "&result=21\n");
		return 0;
	}

	if(!meta_title && !meta_desc && !meta_key)
	{
		// 19 ... title,description,keywordなし
		fprintf(stdout, "&result=19\n");
		return 0;
	}

	// <body … </body>を対象にする
	startbody = strstr(real_save, "<body");
	startbody = startbody ? strstr(startbody, ">") : NULL;
	if (!startbody)
	{
		// 9 ... <body タグがない
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
		// 13 ... <body>の中が少ない 200bytes未満
		fprintf(stdout, "&result=13\n");
		return 0;
	}
	else if (body_len > 200 KB)
	{
		// 14 ... <body>の中が多すぎ 200Kbytesより大きい
		fprintf(stdout, "&result=14\n");
		return 0;
	}
	else
	{

		CText real_a(real);
		count_a   = DeleteTag(real_a, "<a ", "/a>");
		count_img = DeleteTag(real_a, "<img ", ">");

		// imgタグとaタグの差
		int diff_img_a = count_img - count_a ;
		//diff_img_a = (diff_img_a > 0) ? diff_img_a : diff_img_a * (-1);

		/*
		 * パターン1
		 * ・aタグの数  ：10未満
		 * ・body_len   ：-
		 * ・meta_title ：-
		 * ・meta_desc  ：なし
		 * ・meta_key   ：なし
		 * ・content_len：-
		 * ・ec_count   ：-
		 */
		if(count_a == 0)
		{
			// 24 ... リンクがない
			fprintf(stdout, "&result=24\n");
			return 0;
		}
		/*
		 * パターン2
		 * ・aタグの数  ：10未満
		 * ・body_len   ：-
		 * ・meta_title ：-
		 * ・meta_desc  ：なし
		 * ・meta_key   ：なし
		 * ・content_len：-
		 * ・ec_count   ：-
		 */
		else if(!meta_desc && !meta_key && count_a < 10)
		{
			// 23 ... description,keywordなしでリンク数が10未満
			fprintf(stdout, "&result=23\n");
			return 0;
		}
		/*
		 * パターン3
		 * ・aタグの数  ：10未満
		 * ・imgタグの数：-
		 * ・body_len   ：-
		 * ・meta_title ：-
		 * ・meta_desc  ：なし
		 * ・meta_key   ：なし
		 * ・content_len：-
		 * ・ec_count   ：-
		 *
		 * aタグとimgタグの関係
		 *
		 * ----------------------------------------------------
		 * 抽出条件｜   desc、keyなし｜｜  title,desc、keyあり｜
		 * ----------------------------------------------------
		 *         ｜ a>i｜ a=i｜ a<i｜｜   a>i｜  a=i｜   a<i｜
		 * ----------------------------------------------------
		 *       10｜  87｜   -｜   6｜｜    36｜    -｜     9｜
		 *       11｜   2｜   1｜   1｜｜     9｜    1｜     3｜
		 *       13｜  23｜ 377｜  10｜｜     2｜   17｜     -｜
		 *       15｜  49｜   -｜   2｜｜    60｜    6｜     6｜
		 *       16｜ 230｜   1｜  31｜｜   374｜    7｜    49｜
		 *       19｜1870｜1106｜ 377｜｜     -｜    -｜     -｜
		 *       21｜   4｜  74｜   5｜｜     -｜    -｜     -｜
		 *       23｜3834｜ 100｜ 159｜｜     -｜    -｜     -｜
		 *       24｜   -｜ 445｜ 130｜｜     -｜   17｜    27｜
		 *       25｜3083｜   2｜  64｜｜  5889｜    3｜    93｜
		 * ----------------------------------------------------
		 *
		 * 特徴
		 * 　[通常ページ]
		 * 　　・aタグがimgタグよりも明らかに多い
		 * 　[広告、iframe]
		 * 　　・aタグ=imgタグの不自然が普通に存在する
		 * 　　・リンクよりもイメージが多い
		 * 　　 通常はリンク(内部リンク、外部リンク)がありきのイメージである
		 * 　　→ aタグ > imgタグ
		 * 　　
		 * -------------------------------------------------------
		 * 抽出条件｜       desc、keyなし｜｜ title,desc、keyあり｜
		 * -------------------------------------------------------
		 *         ｜   i/a｜ a/i｜   a/i｜｜   i/a｜ a/i｜   a/i｜
		 * -------------------------------------------------------
		 *       10｜0.1034｜   -｜0.5696｜｜0.1717｜   -｜0.5323｜
		 *       11｜0.2583｜1.00｜0.6875｜｜0.3095｜1.00｜0.1369｜
		 *       13｜0.0000｜1.00｜0.0000｜｜0.0000｜1.00｜     -｜
		 *       15｜0.1347｜   -｜0.5208｜｜0.1236｜   -｜0.4523｜
		 *       16｜0.1928｜1.00｜0.5949｜｜0.1369｜1.00｜0.5317｜
		 *       19｜0.2726｜1.00｜0.3468｜｜     -｜1.00｜     -｜
		 *       21｜0.0000｜1.00｜0.0000｜｜     -｜1.00｜     -｜
		 *       23｜0.0358｜1.00｜0.4190｜｜     -｜1.00｜     -｜
		 *       24｜     -｜1.00｜0.0000｜｜     -｜1.00｜0.0000｜
		 *       25｜0.0735｜1.00｜0.6941｜｜0.1233｜1.00｜0.7028｜
		 * -------------------------------------------------------
		 *
		 * ----------------------------------------------------------
		 * 抽出条件｜       desc、keyなし｜｜    title,desc、keyあり｜
		 * ----------------------------------------------------------
		 *         ｜     a-i｜ a-i｜ i-a｜｜     a-i｜ a-i｜    i-a｜
		 * ----------------------------------------------------------
		 *       10｜ 11.3563｜   -｜14.5｜｜ 11.0000｜   -｜ 6.3333｜
		 *       11｜  8.0000｜   0｜ 5.0｜｜  5.0000｜   0｜ 8.6667｜
		 *       13｜  1.0435｜   0｜ 1.0｜｜  1.0000｜   0｜      -｜
		 *       15｜ 12.0000｜   -｜13.5｜｜  6.6167｜   -｜ 7.5000｜
		 *       16｜ 11.1609｜   0｜21.7｜｜ 11.3209｜   0｜11.0408｜
		 *       19｜  2.0299｜   0｜ 2.3｜｜       -｜   0｜      -｜
		 *       21｜  1.0000｜   0｜ 1.0｜｜       -｜   0｜      -｜
		 *       23｜  2.5527｜   0｜ 9.2｜｜       -｜   0｜      -｜
		 *       24｜       -｜   0｜ 2.2｜｜       -｜   0｜ 6.1852｜
		 *       25｜109.4674｜   0｜40.5｜｜136.8119｜   0｜30.8387｜
		 * ----------------------------------------------------------
		 */
		else if(!meta_desc && !meta_key && diff_img_a > 1)
		{
			// 26  ... description,keywordなしでimgタグとaタグの差分が2以上
			fprintf(stdout, "&result=26\n");
			return 0;
		}
		/*
		 * パターン3
		 * ・aタグの数  ：15以上
		 * ・body_len   ：1000bytes以上
		 * ・meta_title ：-
		 * ・meta_desc  ：-
		 * ・meta_key   ：-
		 * ・content_len：-
		 * ・ec_count   ：-
		 */
		//else if( count_a >= 15 && page->body_len >33)
		//{
		//	
		//}
		/*
		 * パターン4[テスト！]
		 * ・aタグの数  ：20以上
		 * ・body_len   ：-
		 * ・meta_title ：あり
		 * ・meta_desc  ：-
		 * ・meta_key   ：-
		 * ・content_len：-
		 * ・ec_count   ：0
		 */
		else if(meta_title && count_a >= 20 /*&& ec_count == 0*/)
		{
			// 25 ... リンク数が20以上あり、URLも単純であるので通常ページの可能性が高い
			fprintf(stdout, "&result=25\n");
			return 0;
		}

		DeleteTagSkeleton(real);

		// Javascriptを抜いたものをサイズとする。
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
			// 15 ... <body>の中がほとんど空
			fprintf(stdout, "&result=15\n");
			return 0;
		}
	}

	take_len = strlen(real);

	if (take_len < min_take)
	{
		//  11 ... 中身が少ない 100bytes未満
		fprintf(stdout, "&result=11\n");
		return 0;
	}
	else if (take_len > 500)
	{
		// 16 ... 中身が多い 500bytesより大きい
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
				//  10 ... 中身が0.08
				fprintf(stdout, "&result=10\n");
				return 0;
			}
		}
	}
	
	// 通常判定
	fprintf(stdout, "&result=0\n");

	free(real);
	free(nature);

  return 0;
}

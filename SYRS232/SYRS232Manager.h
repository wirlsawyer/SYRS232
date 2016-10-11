#pragma once

class SYRS232Manager
{
public:
	//creat
	static SYRS232Manager* Instance(void);
	~SYRS232Manager(void);
	//method
	char *FindDevInMgrCom(GUID GUID_STR, LPWSTR lpszHWId);
	bool ConnectComPort(char *ComName);
	bool ConnetComAuto();

private:
	SYRS232Manager(void);
private:	
	//creat
	static SYRS232Manager*				m_instance;		
	//var
	char m_szBuf[512];
};

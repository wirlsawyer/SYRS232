#include "StdAfx.h"
#include "SYRS232Manager.h"
#include "RS232c.h"
#include <string>
#include <iostream>
#include <Setupapi.h>
#pragma comment(lib, "Setupapi.lib")

RS232c *g_rs232 = NULL;

SYRS232Manager* SYRS232Manager::m_instance = NULL;
//=================================================================================================================================
SYRS232Manager* SYRS232Manager::Instance()
{
	if (m_instance == NULL){
		m_instance = new SYRS232Manager(); 
	}
	return m_instance;
}

SYRS232Manager::SYRS232Manager(void)
{
}

SYRS232Manager::~SYRS232Manager(void)
{
}

char* SYRS232Manager::FindDevInMgrCom(GUID GUID_STR, LPWSTR lpszHWId)
{
	wchar_t wszHWId[256];
	wcscpy_s(wszHWId, lpszHWId);
	_wcslwr_s(wszHWId);


	BYTE DevHWID[512];
	memset(DevHWID, 0, 512);
	HDEVINFO hInfo = SetupDiGetClassDevsW(&GUID_STR,NULL,NULL,DIGCF_PRESENT);

	SP_DEVINFO_DATA data;
	data.cbSize = sizeof(SP_DEVINFO_DATA);

	int ItemCount = 0;
	BOOL ret=false,bEndFlag = false;
	while(!bEndFlag)
	{
		ret = SetupDiEnumDeviceInfo(hInfo,ItemCount,&data);
		if(!ret)
		{
			DWORD err = GetLastError();
			if(err != ERROR_NO_MORE_ITEMS)
				::MessageBoxW(NULL, L"Enumerate the USB device Fail !!", L"Message",MB_OK);

			bEndFlag = true;
			break;
		}		

		wchar_t ItemString[512];
		memset(ItemString, 0, 512*2);
		SetupDiGetDeviceRegistryPropertyW(hInfo,&data,SPDRP_HARDWAREID,NULL,DevHWID,512,NULL);
		swprintf_s(ItemString,L"%s",DevHWID);
		_wcslwr_s(ItemString);

		//OutputDebugStringW(ItemString);
		//OutputDebugStringW(L"\n");


		//if(wcscmp(ItemString, wszHWId)==0)
		//	return true;

		wchar_t *pdest = NULL;
		pdest = wcsstr( ItemString, wszHWId);
		if( pdest != NULL )
		{

			BYTE friendlyName[512];

			if(SetupDiGetDeviceRegistryPropertyA(hInfo, &data, SPDRP_FRIENDLYNAME, NULL, friendlyName, 512, NULL))
			{
				
				sprintf_s(m_szBuf,"%s",friendlyName);
				//_wcsupr_s(szbuf);

				OutputDebugStringA(m_szBuf);
				OutputDebugStringW(L"\n");


				std::string str = m_szBuf;

				std::size_t found1 = str.find("(");
				std::size_t found2 = str.find(")");
				if (found1 != std::string::npos && found2 != std::string::npos)
				{
					str = str.substr (found1+1, found2-found1-1);

					memset(m_szBuf, 0, sizeof(m_szBuf));
					sprintf(m_szBuf, "%s", str.c_str());
					return m_szBuf;
				}
			}

			return NULL;
		}

		ItemCount++;
	}
	return NULL;
}

bool SYRS232Manager::ConnectComPort(char *ComName)
{
	if (g_rs232) delete g_rs232;
	g_rs232 = new RS232c();

	return g_rs232->Connect(ComName, 9600);
}

bool SYRS232Manager::ConnetComAuto()
{
	DWORD requiredSize;
	// Get Class GUID for PORTS class
	GUID classGuid[1];
	if ( ! SetupDiClassGuidsFromName( _T("PORTS"), classGuid, 1, &requiredSize ) )
		return false;

	char *ComName = SYRS232Manager::Instance()->FindDevInMgrCom(classGuid[0], L"USB\\VID_10C4&PID_EA60&REV_0100");
	if ( ComName == NULL)
	{
		return false;
	}

	return ConnectComPort(ComName);
}
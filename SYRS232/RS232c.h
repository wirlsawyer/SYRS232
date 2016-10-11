#ifndef _RS_232C_H_
#define _RS_232C_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>

class RS232c{
private:
	HANDLE m_hComm;
	DWORD size;
	DCB	m_Dcb;
public:
	
	bool Connect(char* PortNmae,  
		int BaudRate,
		int ByteSize,
		int Parity,		
		int StopBits,		
		int RTS,		
		int DTR,		
		int ReadTimeOut,
		int WriteTimeOut
		);
	
	bool Read(char* Buff,int NumberOfCharactersToRead);
	
	
	
	bool Read_CRLF(char* Buff,int NumberOfCharactersToRead);
	
	bool Send(char* word);
	
	bool isLink();
	
	~RS232c();
};

bool RS232c::Connect(char* PortNmae="COM1",
					 int BaudRate = 9600,
					 int ByteSize = 8,
					 int Parity = NOPARITY,
					 int StopBits = ONESTOPBIT,
					 int RTS = RTS_CONTROL_DISABLE,
					 int DTR = DTR_CONTROL_DISABLE,
					 int ReadTimeOut =5000,
					 int WriteTimeOut = 20000   
					 )
{

	m_hComm = CreateFileA( 
		PortNmae,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
		);

	if( m_hComm == INVALID_HANDLE_VALUE ) return FALSE;

	
	SetCommMask( m_hComm, 0 );

	SetupComm( m_hComm, 2000L, 2000L );


	COMMTIMEOUTS	m_CommTime;
	m_CommTime.ReadIntervalTimeout = 0xFFFFFFFF;
	m_CommTime.ReadTotalTimeoutMultiplier = 0;
	m_CommTime.ReadTotalTimeoutConstant = ReadTimeOut;
	m_CommTime.WriteTotalTimeoutMultiplier = 0;
	m_CommTime.WriteTotalTimeoutConstant = WriteTimeOut;

	
	SetCommTimeouts( m_hComm, &m_CommTime);



	
	GetCommState( m_hComm, &m_Dcb );

	
	m_Dcb.DCBlength = sizeof( DCB );
	m_Dcb.fBinary = FALSE;
	m_Dcb.BaudRate = BaudRate;
	m_Dcb.ByteSize = ByteSize;
	m_Dcb.Parity = Parity;
	m_Dcb.StopBits = StopBits;
	m_Dcb.fRtsControl = RTS;
	m_Dcb.fDtrControl = DTR;
	m_Dcb.fDsrSensitivity = FALSE;
	m_Dcb.fAbortOnError = FALSE;
	m_Dcb.fNull = TRUE;
	m_Dcb.fParity =TRUE;
	m_Dcb.ErrorChar =0x00;
	m_Dcb.fErrorChar = TRUE;


	SetCommState( m_hComm, &m_Dcb ) ;
	if( !SetCommState( m_hComm, &m_Dcb ) ) return FALSE;

	return TRUE;
}

bool RS232c::Read(char* Buff,int NumberOfCharactersToRead){
	for(int i=0;i<NumberOfCharactersToRead;i++){
		Buff[i]='\0';
	}
	if(0==ReadFile(m_hComm,Buff,NumberOfCharactersToRead, &size,NULL)){
		return FALSE;
	}else{
		return TRUE;
	}
}

bool RS232c::Read_CRLF(char* Buff,int NumberOfCharactersToRead){

	for(int i=0;i<NumberOfCharactersToRead;i++){
		Buff[i]='\0';
	}
	char buf[10]="";
	char beChar=0x00;
	bool flag=TRUE;
	int c=0;

	while((!(buf[0]==0x0A && beChar==0x0D)) && c<NumberOfCharactersToRead && isLink()){
		buf[0]=0x00;

		if(0==ReadFile(m_hComm,buf,1, &size,NULL)){
			flag=FALSE;
		}else{
			flag=TRUE;
		}
		if(buf[0]!=0x00 && flag){
			if(c>1) beChar=Buff[c-1];
			Buff[c++]=buf[0];
		}else{
			return FALSE;
		}
	}
	return TRUE;
}

bool RS232c::Send(char* word){
	if(0==WriteFile(m_hComm,word,strlen(word),&size, NULL)){
		return FALSE;
	}else{
		return TRUE;
	}
}

bool RS232c::isLink(){
	if(m_Dcb.fDtrControl == DTR_CONTROL_ENABLE){
		DWORD   dwSts;
		BOOL    fSuccess = GetCommModemStatus( m_hComm, &dwSts );    
		if( !fSuccess ){
		} else {
			if(!(dwSts & MS_DSR_ON))	return FALSE;			
			//if(dwSts & MS_CTS_ON) //CTSON
			//if(dwSts & MS_RING_ON) //RINGON
			if(!(dwSts & MS_RLSD_ON))  return FALSE;			//RLSDON
		}
	}
	return TRUE;
}


RS232c::~RS232c(){
	
	CloseHandle(m_hComm);
}



#endif // _RS_232C_H_


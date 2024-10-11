﻿//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UDP_MonUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUDPMon *UDPMon;
//---------------------------------------------------------------------------
__fastcall TUDPMon::TUDPMon(TComponent* Owner)
	: TForm(Owner)
{

	struct channel *c1 = new struct channel;
	c1->bMCAST = false;
	c1->port = 4349;
	c1->UDPServer = CreateUDPListener(c1->port);
	ChannelList.push_back(c1);

	c1 = new struct channel;
	c1->bMCAST = true;
	c1->port = 5001;
	c1->MCastGroup = "239.255.1.1";
	c1->MCastClient = CreateMulticastListener(c1->MCastGroup, c1->port);
	ChannelList.push_back(c1);

	c1 = new struct channel;
	c1->bMCAST = true;
	c1->port = 5002;
	c1->MCastGroup = "239.255.1.1";
	c1->MCastClient = CreateMulticastListener(c1->MCastGroup, c1->port);
	ChannelList.push_back(c1);

	c1 = new struct channel;
	c1->bMCAST = true;
	c1->port = 5003;
	c1->MCastGroup = "239.255.1.1";
	c1->MCastClient = CreateMulticastListener(c1->MCastGroup, c1->port);
	ChannelList.push_back(c1);

	c1 = new struct channel;
	c1->bMCAST = true;
	c1->port = 5004;
	c1->MCastGroup = "239.255.1.1";
	c1->MCastClient = CreateMulticastListener(c1->MCastGroup, c1->port);
	ChannelList.push_back(c1);

	c1 = new struct channel;
	c1->bMCAST = true;
	c1->port = 5005;
	c1->MCastGroup = "239.255.1.1";
	c1->MCastClient = CreateMulticastListener(c1->MCastGroup, c1->port);
	ChannelList.push_back(c1);

	c1 = new struct channel;
	c1->bMCAST = false;
	c1->port = 6666;
	c1->UDPServer = CreateUDPListener(c1->port);
	ChannelList.push_back(c1);

	c1 = new struct channel;
	c1->bMCAST = true;
	c1->port = 7003;
	c1->MCastGroup = "239.5.50.40";
	c1->MCastClient = CreateMulticastListener(c1->MCastGroup, c1->port);
	ChannelList.push_back(c1);



	StartAllMonitors();
	Button1->Caption="Stop";

}
//---------------------------------------------------------------------------


void  TUDPMon::pme_SetColour(TColor color, bool _isBold)
{
	colour = color;
	isBold = _isBold;
}

 //this is a printf for the Debug Memo widget
void  TUDPMon::pme(const char* fmt, ...)
{
	MemoDebug->StyleElements = MemoDebug->StyleElements >> seFont;  //necessary to allow colour/font changes if a style is in use
	MemoDebug->SelAttributes->Color = colour;
	if (isBold) MemoDebug->SelAttributes->Style = MemoDebug->SelAttributes->Style << fsBold;
	else MemoDebug->SelAttributes->Style = MemoDebug->SelAttributes->Style >> fsBold;

	va_list args;
	va_start(args, fmt);
	char buf[2000];
	vsprintf(buf,fmt,args);

	MemoDebug->SelText = buf;
	va_end(args);

	MemoDebug->SelText = "\r\n";

}
//---------------------------------------------------------------------------

void __fastcall TUDPMon::StartAllMonitors()
{
	for (auto a: ChannelList) {
		a->bActive = true;
		if (a->bMCAST)  a->MCastClient->Active = true;
		else a->UDPServer->Active = true;
	}
}

void __fastcall TUDPMon::StopAllMonitors()
{
	for (auto a: ChannelList) {
		a->bActive = false;
		if (a->bMCAST)  a->MCastClient->Active = false;
		else a->UDPServer->Active = false;
	}
}


void __fastcall TUDPMon::Button1Click(TObject *Sender)
{
	if (true == bRunning)  //is running so stop it now
	{
		bRunning = false;
		Button1->Caption="Start";
		StopAllMonitors();
	}
	else      //was not running so start it now
	{
		bRunning = true;
		Button1->Caption="Stop";
		StartAllMonitors();
	}





}
//---------------------------------------------------------------------------


TIdUDPServer* __fastcall TUDPMon::CreateUDPListener(int port)
{
	TIdUDPServer *IdUDPServer = new TIdUDPServer();
	IdUDPServer->DefaultPort = port;
	IdUDPServer->ReuseSocket = rsTrue;
	IdUDPServer->OnUDPRead = IdUDPServerUDPRead;
	IdUDPServer->Active = false;
	return IdUDPServer;
}


TIdIPMCastClient* __fastcall TUDPMon::CreateMulticastListener(String MCastGroup, int port)
{
	TIdIPMCastClient *IdIPMCastClient = new TIdIPMCastClient();
	IdIPMCastClient->DefaultPort = port;
	IdIPMCastClient->MulticastGroup = MCastGroup;
	IdIPMCastClient->ReuseSocket = rsTrue;
	IdIPMCastClient->OnIPMCastRead = IdIPMCastClientIPMCastRead;
	IdIPMCastClient->Active = false;
	return IdIPMCastClient;
}


int __fastcall TUDPMon::checkProto(const TIdBytes AData)
{
    return 0;
}


void __fastcall TUDPMon::IdIPMCastClientIPMCastRead(TObject *Sender, const TIdBytes AData, TIdSocketHandle *ABinding)
{
	int port = ABinding->Port;
	//pme("%d Rx %d bytes",port,AData.size());

	struct channel *c = FindChannel(port);
	if (NULL == c) return;

	c->Age=0;
	c->RxCount++;

    int t = checkProto(AData);


}
//---------------------------------------------------------------------------
void __fastcall TUDPMon::IdUDPServerUDPRead(TIdUDPListenerThread *AThread, const TIdBytes AData, TIdSocketHandle *ABinding)
{
	int port = ABinding->Port;

	//pme("%d Rx %d bytes",port, AData.size());

	struct channel *c = FindChannel(port);

	c->Age=0;
	c->RxCount++;
 }



void __fastcall TUDPMon::Timer1Timer(TObject *Sender)
{
	MemoDebug->Lines->Clear();
    pme_SetColour(clBlack,false);
	pme("There are %d Channels",ChannelList.size());
	pme("  PORT    Pkts Rx         Age[s]        Type");
	pme("--------------------------------------------");

	for (auto a: ChannelList) {

		if (a->Age > 3) pme_SetColour(clRed,false);
		else pme_SetColour(clBlack,false);
		pme("%6d  %9d       % 7d    %8s",a->port, a->RxCount,a->Age, a->Type);
		a->Age++;
	}
}
//---------------------------------------------------------------------------

void __fastcall TUDPMon::BN_QuitClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

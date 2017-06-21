#ifndef _SP_SCRIPTING_
#define _SP_SCRIPTING_


#include <wex/lkscript.h>


class SolarPILOTScriptWindowFactory : public wxLKScriptWindowFactory
{
public:
	SolarPILOTScriptWindowFactory();
	virtual ~SolarPILOTScriptWindowFactory();
	virtual wxLKScriptWindow *Create();
};


class SolarPILOTScriptWindow  : public wxLKScriptWindow
{
public:
	SolarPILOTScriptWindow( wxWindow *parent, int id = wxID_ANY );
	
protected:
	virtual void OnScriptStarted();
	virtual void OnScriptStopped();
	virtual void OnHelp();

	//DECLARE_EVENT_TABLE();
};

#endif
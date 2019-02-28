// cep1.0.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

int selfid;
int memorybrowserpluginid = -1; //initialize it to -1 to indicate failure (used by the DisablePlugin routine)
int addresslistPluginID = -1;
int debugpluginID = -1;
int ProcesswatchpluginID = -1;
int PointerReassignmentPluginID = -1;
int MainMenuPluginID = -1;

ExportedFunctions Exported;

extern BOOL maskingCEProcessPath(void);

extern void lua_export(lua_State *);

extern "C" __declspec(dllexport) BOOL __stdcall CEPlugin_GetVersion(PPluginVersion pv, int sizeofpluginversion)
{
	pv->version = CESDK_VERSION;
	pv->pluginname = (char *)"CheatEnginePlugin v1.0"; //exact strings like this are pointers to the string in the dll, so workable
	return TRUE;
}


extern "C" __declspec(dllexport) BOOL __stdcall CEPlugin_InitializePlugin(PExportedFunctions ef, int pluginid)
{

	ADDRESSLISTPLUGIN_INIT init0;
	MEMORYVIEWPLUGIN_INIT init1;
	DEBUGEVENTPLUGIN_INIT init2;
	PROCESSWATCHERPLUGIN_INIT init3;
	POINTERREASSIGNMENTPLUGIN_INIT init4;
	MAINMENUPLUGIN_INIT init5;

	selfid = pluginid;

	Exported = *ef;
	if (Exported.sizeofExportedFunctions != sizeof(Exported))
		return FALSE;

	lua_export(Exported.GetLuaState());



	return TRUE;
}


extern "C" __declspec(dllexport) BOOL __stdcall CEPlugin_DisablePlugin(void)
{
	MessageBoxA(0, "disabled plugin", "Example C plugin", MB_OK);
	return TRUE;
}
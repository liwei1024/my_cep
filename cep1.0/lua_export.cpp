#include "stdafx.h"

extern BOOL maskingCEProcessPath(void);

int lua_maskingCEProcessPath(lua_State * L)
{
	maskingCEProcessPath();
	return 0;
}

void lua_export(lua_State * L)
{
	lua_register(L, "maskingCEProcessPath", lua_maskingCEProcessPath); // 
}
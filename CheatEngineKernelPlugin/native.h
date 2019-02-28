#pragma once
#include <ntifs.h>
#include <ntddk.h>


NTKERNELAPI
UCHAR *
PsGetProcessImageFileName(
	__in PEPROCESS Process
);
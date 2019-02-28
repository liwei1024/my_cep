#include "cekp.h"

UNICODE_STRING  GetFilePathByFileObject(PVOID FileObject)
{
	POBJECT_NAME_INFORMATION ObjetNameInfor;
	
	if (!NT_SUCCESS(IoQueryFileDosDeviceName((PFILE_OBJECT)FileObject, &ObjetNameInfor)))
	{
		//Êä³ö´íÎóÈÕÖ¾
	}
	return ObjetNameInfor->Name;
}

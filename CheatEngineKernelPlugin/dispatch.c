#include "cekp.h"


NTSTATUS Dispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);



	return STATUS_SUCCESS;
}
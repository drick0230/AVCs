#include "DevicesManager.h"
/////////////////////////// Constructors /////////////////////////////////////
DevicesManager::DevicesManager() {}

/////////////////////////// Functions /////////////////////////////////////
void DevicesManager::ShowDevices() {
#if _WIN32
	UINT32 count = 0;

	IMFAttributes* pConfig = NULL;
	IMFActivate** ppDevices = NULL;
	// Initialize Media Foundation
	HRESULT hr = MFStartup(MF_VERSION);

	// Create an attribute store to hold the search criteria.
	hr = MFCreateAttributes(&pConfig, 1);

	// Request video capture devices.
	if (SUCCEEDED(hr))
	{
		hr = pConfig->SetGUID(
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID
		);
	}

	// Enumerate the devices,
	if (SUCCEEDED(hr))
	{
		hr = MFEnumDeviceSources(pConfig, &ppDevices, &count);
	}
	count = count;
	if (SUCCEEDED(hr))
	{
		if (count > 0)
		{
			WCHAR* devName = NULL;
			UINT32 length;
			hr = ppDevices[0]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &devName, &length);
			length = length;
		}
		else
		{
			//hr = MF_E_NOT_FOUND;
		}
	}
	//// Create a media source for the first device in the list.
	//if (SUCCEEDED(hr))
	//{
	//    if (count > 0)
	//    {
	//        hr = ppDevices[0]->ActivateObject(IID_PPV_ARGS(ppSource));
	//    }
	//    else
	//    {
	//        hr = MF_E_NOT_FOUND;
	//    }
	//}

	for (DWORD i = 0; i < count; i++)
	{
		ppDevices[i]->Release();
	}
	CoTaskMemFree(ppDevices);
#endif //_WIN32

}
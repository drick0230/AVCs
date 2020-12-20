#include "DevicesManager.h"
/////////////////////////// Constructors /////////////////////////////////////
DevicesManager::DevicesManager() :
#if _WIN32
	audioCaptureDevices(NULL),
	audioRenderDevices(NULL),
	videoCaptureDevices(NULL),
	pConfig(NULL),
	hr(NULL),
	audioCaptureDatas(NULL),
	audioOutDatas(NULL),
#endif //_WIN32
	nbAudioCaptureDevices(0),
	nbAudioRenderDevices(0),
	nbVideoCaptureDevices(0) {
	// Initialize Devices Manager
#if _WIN32
	hr = MFStartup(MF_VERSION);	// Initialize Media Foundation
	hr = MFCreateAttributes(&pConfig, 1); // Create an attribute store to hold the search criteria.
#endif //_WIN32
}


DevicesManager::~DevicesManager() {
	ClearDevices();
#if _WIN32
	SafeRelease(&pConfig);

	// Shut down Media Foundation.
	MFShutdown();
#endif //_WIN32
}
/////////////////////////// Functions /////////////////////////////////////
#pragma region EnumerateDevices
void DevicesManager::EnumerateDevices(const unsigned int _devicesType) {
#if _WIN32
	// Audio capture devices
	if (SUCCEEDED(hr) && (_devicesType == DevicesTypes::AUD_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL))
		EnumerateAudioCaptureDevices();

	// Audio render devices
	if (SUCCEEDED(hr) && (_devicesType == DevicesTypes::AUD_REND || _devicesType == DevicesTypes::BOTH_REND || _devicesType == DevicesTypes::ALL))
		EnumerateAudioRenderDevices();

	// Video capture devices
	if (SUCCEEDED(hr) && (_devicesType == DevicesTypes::VID_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL))
		EnumerateVideoCaptureDevices();
#endif //_WIN32
}

void DevicesManager::EnumerateAudioCaptureDevices() {
#if _WIN32
	hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID); 	// Request audio capture devices.
	if (SUCCEEDED(hr))
		hr = MFEnumDeviceSources(pConfig, &audioCaptureDevices, &nbAudioCaptureDevices); 	// Enumerate the audio capture devices,
#endif //_WIN32

}

void DevicesManager::EnumerateAudioRenderDevices() {
#if _WIN32
	IMMDeviceEnumerator* pEnum = NULL;      // Audio device enumerator.

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);	// Create the device enumerator.
	if (SUCCEEDED(hr))
		hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &audioRenderDevices);	// Enumerate the rendering devices.
	SafeRelease(&pEnum);
	audioRenderDevices->GetCount(&nbAudioRenderDevices); // Get the number of audio render devices
#endif //_WIN32
}

void DevicesManager::EnumerateVideoCaptureDevices() {
#if _WIN32
	hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID); 	// Request video capture devices.
	if (SUCCEEDED(hr))
		hr = MFEnumDeviceSources(pConfig, &videoCaptureDevices, &nbVideoCaptureDevices); 	// Enumerate the video capture devices,
#endif //_WIN32

}
#pragma endregion
#pragma region SelectSource
void DevicesManager::SelectSource(const unsigned int _devicesType, const unsigned int _deviceID) {
	if (_devicesType == DevicesTypes::AUD_CAPT)
	{
		SelectAudioCaptureSource(_deviceID); // Select the audio capture device datas as source for the audio output device
	}
	else if (_devicesType == DevicesTypes::VID_CAPT) {
		//SelectVideoCaptureSource(_deviceID); // Return the name of the video device
	}
	else {
		std::cerr << "_devicesType cant be BOTH in std::string DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
		throw "_devicesType cant be BOTH in std::string DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
	}
}

void DevicesManager::SelectAudioCaptureSource(const unsigned int _deviceID) {
#if _WIN32
	hr = MFCreateDeviceSource(audioCaptureDevices[_deviceID], &audioCaptureDatas);
	if (SUCCEEDED(hr))
		hr = MFCreateSourceReaderFromMediaSource(audioCaptureDatas, NULL, &audioOutDatas);
	else
		throw hr;

	if (!SUCCEEDED(hr))
	{
		std::cerr << hr;
		throw hr;
	}
#endif //_WIN32
}

#pragma endregion


void DevicesManager::ClearDevices(const unsigned int _devicesType) {
#if _WIN32
	if (_devicesType == DevicesTypes::AUD_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL) {
		// Release Audio capture Devices
		for (unsigned int _i = 0; _i < nbAudioCaptureDevices; _i++)
		{
			audioCaptureDevices[_i]->Release();
		}
		CoTaskMemFree(audioCaptureDevices);
		SafeRelease(&audioCaptureDatas);
	}

	if (_devicesType == DevicesTypes::AUD_REND || _devicesType == DevicesTypes::BOTH_REND || _devicesType == DevicesTypes::ALL) {
		SafeRelease(&audioRenderDevices);
		SafeRelease(&audioOutDatas);
	}

	if (_devicesType == DevicesTypes::VID_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL) {
		// Release Video Devices
		for (unsigned int _i = 0; _i < nbVideoCaptureDevices; _i++)
		{
			videoCaptureDevices[_i]->Release();
		}
		CoTaskMemFree(videoCaptureDevices);
	}
#endif //_WIN32
}

#pragma region GetDevicesName
std::wstring DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID) {
	if (_devicesType == DevicesTypes::AUD_CAPT)
	{
		//return GetAudioCaptureDevicesName(_deviceID); // Return the name of the audio capture device
		return GetIMFActivateName(_devicesType, _deviceID, audioCaptureDevices);
	}
	else if (_devicesType == DevicesTypes::AUD_REND) {
		return GetAudioRenderDevicesName(_deviceID); // Return the name of the video device
	}
	else if (_devicesType == DevicesTypes::VID_CAPT) {
		//return GetVideoCaptureDevicesName(_deviceID); // Return the name of the video device
		return GetIMFActivateName(_devicesType, _deviceID, videoCaptureDevices);
	}
	else {
		std::cerr << "_devicesType cant be BOTH or ALL in std::string DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
		throw "_devicesType cant be BOTH or ALL in std::string DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
	}

	return ToWstring("Not Found"); // The device ID or the device type is invalid
}

#if _WIN32
std::wstring DevicesManager::GetIMFActivateName(const unsigned int _devicesType, const unsigned int _deviceID, IMFActivate** _devices) {
	std::wstring _returnDeviceName;
	_returnDeviceName.clear();

	// Test if the device ID is available
	bool _err = false;
	switch (_devicesType)
	{
	case DevicesTypes::AUD_CAPT:
		if (_deviceID >= nbAudioCaptureDevices)
			_err = true;
		break;
	case DevicesTypes::AUD_REND:
		if (_deviceID >= nbAudioRenderDevices)
			_err = true;
		break;
	case DevicesTypes::VID_CAPT:
		if (_deviceID >= nbVideoCaptureDevices)
			_err = true;
		break;
	default:
		break;
	}

	if (_err) {
		std::cerr << "deviceID > nbAudioDevices in std::string DevicesManager::GetAudioDevicesName(const unsigned int _deviceID)";
		throw "deviceID > nbAudioDevices in std::string DevicesManager::GetAudioDevicesName(const unsigned int _deviceID)";
		return ToWstring("Not Found");
	}

	wchar_t* _deviceName = NULL;
	unsigned int _length; // Length of _deviceName without '\0'
	hr = _devices[0]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &_deviceName, &_length);

	_returnDeviceName.assign(_deviceName);
	//SafeRelease(&_deviceName);

	return _returnDeviceName; // Cant directly return _deviceName 'cause of memory leak
}
#endif //_WIN32

std::wstring DevicesManager::GetAudioRenderDevicesName(const unsigned int _deviceID) {
	std::wstring _returnDeviceName;
	_returnDeviceName.clear();

	if (_deviceID < nbAudioRenderDevices)
	{
#if _WIN32
		IMMDevice* pDevice = NULL;
		IPropertyStore* pProps = NULL; // To contain the properties of the device
		PROPVARIANT _deviceName; // Propvariant that will contain the device name
		PropVariantInit(&_deviceName); // Initialize container for property value.
		hr = audioRenderDevices->Item(_deviceID, &pDevice); // Get the device with the _deviceID
		hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
		hr = pProps->GetValue(PKEY_Device_FriendlyName, &_deviceName); // Get the endpoint's friendly-name property.

		// PROPVARIANT to std::string	
		_returnDeviceName.assign(_deviceName.pwszVal);

		SafeRelease(&pDevice);
		SafeRelease(&pProps);
		PropVariantClear(&_deviceName);
#endif //_WIN32
	}
	else
	{
		std::cerr << "deviceID > nbAudioDevices in std::string DevicesManager::GetAudioDevicesName(const unsigned int _deviceID)";
		throw "deviceID > nbAudioDevices in std::string DevicesManager::GetAudioDevicesName(const unsigned int _deviceID)";
	}

	return _returnDeviceName; // Cant directly return _deviceName 'cause of memory leak
}
#pragma endregion

template <class T>
void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}
#pragma once
#include <locale>
#include <codecvt>
#include <string>

#include "Console.h"

#if _WIN32
#include <mfapi.h> // Media Foundation function
#include <mfidl.h> // IMFMediaSource
#include <mfreadwrite.h> // IMFSourceReader
#include <mmdeviceapi.h> // IMMDevice
#include <Functiondiscoverykeys_devpkey.h> // IMMDevice property Ex: PKEY_Device_FriendlyName
#endif //_WIN32

// Class Prototype
namespace DevicesTypes {
	const enum DEVICES_TYPES {AUD_CAPT, AUD_REND, VID_CAPT, BOTH_CAPT, BOTH_REND, ALL};
}

class DevicesManager {
private:
	// Windows API
#if _WIN32
	// Private Windows API Variables
	IMFActivate** audioCaptureDevices; // Array of pointer of audio capture devices
	IMMDeviceCollection* audioRenderDevices;   // Audio device collection. (Kinda like an array of Audio Render Devices)
	IMFActivate** videoCaptureDevices; // Array of pointer of video capture devices

	IMFAttributes* pConfig; // Attribute store to store the founded devices
	IMFMediaSource* audioCaptureDatas; // Datas of the selected audio capture device
	IMFSourceReader* audioOutDatas;

	HRESULT hr;
	// Private Windows API Functions
	std::wstring GetIMFActivateName(const unsigned int _devicesType, const unsigned int _deviceID, IMFActivate** _devices); // Return the name of a IMFActivate (audio/video capture device)
#endif //_WIN32
	// Private Variables
	unsigned int nbAudioCaptureDevices;
	unsigned int nbAudioRenderDevices;
	unsigned int nbVideoCaptureDevices;

	// Private Functions
	void EnumerateAudioCaptureDevices();
	void EnumerateAudioRenderDevices();
	void EnumerateVideoCaptureDevices();

	std::wstring GetAudioRenderDevicesName(const unsigned int _deviceID); // Return the name of the audio render device

	void SelectAudioCaptureSource(const unsigned int _deviceID); // Select the audio capture device datas as source for the audio output device
public:
	// Variables

	// Constructors
	DevicesManager();
	// Destructors
	~DevicesManager();

	// Functions
	void InitializeDevicesManager();
	void EnumerateDevices(const unsigned int _devicesType = DevicesTypes::ALL);
	std::wstring GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID = 0); // Return the name of the device

	void SelectSource(const unsigned int _devicesType, const unsigned int _deviceID = 0); // Select the capture device datas as source for an output device

	void ClearDevices(const unsigned int _devicesType = DevicesTypes::ALL);
};


template <class T>
static void SafeRelease(T** ppT);

static std::wstring ToWstring(std::string str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> strconverter;
	return strconverter.from_bytes(str);
}

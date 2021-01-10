#pragma once
//#include <locale>
//#include <codecvt>
//#include <string>

#include "Console.h"
#include "general.h"

#include <mfapi.h> // Media Foundation function
#include <mfidl.h> // MF_DEVSOURCE_ATTRIBUTE
//#include <mfreadwrite.h> // IMFSourceReader
#include <mmdeviceapi.h> // IMMDevice
#include <Functiondiscoverykeys_devpkey.h> // IMMDevice property Ex: PKEY_Device_FriendlyName
//// Core Audio API
//#include <audioclient.h> // WASAPI
//#include <audiopolicy.h> // WASAPI
//#include <AudioSessionTypes.h> // Constants define for Core Audio
//
//#include <initguid.h> // include this for DEFINE_GUID to create definition, not declaration
//
//DEFINE_MEDIATYPE_GUID(MFAudioFormat_PCM, WAVE_FORMAT_PCM);
//DEFINE_MEDIATYPE_GUID(MFAudioFormat_Float, WAVE_FORMAT_IEEE_FLOAT);
//DEFINE_GUID(MF_MT_SUBTYPE, 0xf7e34c9a, 0x42e8, 0x4714, 0xb7, 0x4b, 0xcb, 0x29, 0xd7, 0x2c, 0x35, 0xe5);
//#define MF_E_NO_MORE_TYPES 0xc00d36b9

#pragma region Device
namespace DevicesTypes {
	enum DEVICES_TYPES { AUD_CAPT, AUD_REND, VID_CAPT, BOTH_CAPT, BOTH_REND, ALL };
}

class Device {
protected:
	IMMDevice* device; // Pointer to an IMMDevice of a Device
	IMFActivate* activate; // Pointer to an IMFActivate of a Device (Not for Audio Render Device)

	Device(const unsigned int _deviceType, IMFActivate* _activate);
	Device(const unsigned int _deviceType, IMMDevice* _device);
public:
	unsigned int deviceType;
	~Device();

	virtual std::wstring GetName(HRESULT* hr = NULL);
	std::wstring GetId(HRESULT* hr = NULL);

	void* Activate(REFIID riid, HRESULT* hr = NULL);
};

// Childs of Device
class AudioCaptureDevice : public Device {
private:

public:
	AudioCaptureDevice(IMFActivate* _activate = NULL);
	AudioCaptureDevice(IMMDevice* _device = NULL);
};

class VideoCaptureDevice : public Device {
private:

public:
	VideoCaptureDevice(IMFActivate* _activate = NULL);
	VideoCaptureDevice(IMMDevice* _device = NULL);
};

// Childs of RenderDevice
class AudioRenderDevice : public Device {
private:

public:
	AudioRenderDevice(IMMDevice* _device = NULL);
};
#pragma endregion // Device

class DevicesManager {
private:
	// Windows API
#if _WIN32
	// Private Windows API Variables
	static HRESULT hr;
	// Private Windows API Functions
	static void EnumerateIMFActivates(const unsigned int _devicesType);
	static void EnumerateIMMDevices(const unsigned int _devicesType);
#endif //_WIN32
	// Private Variables
	static unsigned int nbAudioCaptureDevices;
	static unsigned int nbAudioRenderDevices;
	static unsigned int nbVideoCaptureDevices;

	// Private Functions
	//std::wstring GetAudioRenderDevicesName(const unsigned int _deviceID); // Return the name of the audio render device

	static void SelectAudioCaptureSource(const unsigned int _deviceID); // Select the audio capture device datas as source for the audio output device
public:
	// Public Variables
	static std::vector<AudioCaptureDevice> audioCaptureDevices;
	static std::vector<AudioRenderDevice> audioRenderDevices;
	static std::vector<VideoCaptureDevice> videoCaptureDevices;

	// Public Functions
	static void Initialize();
	static void Uninitialize();
	static void EnumerateDevices(const unsigned int _devicesType = DevicesTypes::ALL);
	static std::wstring GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID = 0); // Return the name of the device

	static void ClearDevices(const unsigned int _devicesType = DevicesTypes::ALL);
};
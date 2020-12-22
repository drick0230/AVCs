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
// Core Audio API
#include <audioclient.h> // WASAPI
#include <initguid.h> // include this for DEFINE_GUID to create definition, not declaration
#endif //_WIN32

// Class Prototype
// Device
#pragma region Device
namespace DevicesTypes {
	enum DEVICES_TYPES { AUD_CAPT, AUD_REND, VID_CAPT, BOTH_CAPT, BOTH_REND, ALL };
}

class Device {
protected:
public:
	unsigned int deviceType;
	virtual std::wstring GetName(HRESULT* hr = NULL) = 0;
};

class CaptureDevice : public Device {
protected:
	CaptureDevice(const unsigned int _devicesType, IMFActivate* _activate);
public:
	~CaptureDevice();
	IMFActivate* activate; // Pointer of an IMFActivate of an Audio Capture Device
	std::wstring GetName(HRESULT* hr = NULL);
};

class AudioCaptureDevice : public CaptureDevice {
private:

public:
	AudioCaptureDevice(IMFActivate* _activate = NULL);
};

class VideoCaptureDevice : public CaptureDevice {
private:

public:
	VideoCaptureDevice(IMFActivate* _activate = NULL);
};
#pragma endregion

class WASAPI {
private:
	IAudioClient* audioClient;
public:
	WASAPI();
	~WASAPI();
	void PlayAudioCaptureDatas();
};

class DevicesManager {
private:
	// Windows API
#if _WIN32
	// Private Windows API Variables
	IMMDeviceCollection* audioRenderDevices;   // Audio device collection. (Kinda like an array of Audio Render Devices)

	IMFAttributes* pConfig; // Attribute store to store the founded devices
	IMFMediaSource* audioCaptureDatas; // Datas of the selected audio capture device
	IMFSourceReader* audioRenderDatas; // Handles datas from source (capture devices) to the Audio Render Device
	IMFMediaSink* audioRenderListener;             // Streaming audio renderer (SAR)

	WASAPI wasapi;

	//IMFMediaSink* audioRenderListener;             // Streaming audio renderer (SAR)

	HRESULT hr;
	// Private Windows API Functions
	std::wstring GetIMFActivateName(const unsigned int _devicesType, const unsigned int _deviceID, IMFActivate** _devices); // Return the name of a IMFActivate (audio/video capture device)
	void EnumerateCaptureDevices(const unsigned int _devicesType);
#endif //_WIN32
	// Private Variables
	unsigned int nbAudioCaptureDevices;
	unsigned int nbAudioRenderDevices;
	unsigned int nbVideoCaptureDevices;

	// Private Functions
	std::wstring GetAudioRenderDevicesName(const unsigned int _deviceID); // Return the name of the audio render device

	void SelectAudioCaptureSource(const unsigned int _deviceID); // Select the audio capture device datas as source for the audio output device
public:
	std::vector<AudioCaptureDevice> audioCaptureDevices;
	std::vector<VideoCaptureDevice> videoCaptureDevices;

	// Variables
	IMFMediaSession* _mediaSession; // Control the audio (Play/Pause/Stop)

	// Constructors
	DevicesManager();
	// Destructors
	~DevicesManager();

	// Functions
	void InitializeDevicesManager();
	void EnumerateDevices(const unsigned int _devicesType = DevicesTypes::ALL);
	std::wstring GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID = 0); // Return the name of the device

	void PlayAudioCaptureDatasMediaFoundation();

	void SaveAudioCaptureDatas();
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

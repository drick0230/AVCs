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
	unsigned int deviceType = 0;
	virtual std::wstring GetName(HRESULT* hr = NULL) = 0;
};

// Childs of Device
class CaptureDevice : public Device {
protected:
	IMFActivate* activate; // Pointer to an IMFActivate of an Audio Capture Device
	CaptureDevice(const unsigned int _devicesType, IMFActivate* _activate);
public:
	~CaptureDevice();
	std::wstring GetName(HRESULT* hr = NULL);
	void* Activate(REFIID riid, HRESULT* hr = NULL);
};

class RenderDevice : public Device {
protected:
	IMMDevice* device; // Pointer to an IMMDevice of an Audio Render Device
	RenderDevice(const unsigned int _devicesType, IMMDevice *_device);
public:
	~RenderDevice();
	std::wstring GetName(HRESULT* hr = NULL);
	std::wstring GetId(HRESULT* hr = NULL);
};

// Childs of CaptureDevice
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

// Childs of RenderDevice
class AudioRenderDevice : public RenderDevice {
private:

public:
	AudioRenderDevice(IMMDevice* _device = NULL);
};
#pragma endregion // Device

//Audio API
#pragma region AudioAPI
class WASAPI {
private:
	IAudioClient* audioClient;
public:
	WASAPI();
	~WASAPI();
	void PlayAudioCaptureDatas();
};

class MediaSession {
private:
	IMFMediaSession *mediaSession; // Control the audio (Play/Pause/Stop)
	IMFMediaSource *audioCaptureDevice;
	IMFActivate* audioRenderDevice;
public:
	MediaSession(AudioCaptureDevice &_audioCaptureDevice, AudioRenderDevice &_audioRenderDevice, HRESULT* hr = NULL);
	MediaSession(HRESULT* hr = NULL);
	~MediaSession();
	void Initialize(HRESULT* hr = NULL);

	void SetActiveDevice(AudioCaptureDevice& _audioCaptureDevice, HRESULT* hr = NULL);
	void SetActiveDevice(AudioRenderDevice& _audioRenderDevice, HRESULT* hr = NULL);

	void PlayAudioCaptureDatas(HRESULT* hr = NULL);
};
#pragma endregion //AudioAPI


class DevicesManager {
private:
	// Windows API
#if _WIN32
	// Private Windows API Variables
	WASAPI wasapi;

	HRESULT hr;
	// Private Windows API Functions
	void EnumerateCaptureDevices(const unsigned int _devicesType);
	void EnumerateAudioRenderDevices(const unsigned int _devicesType);
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
	std::vector<AudioRenderDevice> audioRenderDevices;
	std::vector<VideoCaptureDevice> videoCaptureDevices;

	MediaSession mediaSession;

	// Variables
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

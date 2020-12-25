#pragma once
#include <locale>
#include <codecvt>
#include <string>

#include "Console.h"

#include <mfapi.h> // Media Foundation function
#include <mfidl.h> // IMFMediaSource
#include <mfreadwrite.h> // IMFSourceReader
#include <mmdeviceapi.h> // IMMDevice
#include <Functiondiscoverykeys_devpkey.h> // IMMDevice property Ex: PKEY_Device_FriendlyName
// Core Audio API
#include <audioclient.h> // WASAPI
#include <initguid.h> // include this for DEFINE_GUID to create definition, not declaration

DEFINE_MEDIATYPE_GUID(MFAudioFormat_PCM, WAVE_FORMAT_PCM);
DEFINE_MEDIATYPE_GUID(MFAudioFormat_Float, WAVE_FORMAT_IEEE_FLOAT);
DEFINE_GUID(MF_MT_SUBTYPE, 0xf7e34c9a, 0x42e8, 0x4714, 0xb7, 0x4b, 0xcb, 0x29, 0xd7, 0x2c, 0x35, 0xe5);
#define MF_E_NO_MORE_TYPES 0xc00d36b9

// Class Prototype
// Device
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

class SourceReader_SinkWritter {
	IMFMediaSource* audioCaptureSource;
	IMFSourceReader* audioCaptureDatas;

	IMFMediaSink* audioRenderSink;
	IMFSinkWriter* audioRenderDatas;
public:
	SourceReader_SinkWritter(HRESULT* hr = NULL);
	~SourceReader_SinkWritter();

	void SetActiveDevice(AudioCaptureDevice& _audioCaptureDevice, HRESULT* hr = NULL);
	void SetActiveDevice(AudioRenderDevice& _audioRenderDevice, HRESULT* hr = NULL);

	void PlayAudioCaptureDatas(HRESULT* hr = NULL);
};

class MediaSession {
private:
	IMFMediaSession *mediaSession; // Control the audio (Play/Pause/Stop)
	IMFMediaSource *audioCaptureSource;

	IMFActivate* audioRenderSource;
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
	void EnumerateIMFActivates(const unsigned int _devicesType);
	void EnumerateIMMDevices(const unsigned int _devicesType);
#endif //_WIN32
	// Private Variables
	unsigned int nbAudioCaptureDevices;
	unsigned int nbAudioRenderDevices;
	unsigned int nbVideoCaptureDevices;

	// Private Functions
	//std::wstring GetAudioRenderDevicesName(const unsigned int _deviceID); // Return the name of the audio render device

	void SelectAudioCaptureSource(const unsigned int _deviceID); // Select the audio capture device datas as source for the audio output device
public:
	// Public Variables
	std::vector<AudioCaptureDevice> audioCaptureDevices;
	std::vector<AudioRenderDevice> audioRenderDevices;
	std::vector<VideoCaptureDevice> videoCaptureDevices;

	SourceReader_SinkWritter sr_sw;
	MediaSession mediaSession;

	// Constructors
	DevicesManager();
	// Destructors
	~DevicesManager();

	// Public Functions
	void InitializeDevicesManager();
	void EnumerateDevices(const unsigned int _devicesType = DevicesTypes::ALL);
	std::wstring GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID = 0); // Return the name of the device

	void ClearDevices(const unsigned int _devicesType = DevicesTypes::ALL);
};


template <class T>
static void SafeRelease(T** ppT);

static std::wstring ToWstring(std::string str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> strconverter;
	return strconverter.from_bytes(str);
}

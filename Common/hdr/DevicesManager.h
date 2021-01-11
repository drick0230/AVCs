#pragma once
//#include <locale>
//#include <codecvt>
//#include <string>

#include "Console.h"
#include "general.h"

#include <mfapi.h> // Media Foundation function
#include <mfidl.h> // MF_DEVSOURCE_ATTRIBUTE
#include <mmdeviceapi.h> // IMMDevice
#include <Functiondiscoverykeys_devpkey.h> // IMMDevice property Ex: PKEY_Device_FriendlyName
#include <mfreadwrite.h> // IMFSourceReader and IMFSinkWriter

#pragma region Device
class AudioDatas;

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
	IMFMediaSink* mediaSink;
	IMFSinkWriter* sinkWritter;

public:
	AudioRenderDevice(IMMDevice* _device = NULL);

	// Set the media type of the datas to be played
	void SetInputMediaType(std::vector<unsigned char> _mediaTypeDatas);

	// Call SetInputMediaType once before playing audio datas
	void Play(AudioDatas _audioDatas);
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
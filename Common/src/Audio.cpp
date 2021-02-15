/**
 * @file	Audio.cpp
 * @author	Dérick Gagnon
 * @date	2021-02-14
 */

#include "Audio.h"

Packet& Packet::operator << (AudioDatas _audioDatas) {
	*this << _audioDatas.duration;
	*this << _audioDatas.time;
	*this << _audioDatas.datas;

	return *this;
}

Packet& Packet::operator >> (AudioDatas& _audioDatas) {
	*this >> _audioDatas.duration;
	*this >> _audioDatas.time;
	*this >> _audioDatas.datas;

	return *this;
}

AudioTrack::AudioTrack(const std::wstring _deviceID) : mediaSink(NULL), sinkWritter(NULL) {
	IMFAttributes* pConfig; // Store the IMMDevice endpoint id
	HRESULT hr(S_OK);

	// Create the MediaSink from the IMMDevice
	if (SUCCEEDED(hr)) hr = MFCreateAttributes(&pConfig, 1); // Create an attribute store to hold the Audio Capture Device ID
	if (SUCCEEDED(hr)) hr = pConfig->SetString(MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID, _deviceID.c_str()); 	// Request audio capture devices
	if (SUCCEEDED(hr)) hr = MFCreateAudioRenderer(pConfig, &mediaSink);

	// Create the SinkWritter from the MediaSink
	if (SUCCEEDED(hr)) hr = MFCreateSinkWriterFromMediaSink(mediaSink, NULL, &sinkWritter);

	if (!SUCCEEDED(hr)) throw hr;
};

void AudioTrack::Release() {
	if (sinkWritter != NULL) {
		sinkWritter->Finalize();
		SafeRelease(&sinkWritter);
	}
	if (mediaSink != NULL) {
		mediaSink->Shutdown();
		SafeRelease(&mediaSink);
	}
}


#pragma region Device
/////////////////////////// Constructors /////////////////////////////////////
#pragma region Constructors
//Device
Device::Device(const unsigned int _deviceType, IMFActivate* _activate) : deviceType(_deviceType), activate(_activate), device(NULL) {}
Device::Device(const unsigned int _deviceType, IMMDevice* _device) : deviceType(_deviceType), activate(NULL), device(_device) {}

Device::~Device() {
	SafeRelease(&activate);
	SafeRelease(&device);
}

// Childs of Device
AudioCaptureDevice::AudioCaptureDevice(IMFActivate* _activate) : Device(DevicesTypes::AUD_CAPT, _activate), mediaSource(NULL), sourceReader(NULL) {}
AudioCaptureDevice::AudioCaptureDevice(IMMDevice* _device) : Device(DevicesTypes::AUD_CAPT, _device), mediaSource(NULL), sourceReader(NULL) {
	IMFAttributes* pConfig; // Store the search criteria for the devices
	HRESULT hr(S_OK);

	// Create the MediaSource from the IMMDevice
	if (SUCCEEDED(hr)) hr = MFCreateAttributes(&pConfig, 2); // Create an attribute store to hold the Audio Capture Device ID
	if (SUCCEEDED(hr)) hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID); 	// Request audio capture devices
	if (SUCCEEDED(hr)) hr = pConfig->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID, GetId(&hr).c_str()); 	// Request audio capture devices by ID
	if (SUCCEEDED(hr)) hr = MFCreateDeviceSource(pConfig, &mediaSource);

	// Create the SourceReader from the MediaSource
	if (SUCCEEDED(hr)) hr = MFCreateSourceReaderFromMediaSource(mediaSource, NULL, &sourceReader);

	SafeRelease(&pConfig);

	if (!SUCCEEDED(hr)) throw hr;
}

VideoCaptureDevice::VideoCaptureDevice(IMFActivate* _activate) : Device(DevicesTypes::VID_CAPT, _activate) {}
VideoCaptureDevice::VideoCaptureDevice(IMMDevice* _device) : Device(DevicesTypes::VID_CAPT, _device) {}

AudioRenderDevice::AudioRenderDevice(IMMDevice* _device) : Device(DevicesTypes::AUD_REND, _device), audioTracks(), nbTracks(0) { AddTrack(); /*Create the first track*/ }
AudioRenderDevice::~AudioRenderDevice() {
	// Release the audio tracks
	for (unsigned int _i = 0; _i < audioTracks.size(); _i++)
		audioTracks[_i].Release();

	// Call the Device destructor
	((Device*)this)->~Device();
}



#pragma endregion // Constructors

/////////////////////////// Functions /////////////////////////////////////
#pragma region Functions
std::wstring Device::GetName(HRESULT* hr) {
	std::wstring _returnDeviceName;

	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	if (activate != NULL) {
		// Is An IMFActivate
		WCHAR* _deviceName = NULL;
		unsigned int _length = 0; // Length of _deviceName without '\0'



		if (SUCCEEDED(*hr)) *hr = activate->GetStringLength(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &_length);
		_length++; // to include the '\0'
		_deviceName = new WCHAR[_length];
		if (_deviceName == NULL) *hr = E_OUTOFMEMORY;
		if (SUCCEEDED(*hr)) *hr = activate->GetString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, _deviceName, _length, &_length);

		if (SUCCEEDED(*hr)) _returnDeviceName.assign(_deviceName);
		else throw _deviceName;

		unsigned int _i = 0;

		// Release Variables
		delete[] _deviceName;
	}
	else if (device != NULL) {
		// Is An IMMDevice
		IPropertyStore* pProps = NULL; // To contain the properties of the device
		PROPVARIANT _deviceName; // Propvariant that will contain the device name

		bool _localHr = false;
		if (hr == NULL) {
			hr = new HRESULT(S_OK);
			_localHr = true;
		}


		PropVariantInit(&_deviceName); // Initialize container for property value.
		if (SUCCEEDED(*hr)) *hr = device->OpenPropertyStore(STGM_READ, &pProps);
		if (SUCCEEDED(*hr)) *hr = pProps->GetValue(PKEY_Device_FriendlyName, &_deviceName); // Get the endpoint's friendly-name property.

		// PROPVARIANT to std::string	
		_returnDeviceName.assign(_deviceName.pwszVal);

		// Release Variables
		SafeRelease(&pProps);
		PropVariantClear(&_deviceName);
	}
	else {
		throw "This Device's IMFActivate and IMMDevice are NULL. Missing something at the construction?";
	}

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;

	return _returnDeviceName; // Cant directly return _deviceName or the user will need to manually release it to prevent memory leak
}

void* Device::Activate(REFIID _riid, HRESULT* hr) {
	void* _return = NULL;
	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	if (SUCCEEDED(*hr)) *hr = activate->ActivateObject(_riid, &_return);

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;

	return _return;
}

std::wstring Device::GetId(HRESULT* hr) {
	std::wstring _returnDeviceID;

	if (device != NULL) {
		LPWSTR _deviceID = NULL;

		bool _localHr = false;
		if (hr == NULL) {
			hr = new HRESULT(S_OK);
			_localHr = true;
		}

		if (SUCCEEDED(*hr)) *hr = device->GetId(&_deviceID);

		if (_deviceID) _returnDeviceID.assign(_deviceID);

		if (!SUCCEEDED(*hr)) throw* hr;
		if (_localHr) delete hr;
		CoTaskMemFree(_deviceID);
	}
	else {
		_returnDeviceID = L"device is NULL";
		throw device;
	}


	return _returnDeviceID; // Cant directly return _deviceName or the user will need to manually release it to prevent memory leak
}

std::vector<unsigned char> AudioCaptureDevice::GetMediaTypeDatas() {
	HRESULT hr(S_OK);

	std::vector<unsigned char> _returnDatas;
	IMFMediaType* _mediaType = NULL;

	if (SUCCEEDED(hr)) hr = sourceReader->GetCurrentMediaType(0, &_mediaType); // Get the MediaType

	unsigned char* _datas;
	unsigned int _datasSize;

	if (SUCCEEDED(hr)) hr = MFGetAttributesAsBlobSize(_mediaType, &_datasSize);
	if (SUCCEEDED(hr)) _datas = new unsigned char[_datasSize];
	if (SUCCEEDED(hr)) hr = MFGetAttributesAsBlob(_mediaType, _datas, _datasSize);

	if (SUCCEEDED(hr)) {
		_returnDatas.reserve(_datasSize);
		for (unsigned int _i = 0; _i < _datasSize; _i++)
			_returnDatas.push_back(_datas[_i]);
	}

	if (SUCCEEDED(hr)) delete[] _datas;
	SafeRelease(&_mediaType);

	if (!SUCCEEDED(hr)) throw hr;
	return _returnDatas;
}
AudioDatas AudioCaptureDevice::Read() {
	HRESULT hr(S_OK);
	AudioDatas _returnAudioDatas;

	IMFSample* pSample = NULL;
	size_t  cSamples = 0;

	DWORD streamIndex = 0, streamStatus = 0;
	LONGLONG llTimeStamp = 0;

	if (SUCCEEDED(hr)) hr = sourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &streamStatus, &llTimeStamp, &pSample);
	if (SUCCEEDED(hr))
	{
		if (streamStatus & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			wprintf(L"\tEnd of stream\n");
		}
		if (streamStatus & MF_SOURCE_READERF_NEWSTREAM)
		{
			wprintf(L"\tNew stream\n");
		}
		if (streamStatus & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED)
		{
			wprintf(L"\tNative type changed\n");
		}
		if (streamStatus & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
		{
			wprintf(L"\tCurrent type changed\n");
		}
		if (streamStatus & MF_SOURCE_READERF_STREAMTICK)
		{
			wprintf(L"\tStream tick\n");
		}

		if (streamStatus & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED)
		{
			// The format changed. Reconfigure the decoder.
			throw "Format Changed";
		}

		if (pSample) {
			++cSamples; // Count how many sample are process (Not NULL)

			unsigned long _captureBufferLength; // How many datas in the buffer
			if (SUCCEEDED(hr)) hr = pSample->GetTotalLength(&_captureBufferLength);

			_returnAudioDatas.datas.resize(_captureBufferLength, 0); // Resize to the right amount of datas

			IMFMediaBuffer* _captureBuffer; // Copy of the contiguous buffer from the Capture Device Sample
			if (SUCCEEDED(hr)) hr = MFCreateMemoryBuffer(_captureBufferLength, &_captureBuffer);
			if (SUCCEEDED(hr)) hr = pSample->CopyToBuffer(_captureBuffer);

			//	Transfer Audio Datas from the buffer to the AudioDatas' datas
			unsigned char* _bufferPointer;

			if (SUCCEEDED(hr)) hr = _captureBuffer->Lock(&_bufferPointer, NULL, NULL);
			if (SUCCEEDED(hr)) for (unsigned long _i = 0; _i < _captureBufferLength; _i++)
				_returnAudioDatas.datas[_i] = _bufferPointer[_i];
			if (SUCCEEDED(hr)) hr = _captureBuffer->Unlock();

			if (SUCCEEDED(hr)) hr = pSample->GetSampleTime(&_returnAudioDatas.time); // Return the time
			if (SUCCEEDED(hr)) hr = pSample->GetSampleDuration(&_returnAudioDatas.duration); // Return the duration

			SafeRelease(&_captureBuffer);
		}
	}
	SafeRelease(&pSample);

	if (!SUCCEEDED(hr)) throw hr;

	return _returnAudioDatas;

}

void AudioRenderDevice::SetInputMediaType(std::vector<unsigned char> _mediaTypeDatas, unsigned int _track) {
	HRESULT hr(S_OK);

	// Vector of Char --> Media Type
	IMFMediaType* _mediaType;

	if (SUCCEEDED(hr)) hr = MFCreateMediaType(&_mediaType);
	if (SUCCEEDED(hr)) hr = MFInitAttributesFromBlob(_mediaType, _mediaTypeDatas.data(), _mediaTypeDatas.size());

	// Set the Media Type
	if (SUCCEEDED(hr)) hr = audioTracks[_track].sinkWritter->SetInputMediaType(0, _mediaType, NULL); // Need to send this information at the begining of a Room or when it change (IMPORTANT)
	if (SUCCEEDED(hr)) hr = audioTracks[_track].sinkWritter->BeginWriting(); // The SinkWriter is ready to receive and write datas

	SafeRelease(&_mediaType);

	if (!SUCCEEDED(hr)) throw hr;
}

void AudioRenderDevice::Play(AudioDatas _audioDatas, unsigned int _track) {
	HRESULT hr(S_OK);

	if (_audioDatas.datas.size() > 0) {
		// Create and fill the Buffer
		unsigned char* _bufferPointer;
		IMFMediaBuffer* _buffer;
		IMFSample* _sample;
		if (SUCCEEDED(hr)) hr = MFCreateMemoryBuffer(_audioDatas.datas.size(), &_buffer);

		if (SUCCEEDED(hr)) hr = _buffer->Lock(&_bufferPointer, NULL, NULL);
		if (SUCCEEDED(hr)) for (unsigned long _i = 0; _i < _audioDatas.datas.size(); _i++)
			_bufferPointer[_i] = _audioDatas.datas[_i];
		if (SUCCEEDED(hr)) hr = _buffer->SetCurrentLength(_audioDatas.datas.size());
		if (SUCCEEDED(hr)) hr = _buffer->Unlock();

		// Create the sample that will be read on the Audio Render Device
		if (SUCCEEDED(hr)) hr = MFCreateSample(&_sample);
		if (SUCCEEDED(hr))hr = _sample->AddBuffer(_buffer);

		if (SUCCEEDED(hr))hr = _sample->SetSampleDuration(_audioDatas.duration);
		if (SUCCEEDED(hr))hr = _sample->SetSampleTime(_audioDatas.time);

		// Read the sample
		if (SUCCEEDED(hr))hr = audioTracks[_track].sinkWritter->WriteSample(0, _sample);

		//audioRenderDatas->Finalize();

		SafeRelease(&_buffer);
		SafeRelease(&_sample);
	}

	if (!SUCCEEDED(hr)) throw hr;
}

void AudioRenderDevice::AddTrack() {
	audioTracks.emplace_back(GetId());
	nbTracks++;
}
#pragma endregion // Functions
#pragma endregion //Device

#pragma region DeviceManager

// Static Members
HRESULT DevicesManager::hr(S_OK);

std::vector<AudioCaptureDevice> DevicesManager::audioCaptureDevices;
std::vector<AudioRenderDevice> DevicesManager::audioRenderDevices;
std::vector<VideoCaptureDevice> DevicesManager::videoCaptureDevices;

unsigned int DevicesManager::nbAudioCaptureDevices(0);
unsigned int DevicesManager::nbAudioRenderDevices(0);
unsigned int DevicesManager::nbVideoCaptureDevices(0);

void DevicesManager::Initialize() {
	if (SUCCEEDED(hr)) hr = MFStartup(MF_VERSION);	// Initialize Media Foundation
	//if (SUCCEEDED(hr)) mediaSession.Initialize(&hr); 	// Initialize Media Session

	if (!SUCCEEDED(hr)) throw hr;
}

void DevicesManager::Uninitialize() {
	ClearDevices();
	MFShutdown();	// Shut down Media Foundation.
}

//
//DevicesManager::~DevicesManager() {
//	ClearDevices();
//#if _WIN32
//	MFShutdown();	// Shut down Media Foundation.
//#endif //_WIN32
//}
/////////////////////////// Functions /////////////////////////////////////
#pragma region EnumerateDevices
void DevicesManager::EnumerateDevices(const unsigned int _devicesType) {
#if _WIN32
	// Audio capture devices
	if (SUCCEEDED(hr) && (_devicesType == DevicesTypes::AUD_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL))
		EnumerateIMMDevices(DevicesTypes::AUD_CAPT);
	//EnumerateAudioCaptureDevices();

// Audio render devices
	if (SUCCEEDED(hr) && (_devicesType == DevicesTypes::AUD_REND || _devicesType == DevicesTypes::BOTH_REND || _devicesType == DevicesTypes::ALL))
		EnumerateIMMDevices(DevicesTypes::AUD_REND);

	// Video capture devices
	if (SUCCEEDED(hr) && (_devicesType == DevicesTypes::VID_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL))
		EnumerateIMFActivates(DevicesTypes::VID_CAPT);
	//EnumerateVideoCaptureDevices();
#endif //_WIN32
}

void DevicesManager::EnumerateIMFActivates(const unsigned int _devicesType) {
	IMFActivate** _activates = NULL; // Array of pointer of IMFActivate of Audio Capture Devices
	unsigned int* _nbDevices = NULL;
	GUID _deviceTypeGUID;
	IMFAttributes* pConfig; // Store the search criteria for the devices

	if (_devicesType == DevicesTypes::AUD_CAPT) {
		_deviceTypeGUID = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID;
		_nbDevices = &nbAudioCaptureDevices;
	}
	else if (_devicesType == DevicesTypes::VID_CAPT) {
		_deviceTypeGUID = MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID;
		_nbDevices = &nbVideoCaptureDevices;
	}
	else {
		throw _devicesType;
	}

	if (SUCCEEDED(hr)) hr = MFCreateAttributes(&pConfig, 1); // Create an attribute store to hold the search criteria.
	if (SUCCEEDED(hr)) hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, _deviceTypeGUID); 	// Request audio capture devices.
	if (SUCCEEDED(hr)) hr = MFEnumDeviceSources(pConfig, &_activates, _nbDevices); 	// Enumerate the audio capture devices,

	if (_activates != NULL) {
		if (_devicesType == DevicesTypes::AUD_CAPT) {
			audioCaptureDevices.clear();
			audioCaptureDevices.reserve(*_nbDevices);

			for (unsigned int _i = 0; _i < *_nbDevices; _i++)
				audioCaptureDevices.emplace_back(_activates[_i]);
		}
		else if (_devicesType == DevicesTypes::VID_CAPT) {
			videoCaptureDevices.clear();
			videoCaptureDevices.reserve(*_nbDevices);

			for (unsigned int _i = 0; _i < *_nbDevices; _i++)
				videoCaptureDevices.emplace_back(_activates[_i]);
		}
	}
	else throw _activates;

	if (!SUCCEEDED(hr)) throw hr;
	// Release Variables
	CoTaskMemFree(_activates); // Dont release the Activate in activates. If it was the case, the Capture Devices could not use them
}

void DevicesManager::EnumerateIMMDevices(const unsigned int _devicesType) {
	IMMDeviceCollection* _deviceCollection = NULL; // Array of IMMDevice : To contain the devices after the fill
	IMMDeviceEnumerator* _deviceEnum = NULL;      // Device enumerator : To fill the Device Collection
	IMMDevice* _device = NULL; // To temporary get a Device Collection item
	EDataFlow _searchDeviceArgument = eAll;

	if (_devicesType == DevicesTypes::AUD_CAPT) _searchDeviceArgument = eCapture;
	else if (_devicesType == DevicesTypes::AUD_REND) _searchDeviceArgument = eRender;
	else { throw _devicesType; hr = E_INVALIDARG; }

	if (SUCCEEDED(hr)) hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&_deviceEnum);	// Create the device enumerator.
	if (SUCCEEDED(hr)) hr = _deviceEnum->EnumAudioEndpoints(_searchDeviceArgument, DEVICE_STATE_ACTIVE, &_deviceCollection);	// Enumerate the rendering devices.
		//SafeRelease(&pEnum);

	if (SUCCEEDED(hr))
		if (_devicesType == DevicesTypes::AUD_CAPT) {
			_deviceCollection->GetCount(&nbAudioCaptureDevices); // Get the number of audio render devices

			audioCaptureDevices.clear();
			audioCaptureDevices.reserve(nbAudioCaptureDevices);

			for (unsigned int _i = 0; _i < nbAudioCaptureDevices; _i++) {
				_deviceCollection->Item(_i, &_device);
				audioCaptureDevices.emplace_back(_device);
			}
		}
		else if (_devicesType == DevicesTypes::AUD_REND) {
			_deviceCollection->GetCount(&nbAudioRenderDevices); // Get the number of audio render devices

			audioRenderDevices.clear();
			audioRenderDevices.reserve(nbAudioRenderDevices);

			for (unsigned int _i = 0; _i < nbAudioRenderDevices; _i++) {
				_deviceCollection->Item(_i, &_device);
				audioRenderDevices.emplace_back(_device);
			}
		}



	if (!SUCCEEDED(hr)) throw hr;
}
#pragma endregion // EnumerateDevices

void DevicesManager::ClearDevices(const unsigned int _devicesType) {
#if _WIN32
	// Release Audio Capture Devices
	if (_devicesType == DevicesTypes::AUD_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL) {
		for (unsigned int _i = 0; _i < nbAudioCaptureDevices; _i++)
			Device(audioCaptureDevices[0]).~Device();
		audioCaptureDevices.clear();
	}

	// Release Audio Render Devices
	if (_devicesType == DevicesTypes::AUD_REND || _devicesType == DevicesTypes::BOTH_REND || _devicesType == DevicesTypes::ALL) {
		for (unsigned int _i = 0; _i < nbAudioRenderDevices; _i++)
			audioRenderDevices[0].~AudioRenderDevice();
		audioRenderDevices.clear();
	}

	// Release Video Capture Devices
	if (_devicesType == DevicesTypes::AUD_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL) {
		for (unsigned int _i = 0; _i < nbVideoCaptureDevices; _i++)
			Device(videoCaptureDevices[0]).~Device();
		videoCaptureDevices.clear();
	}
#endif //_WIN32
}

std::wstring DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID) {
	if (_devicesType == DevicesTypes::AUD_CAPT)
	{
		if (_deviceID < nbAudioCaptureDevices)
			return audioCaptureDevices[_deviceID].GetName(&hr); // Return the name of the audio capture device
		else {
			std::cerr << "deviceID >= nbAudioCaptureDevices in std::wstring DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
			throw "deviceID >= nbAudioCaptureDevices in std::wstring DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
		}
	}
	else if (_devicesType == DevicesTypes::AUD_REND) {
		if (_deviceID < nbAudioRenderDevices)
			return audioRenderDevices[_deviceID].GetName(&hr);
		else {
			std::cerr << "deviceID >= nbAudioRenderDevices in std::wstring DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
			throw "deviceID >= nbAudioRenderDevices in std::wstring DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
		}
	}
	else if (_devicesType == DevicesTypes::VID_CAPT) {
		if (_deviceID < nbVideoCaptureDevices)
			return videoCaptureDevices[_deviceID].GetName(&hr); // Return the name of the video device
		else {
			std::cerr << "deviceID >= nbVideoCaptureDevices in std::wstring DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
			throw "deviceID >= nbVideoCaptureDevices in std::wstring DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
		}
	}
	else {
		std::cerr << "_devicesType cant be BOTH or ALL in std::string DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
		throw "_devicesType cant be BOTH or ALL in std::string DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
	}

	return L"Not Found"; // The device ID or the device type is invalid
}
#pragma endregion //DeviceManager
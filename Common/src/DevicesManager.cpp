#include "DevicesManager.h"

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
AudioCaptureDevice::AudioCaptureDevice(IMFActivate* _activate) : Device(DevicesTypes::AUD_CAPT, _activate) {}
AudioCaptureDevice::AudioCaptureDevice(IMMDevice* _device) : Device(DevicesTypes::AUD_CAPT, _device) {}

VideoCaptureDevice::VideoCaptureDevice(IMFActivate* _activate) : Device(DevicesTypes::VID_CAPT, _activate) {}
VideoCaptureDevice::VideoCaptureDevice(IMMDevice* _device) : Device(DevicesTypes::VID_CAPT, _device) {}

AudioRenderDevice::AudioRenderDevice(IMMDevice* _device) : Device(DevicesTypes::AUD_REND, _device) {}


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

	if(device != NULL){
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
		_returnDeviceID = L"device is NUL";
		throw device;
	}


	return _returnDeviceID; // Cant directly return _deviceName or the user will need to manually release it to prevent memory leak
}

#pragma endregion // Functions
#pragma endregion //Device


#pragma region AudioAPI
WASAPI::WASAPI() {

}

WASAPI::~WASAPI() {

}

MediaSession::MediaSession(HRESULT* hr) : mediaSession(NULL), audioCaptureDevice(NULL), audioRenderDevice(NULL) {}

MediaSession::MediaSession(AudioCaptureDevice& _audioCaptureDevice, AudioRenderDevice& _audioRenderDevice, HRESULT* hr) : MediaSession(hr) {
	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	Initialize(hr);

	if (SUCCEEDED(*hr)) SetActiveDevice(_audioCaptureDevice, hr);
	if (SUCCEEDED(*hr)) SetActiveDevice(_audioRenderDevice, hr);

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;
}

MediaSession::~MediaSession() {
	mediaSession->Close(); // Close Media Session
	audioCaptureDevice->Shutdown(); // Shut down IMFMediaSource
	mediaSession->Shutdown(); // Shut down Media Session

	SafeRelease(&audioCaptureDevice);
	SafeRelease(&audioRenderDevice);
}

void MediaSession::Initialize(HRESULT* hr) {
	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	if (SUCCEEDED(*hr)) *hr = MFCreateMediaSession(NULL, &mediaSession); // Initialize the Media Session

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;
}

void MediaSession::SetActiveDevice(AudioCaptureDevice& _audioCaptureDevice, HRESULT* hr) {
	IMFAttributes* pConfig; // Store the search criteria for the devices

	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	if (SUCCEEDED(*hr)) *hr = MFCreateAttributes(&pConfig, 2); // Create an attribute store to hold the Audio Capture Device ID
	if (SUCCEEDED(*hr)) *hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID); 	// Request audio capture devices
	if (SUCCEEDED(*hr)) *hr = pConfig->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID, _audioCaptureDevice.GetId(hr).c_str()); 	// Request audio capture devices by ID
	if (SUCCEEDED(*hr)) *hr = MFCreateDeviceSource(pConfig, &audioCaptureDevice);
	//_audioCaptureDevice.Activate(__uuidof(IMFMediaSource), hr); //AudioCaptureDevice -> IMFMediaSource
	//Device(_audioCaptureDevice).~Device();

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;
}
void MediaSession::SetActiveDevice(AudioRenderDevice& _audioRenderDevice, HRESULT* hr) {
	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	//AudioRenderDevice -> IMFActivate of an IMFMediaSink (SAR)
	if (SUCCEEDED(*hr)) *hr = MFCreateAudioRendererActivate(&audioRenderDevice);
	if (SUCCEEDED(*hr)) *hr = audioRenderDevice->SetString(MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID, _audioRenderDevice.GetId(hr).c_str());

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;
}

void MediaSession::PlayAudioCaptureDatas(HRESULT* hr) {
	// Capture
	IMFPresentationDescriptor* _presDesc = NULL; // Point the presentation descriptor of the MediaSource of the Audio Capture Device
	IMFStreamDescriptor* _streamDesc = NULL; // Point the presentation descriptor's stream descriptor of the MediaSource of the Audio Capture Device
	BOOL _streamDescSelected = false; // The value is TRUE if the stream is currently selected, or FALSE if the stream is currently deselected.
	IMFTopology* pTopology = NULL;	// Topology
	IMFTopologyNode* _sourceNode = NULL; // Source Node

	// Render
	IMFTopologyNode* _outNode = NULL;

	// Media Session
	PROPVARIANT _propVar;
	PropVariantInit(&_propVar);
	//_propVar.vt = VT_EMPTY;

	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	// Get information from the Media Source of the Audio Capture Device
	if (SUCCEEDED(*hr)) *hr = audioCaptureDevice->CreatePresentationDescriptor(&_presDesc);  // Get/Create presentation descriptor of the MediaSource of the Audio Capture Device
	if (SUCCEEDED(*hr)) *hr = _presDesc->GetStreamDescriptorByIndex(0, &_streamDescSelected, &_streamDesc);  // Get/Create presentation descriptor of the MediaSource of the Audio Capture Device


	if (SUCCEEDED(*hr)) *hr = MFCreateTopology(&pTopology); // Topology : Create

	// Topology : Create Source Node
	if (SUCCEEDED(*hr)) *hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &_sourceNode); // Create the Source Node
	if (SUCCEEDED(*hr)) *hr = _sourceNode->SetUnknown(MF_TOPONODE_SOURCE, audioCaptureDevice); // Set the MediaSource of the source node
	if (SUCCEEDED(*hr)) *hr = _sourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, _presDesc); // Set the presentation descriptor of the source node
	if (SUCCEEDED(*hr)) *hr = _sourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, _streamDesc); // Set the stream descriptor of the source node

	if (SUCCEEDED(*hr)) *hr = pTopology->AddNode(_sourceNode); // // Topology : Add Source Node

	 // Topology : Create Output Node
	if (SUCCEEDED(*hr)) *hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &_outNode); // Create the Output Node
	if (SUCCEEDED(*hr)) *hr = _outNode->SetObject(audioRenderDevice); // Set the object pointer (Audio Render Device)
	if (SUCCEEDED(*hr)) *hr = _outNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE); // Recomended
	if (SUCCEEDED(*hr)) *hr = pTopology->AddNode(_outNode);


	if (SUCCEEDED(*hr)) *hr = _sourceNode->ConnectOutput(0, _outNode, 0); //Node Connection : Source -> Output

	//IMFTopoLoader* ppObj;
	//if (SUCCEEDED(*hr)) *hr = MFCreateTopoLoader(&ppObj);
	//ppObj->Load(pTopology);

	if (SUCCEEDED(*hr)) *hr = mediaSession->SetTopology(MFSESSION_SETTOPOLOGY_IMMEDIATE, pTopology); // Set the topology of the Media Session

	if (SUCCEEDED(*hr)) *hr = mediaSession->Start(&GUID_NULL, &_propVar);

	//Release Var
	PropVariantClear(&_propVar);

	SafeRelease(&_presDesc);
	SafeRelease(&_streamDesc);

	SafeRelease(&pTopology);

	SafeRelease(&_sourceNode);

	SafeRelease(&_outNode);

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;
}
#pragma endregion //AudioAPI

#pragma region DeviceManager

DevicesManager::DevicesManager() :
	hr(S_OK),
	nbAudioCaptureDevices(0),
	nbAudioRenderDevices(0),
	nbVideoCaptureDevices(0),
	mediaSession() {
	if (SUCCEEDED(hr)) hr = MFStartup(MF_VERSION);	// Initialize Media Foundation
	if (SUCCEEDED(hr)) mediaSession.Initialize(&hr); 	// Initialize Media Session

	if (!SUCCEEDED(hr)) throw hr;
}


DevicesManager::~DevicesManager() {
	ClearDevices();
#if _WIN32
	MFShutdown();	// Shut down Media Foundation.
#endif //_WIN32
}
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
			Device(audioRenderDevices[0]).~Device();
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

template <class T>
void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}
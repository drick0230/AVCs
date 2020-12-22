#include "DevicesManager.h"

#pragma region Device
/////////////////////////// Constructors /////////////////////////////////////
#pragma region Constructors
CaptureDevice::CaptureDevice(const unsigned int _devicesType, IMFActivate* _activate) {
	deviceType = _devicesType;
	activate = _activate;
}

CaptureDevice::~CaptureDevice() {
	activate->Release();
}

AudioCaptureDevice::AudioCaptureDevice(IMFActivate* _activate) : CaptureDevice(DevicesTypes::AUD_CAPT, _activate) {}
VideoCaptureDevice::VideoCaptureDevice(IMFActivate* _activate) : CaptureDevice(DevicesTypes::VID_CAPT, _activate) {}


#pragma endregion

/////////////////////////// Functions /////////////////////////////////////
#pragma region Functions
std::wstring CaptureDevice::GetName(HRESULT* hr) {
	wchar_t* _deviceName = NULL;
	unsigned int _length; // Length of _deviceName without '\0'
	std::wstring _returnDeviceName;

	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT();
		_localHr = true;
	}


	*hr = activate->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &_deviceName, &_length);

	if (_deviceName != NULL)
		_returnDeviceName.assign(_deviceName);
	else
		throw _deviceName;

	return _returnDeviceName; // Cant directly return _deviceName or the user will need to manually release it to prevent memory leak
}
#pragma endregion
#pragma endregion


/////////////////////////// Constructors /////////////////////////////////////
WASAPI::WASAPI() {

}

WASAPI::~WASAPI() {

}

DevicesManager::DevicesManager() :
#if _WIN32
	audioRenderDevices(NULL),
	pConfig(NULL),
	hr(NULL),
	audioCaptureDatas(NULL),
	audioRenderListener(NULL),
	audioRenderDatas(NULL),
	_mediaSession(NULL),
	//pSink(NULL),
#endif //_WIN32
	nbAudioCaptureDevices(0),
	nbAudioRenderDevices(0),
	nbVideoCaptureDevices(0) {
	// Initialize Devices Manager
#if _WIN32
	hr = MFStartup(MF_VERSION);	// Initialize Media Foundation
	if (SUCCEEDED(hr)) hr = MFCreateMediaSession(NULL, &_mediaSession); // Initialize the Media Session
	hr = MFCreateAttributes(&pConfig, 1); // Create an attribute store to hold the search criteria.
#endif //_WIN32
}


DevicesManager::~DevicesManager() {
	ClearDevices();
#if _WIN32
	SafeRelease(&pConfig);
	_mediaSession->Close(); // Close Media Session
	audioCaptureDatas->Shutdown(); // Shut down IMFMediaSource
	_mediaSession->Shutdown(); // Shut down Media Session
	MFShutdown();	// Shut down Media Foundation.
#endif //_WIN32
}
/////////////////////////// Functions /////////////////////////////////////
#pragma region EnumerateDevices
void DevicesManager::EnumerateDevices(const unsigned int _devicesType) {
#if _WIN32
	// Audio capture devices
	if (SUCCEEDED(hr) && (_devicesType == DevicesTypes::AUD_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL))
		EnumerateCaptureDevices(DevicesTypes::AUD_CAPT);
		//EnumerateAudioCaptureDevices();

	// Audio render devices
	if (SUCCEEDED(hr) && (_devicesType == DevicesTypes::AUD_REND || _devicesType == DevicesTypes::BOTH_REND || _devicesType == DevicesTypes::ALL));
	//EnumerateAudioRenderDevices();

	// Video capture devices
	if (SUCCEEDED(hr) && (_devicesType == DevicesTypes::VID_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL))
		EnumerateCaptureDevices(DevicesTypes::VID_CAPT);
		//EnumerateVideoCaptureDevices();
#endif //_WIN32
}

void DevicesManager::EnumerateCaptureDevices(const unsigned int _devicesType) {
	IMFActivate** _activates = NULL; // Array of pointer of IMFActivate of Audio Capture Devices
	unsigned int* _nbDevices = NULL;
	GUID _deviceTypeGUID;

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

	if(SUCCEEDED(hr)) hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, _deviceTypeGUID); 	// Request audio capture devices.
	if (SUCCEEDED(hr)) hr = MFEnumDeviceSources(pConfig, &_activates, _nbDevices); 	// Enumerate the audio capture devices,

	if (_devicesType == DevicesTypes::AUD_CAPT) {
		audioCaptureDevices.clear();
		for (unsigned int _i = 0; _i < *_nbDevices; _i++)
			audioCaptureDevices.emplace_back(_activates[_i]);
	}
	else if (_devicesType == DevicesTypes::VID_CAPT) {
		videoCaptureDevices.clear();
		for (unsigned int _i = 0; _i < *_nbDevices; _i++)
			videoCaptureDevices.emplace_back(_activates[_i]);
	}

	// Release Variables
	//CoTaskMemFree(_activates);
}
/*
void DevicesManager::EnumerateAudioCaptureDevices() {
#if _WIN32
	IMFActivate** audioCaptureDevices; // Array of pointer of IMFActivate of Audio Capture Devices

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
	//SafeRelease(&pEnum);
	audioRenderDevices->GetCount(&nbAudioRenderDevices); // Get the number of audio render devices
#endif //_WIN32
}

void DevicesManager::EnumerateVideoCaptureDevices() {
#if _WIN32
	hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID); 	// Request video capture devices.
	if (SUCCEEDED(hr));
		//hr = MFEnumDeviceSources(pConfig, &videoCaptureDevices, &nbVideoCaptureDevices); 	// Enumerate the video capture devices,
#endif //_WIN32

}*/
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
	//hr = MFCreateDeviceSource(audioCaptureDevices[_deviceID], &audioCaptureDatas);
	//if (SUCCEEDED(hr)) {
	//	const unsigned long REFTIMES_PER_SEC = 10000000;
	//	const unsigned long REFTIMES_PER_MILLISEC = 10000;
	//	IMMDevice* _device = NULL;              // An audio device.
	//	LPWSTR _audioRenderDeviceID = NULL;
	//	IAudioClient* pAudioClient = NULL;
	//	IAudioCaptureClient* _captureAudioClient = NULL;
	//	IAudioRenderClient* pRenderClient = NULL;
	//	WAVEFORMATEX* pwfx = NULL;
	//	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	//	UINT32 bufferFrameCount;
	//	BYTE* pData;

	//	hr = audioRenderDevices->Item(0, &_device); // Get the first device in the list.
	//	
	//	/*// Create a sink with the Audio Capture Device
	//	if(SUCCEEDED(hr)) hr = _device->GetId(&_audioRenderDeviceID); // Get ID of the first device in the list.
	//	if (SUCCEEDED(hr)) hr = pConfig->SetString(MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID, _audioRenderDeviceID);
	//	if (SUCCEEDED(hr)) hr = MFCreateAudioRenderer(pConfig, &audioRenderListener);*/
	//	// Get the audio capture devices datas
	//	IMFMediaType* _mediaType = NULL;
	//	//if (SUCCEEDED(hr)) hr = MFCreateSourceReaderFromMediaSource(audioCaptureDatas, NULL, &audioRenderDatas);
	//	//audioRenderDatas->GetCurrentMediaType(0, &_mediaType);
	//	audioCaptureDevices[0]->ActivateObject(IID_IAudioCaptureClient, (void**)&_captureAudioClient);
	//	if (SUCCEEDED(hr)) hr = pAudioClient->GetMixFormat(&pwfx); // get the MixFormat
	//	if (SUCCEEDED(hr)) hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, pwfx, NULL); // Request an endpoint buffer of a particular size
	//	if (SUCCEEDED(hr)) hr = pAudioClient->GetBufferSize(&bufferFrameCount); // Get the size of the allocated buffer.

	//	_captureAudioClient->GetBuffer();
	//	;
	//	// record audio from Mic
	//	//audioCaptureDatas->Start(,, VT_EMPTY);
	//	// Read the sink with the speaker
	//	if (SUCCEEDED(hr)) hr = _device->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
	//	if (SUCCEEDED(hr)) hr = pAudioClient->GetMixFormat(&pwfx);
	//	if (SUCCEEDED(hr)) hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, pwfx, NULL);
	//	//if (SUCCEEDED(hr)) hr = hr = audioRenderListener->SetFormat(pwfx);
	//	if (SUCCEEDED(hr)) hr = pAudioClient->GetBufferSize(&bufferFrameCount); // Get the actual size of the allocated buffer.
	//	if (SUCCEEDED(hr)) hr = pAudioClient->GetService(IID_IAudioRenderClient, (void**)&pRenderClient);
	//	if (SUCCEEDED(hr)) hr = pRenderClient->GetBuffer(bufferFrameCount, &pData); // Grab the entire buffer for the initial fill operation.
	//	*pData = audioRenderDatas.sampl
	//	//audioCaptureDevices->SetFormat(pwfx);
	//	//SafeRelease(&_device);
	//	CoTaskMemFree(_audioRenderDeviceID);
	//}
	//else
	//	throw hr;

	//if (!SUCCEEDED(hr))
	//{
	//	std::cerr << hr;
	//	throw hr;
	//}

	//ReadMediaFile(audioRenderListener);
	//audioRenderListener->Release();
#endif //_WIN32
}

#pragma endregion

/*
void DevicesManager::PlayAudioCaptureDatasMediaFoundation() {
	DWORD streamcount1 = 0, streamcount2 = 0;
	// Capture
	IMFPresentationDescriptor* _presDesc = NULL; // Point the presentation descriptor of the MediaSource of the Audio Capture Device
	IMFStreamDescriptor* _streamDesc = NULL; // Point the presentation descriptor's stream descriptor of the MediaSource of the Audio Capture Device
	BOOL _streamDescSelected = false; // The value is TRUE if the stream is currently selected, or FALSE if the stream is currently deselected.
	IMFTopology* pTopology = NULL;	// Topology
	IMFTopologyNode* _sourceNode = NULL; // Source Node
	IMFMediaTypeHandler* _streamDescMediaTypeHandler;
	GUID guidMajorType;

	// Render
	IMMDevice* _deviceRender = NULL; // An audio render device
	LPWSTR _audioRenderDeviceID = NULL;  // ID of the audio render device
	IMFTopologyNode* _outNode = NULL;
	//IMFStreamSink* _audioRenderStreamSink;
	IMFMediaTypeHandler* _audioRenderStreamSinkMediaTypeHandler;

	IMFStreamDescriptor* pSD = NULL;
	IMFActivate* pSinkActivate = NULL;


	// Media Session
	PROPVARIANT _propVar;
	PropVariantInit(&_propVar);
	_propVar.vt = VT_EMPTY;



	// Media Source
	hr = MFCreateDeviceSource(audioCaptureDevices[0], &audioCaptureDatas); // Create the MediaSource from the Audio Capture Device
	if (SUCCEEDED(hr)) hr = audioCaptureDatas->CreatePresentationDescriptor(&_presDesc);  // Get/Create presentation descriptor of the MediaSource of the Audio Capture Device
	if (SUCCEEDED(hr)) hr = _presDesc->GetStreamDescriptorByIndex(0, &_streamDescSelected, &_streamDesc);  // Get/Create presentation descriptor of the MediaSource of the Audio Capture Device
	if (SUCCEEDED(hr)) hr = _streamDesc->GetMediaTypeHandler(&_streamDescMediaTypeHandler); // Get the media type handler for the stream.
	if (SUCCEEDED(hr)) hr = _streamDescMediaTypeHandler->GetMajorType(&guidMajorType); // Get the major media type.

	// Streaming Audio Renderer
	if (SUCCEEDED(hr)) hr = audioRenderDevices->Item(0, &_deviceRender); // Get the first Audio Render Device
	if (SUCCEEDED(hr)) hr = _deviceRender->GetId(&_audioRenderDeviceID); // Get ID of the Audio Render Device
	// Streaming Audio Renderer/Sink : Create
	if (SUCCEEDED(hr)) hr = MFCreateAudioRendererActivate(&pSinkActivate);
	//if (SUCCEEDED(hr)) hr = MFCreateAttributes(&pConfig, 2); // Initialize the attributes storage
	if (SUCCEEDED(hr)) hr = pSinkActivate->SetString(MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID, _audioRenderDeviceID); // Assign a Audio Render Device to the SAR
	//if (SUCCEEDED(hr)) hr = MFCreateAttributes(&pConfig, 2); // Initialize the attributes storage
	//if (SUCCEEDED(hr)) hr = pConfig->SetString(MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID, _audioRenderDeviceID);
	//if (SUCCEEDED(hr)) hr = MFCreateAudioRenderer(pConfig, &audioRenderListener);


	// Topology : Create
	if (SUCCEEDED(hr)) hr = MFCreateTopology(&pTopology);
	// Topology : Create Source Node
	if (SUCCEEDED(hr)) hr = hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &_sourceNode);
	if (SUCCEEDED(hr)) hr = _sourceNode->SetUnknown(MF_TOPONODE_SOURCE, audioCaptureDatas); // Set the MediaSource of the source node
	if (SUCCEEDED(hr)) hr = _sourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, _presDesc); // Set the presentation descriptor of the source node
	if (SUCCEEDED(hr)) hr = _sourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, _streamDesc); // Set the stream descriptor of the source node
	// Topology : Add
	if (SUCCEEDED(hr)) hr = pTopology->AddNode(_sourceNode); // Add the node to the topology.

	// Topology : Create Output Node
	if (SUCCEEDED(hr)) hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &_outNode);
	// Topology : Set the object pointer
	if (SUCCEEDED(hr)) hr = _outNode->SetObject(pSinkActivate);
	//if (SUCCEEDED(hr)) hr = _outNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
	if (SUCCEEDED(hr)) hr = pTopology->AddNode(_outNode);
	//audioRenderListener->GetStreamSinkCount(&streamcount2);
	//if (SUCCEEDED(hr)) hr = audioRenderListener->GetStreamSinkByIndex(0, &_audioRenderStreamSink); // Get the stream sink of the SAR
	//if (SUCCEEDED(hr)) hr = _audioRenderStreamSink->GetMediaTypeHandler(&_audioRenderStreamSinkMediaTypeHandler);
	////if (SUCCEEDED(hr)) hr = _audioRenderStreamSinkMediaTypeHandler->SetCurrentMediaType(_streamDescMediaType);
	//if (SUCCEEDED(hr)) hr = _outNode->SetObject(_audioRenderStreamSink); // Set the object pointer
	//if (SUCCEEDED(hr)) hr = pTopology->AddNode(_outNode); // Add the node to the topology.
	//if (SUCCEEDED(hr)) hr = _outNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, TRUE); // Recommended

	//Node Connection : Source -> Output
	if (SUCCEEDED(hr)) hr = _sourceNode->ConnectOutput(0, _outNode, 0);

	//Media Session
	if (SUCCEEDED(hr)) hr = _mediaSession->SetTopology(MFSESSION_SETTOPOLOGY_IMMEDIATE, pTopology); // Set the topology
	//if (SUCCEEDED(hr)) hr = _mediaSession->Start(NULL, &_propVar);

	if (!SUCCEEDED(hr))
		throw hr;

	PropVariantClear(&_propVar);

	SafeRelease(&_presDesc);
	SafeRelease(&pTopology);
	SafeRelease(&_streamDesc);
	SafeRelease(&_sourceNode);

	SafeRelease(&_deviceRender);
	CoTaskMemFree(_audioRenderDeviceID);
	SafeRelease(&_outNode);
}

void DevicesManager::PlayAudioCaptureDatasWASAPI() {

}*/

/*
void DevicesManager::SaveAudioCaptureDatas() {
#if _WIN32
	// Listen
	const unsigned long REFTIMES_PER_SEC = 10000000;
	const unsigned long REFTIMES_PER_MILLISEC = 10000;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	IMMDevice* _device = NULL;              // An audio device.
	LPWSTR _audioRenderDeviceID = NULL;
	IAudioClient* pAudioClient = NULL;
	IAudioRenderClient* pRenderClient = NULL;
	BYTE* pRenderClientBuffer = NULL;
	WAVEFORMATEX* pwfx = NULL;

	hr = audioRenderDevices->Item(0, &_device); // Get the first device in the list.
	if (SUCCEEDED(hr)) hr = _device->GetId(&_audioRenderDeviceID); // Get ID of the first device in the list.
	if (SUCCEEDED(hr)) hr = _device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
	if (SUCCEEDED(hr)) hr = pAudioClient->GetMixFormat(&pwfx);
	if (SUCCEEDED(hr)) hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, pwfx, NULL);
	if (SUCCEEDED(hr)) hr = pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&pRenderClient);
	if (SUCCEEDED(hr)) hr = pRenderClient->GetBuffer(1, &pRenderClientBuffer);
	if (hr == E_POINTER)
		hr = hr;
	if (hr == E_NOINTERFACE)
		hr = hr;
	if (hr == AUDCLNT_E_NOT_INITIALIZED)
		hr = hr;
	if (hr == AUDCLNT_E_WRONG_ENDPOINT_TYPE)
		hr = hr;
	if (hr == AUDCLNT_E_DEVICE_INVALIDATED)
		hr = hr;
	if (hr == AUDCLNT_E_SERVICE_NOT_RUNNING)
		hr = hr;
	// Capture
	IMFMediaType* _mediaType = NULL;
	DWORD streamIndex; // Receives the actual stream index.
	DWORD flags; // Receives status flags.
	LONGLONG llTimeStamp; // Receives the time stamp.
	IMFSample* pSample = NULL; // Receives the sample or NULL.
	IMFMediaBuffer* _buffer = NULL;
	DWORD _bufferLength;
	DWORD _bufferMaxLength;
	BYTE* _bufferDatas;

	if (SUCCEEDED(hr)) hr = MFCreateDeviceSource(audioCaptureDevices[0], &audioCaptureDatas); // Create the MediaSource from the Audio Capture Device
	if (SUCCEEDED(hr)) hr = MFCreateSourceReaderFromMediaSource(audioCaptureDatas, NULL, &audioRenderDatas); // Create the SourceReader from the MediaSource of the Audio Capture Device

	// Record Datas
	IMFPresentationDescriptor* _presDesc;
	PROPVARIANT var;
	PropVariantInit(&var);
	var.vt = VT_EMPTY;
	if (SUCCEEDED(hr)) hr = audioCaptureDatas->CreatePresentationDescriptor(&_presDesc);
	if (SUCCEEDED(hr)) hr = audioCaptureDatas->Start(_presDesc, NULL, &var); // 0 = VT_EMPTY
	//if (SUCCEEDED(hr)) hr = audioCaptureDatas->Stop();
	//if (SUCCEEDED(hr)) hr = audioRenderDatas->GetCurrentMediaType(0, &_mediaType);
	bool _quit = false;
	unsigned int _i = 0;
	const int MAX_SAMPLE = 50;
	while (_quit != true && _i < MAX_SAMPLE) {
		if (SUCCEEDED(hr)) hr = audioRenderDatas->ReadSample(MF_SOURCE_READER_ANY_STREAM, 0, &streamIndex, &flags, &llTimeStamp, &pSample);
		if (pSample != NULL)
			wprintf(L"Stream %d (%I64d) (%x)\n", streamIndex, llTimeStamp, *pSample);
		else
			wprintf(L"Stream %d (%I64d) (NULL)\n", streamIndex, llTimeStamp);

		if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
			_quit = true;
			wprintf(L"\tEnd of stream\n");
		}
		if (flags & MF_SOURCE_READERF_NEWSTREAM)
			wprintf(L"\tNew stream\n");
		if (flags & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED)
			wprintf(L"\tNative type changed\n");
		if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
			wprintf(L"\tCurrent type changed\n");
		if (flags & MF_SOURCE_READERF_STREAMTICK)
			wprintf(L"\tStream tick\n");
		else {
			// Listen
			if (SUCCEEDED(hr)) hr = pSample->ConvertToContiguousBuffer(&_buffer);
			_buffer->Lock(&_bufferDatas, &_bufferMaxLength, &_bufferLength);
			for (unsigned int i2 = 0; i2 < _bufferLength; i2++)
			{
				pRenderClientBuffer[i2] = _bufferDatas[i2];
			}
			_buffer->Unlock();
			pRenderClient->ReleaseBuffer(_bufferLength, 0);
			pAudioClient->Start();
			hr = hr;
		}
		_i++;
	}
	_i = _i;*/
	/*IMFSinkWriter* pSinkWriter;
	hr = MFCreateSourceReaderFromMediaSource(audioCaptureDatas, NULL, &audioRenderDatas);
	audioRenderDatas->GetCurrentMediaType(0, &_mediaType);
	hr = MFCreateSinkWriterFromURL(L"output.wmv", NULL, NULL, &pSinkWriter);
	audioRenderDatas->ReadSample(0,);
	pSinkWriter->(audioCaptureDatas);
	SafeRelease(&pSinkWriter);*//*
	//SafeRelease(&_mediaType);
#endif //_WIN32
}*/

void DevicesManager::ClearDevices(const unsigned int _devicesType) {
#if _WIN32
	// Release Audio Capture Devices
	if (_devicesType == DevicesTypes::AUD_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL)
		for (unsigned int _i = 0; _i < nbAudioCaptureDevices; _i++)
			audioCaptureDevices.clear();

	if (_devicesType == DevicesTypes::AUD_REND || _devicesType == DevicesTypes::BOTH_REND || _devicesType == DevicesTypes::ALL) {
		//SafeRelease(&audioRenderDevices);
		//SafeRelease(&audioOutDatas);
		//SafeRelease(&audioRenderListener);
	}

	// Release Video Capture Devices
	if (_devicesType == DevicesTypes::AUD_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL)
		for (unsigned int _i = 0; _i < nbAudioCaptureDevices; _i++)
			videoCaptureDevices.clear();

	//if (_devicesType == DevicesTypes::VID_CAPT || _devicesType == DevicesTypes::BOTH_CAPT || _devicesType == DevicesTypes::ALL) {
	//	// Release Video Devices
	//	for (unsigned int _i = 0; _i < nbVideoCaptureDevices; _i++)
	//	{
	//		videoCaptureDevices[_i]->Release();
	//	}
	//	CoTaskMemFree(videoCaptureDevices);
	//}
#endif //_WIN32
}

#pragma region GetDevicesName
std::wstring DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID) {
	if (_devicesType == DevicesTypes::AUD_CAPT)
	{
		//return GetIMFActivateName(_devicesType, _deviceID, audioCaptureDevices); 
		return audioCaptureDevices[_deviceID].GetName(&hr); // Return the name of the audio capture device
	}
	else if (_devicesType == DevicesTypes::AUD_REND) {
		return GetAudioRenderDevicesName(_deviceID); // Return the name of the video device
	}
	else if (_devicesType == DevicesTypes::VID_CAPT) {
		//return GetIMFActivateName(_devicesType, _deviceID, videoCaptureDevices); // Return the name of the video device
		return videoCaptureDevices[_deviceID].GetName(&hr); // Return the name of the video device
	}
	else {
		std::cerr << "_devicesType cant be BOTH or ALL in std::string DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
		throw "_devicesType cant be BOTH or ALL in std::string DevicesManager::GetDevicesName(const unsigned int _devicesType, const unsigned int _deviceID)";
	}

	return L"Not Found"; // The device ID or the device type is invalid
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
		return L"Not Found";
	}

	wchar_t* _deviceName = NULL;
	unsigned int _length; // Length of _deviceName without '\0'
	hr = _devices[0]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &_deviceName, &_length);

	if (_deviceName != NULL)
		_returnDeviceName.assign(_deviceName);
	else
		throw _deviceName;
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
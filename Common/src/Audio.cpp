#include "Audio.h"

#pragma region SourceReader_SinkWritter

SourceReader_SinkWritter::SourceReader_SinkWritter(HRESULT* hr) :
	audioCaptureSource(NULL), audioCaptureDatas(NULL), audioRenderSink(NULL), audioRenderDatas(NULL) {}

SourceReader_SinkWritter::~SourceReader_SinkWritter() {
	SafeRelease(&audioCaptureSource);
	SafeRelease(&audioCaptureDatas);

	SafeRelease(&audioRenderSink);
	SafeRelease(&audioRenderDatas);
}

void SourceReader_SinkWritter::SetActiveDevice(AudioCaptureDevice& _audioCaptureDevice, HRESULT* hr) {
	IMFAttributes* pConfig; // Store the search criteria for the devices

	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	// Create the MediaSource from the IMMDevice
	if (SUCCEEDED(*hr)) *hr = MFCreateAttributes(&pConfig, 2); // Create an attribute store to hold the Audio Capture Device ID
	if (SUCCEEDED(*hr)) *hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID); 	// Request audio capture devices
	if (SUCCEEDED(*hr)) *hr = pConfig->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID, _audioCaptureDevice.GetId(hr).c_str()); 	// Request audio capture devices by ID
	if (SUCCEEDED(*hr)) *hr = MFCreateDeviceSource(pConfig, &audioCaptureSource);

	// Create the SourceReader from the MediaSource
	if (SUCCEEDED(*hr)) *hr = MFCreateSourceReaderFromMediaSource(audioCaptureSource, NULL, &audioCaptureDatas);

	// Enumerate MediaType of the SourceReader
	DWORD dwMediaTypeIndex = 0;
	GUID _subTypeGUID;

	while (SUCCEEDED(*hr))
	{
		IMFMediaType* pType = NULL;
		*hr = audioCaptureDatas->GetNativeMediaType(0, dwMediaTypeIndex, &pType);
		if (SUCCEEDED(*hr))
		{
			// Examine the media type. (Not shown.)
			pType->GetGUID(MF_MT_SUBTYPE, &_subTypeGUID);
			if (_subTypeGUID == MFAudioFormat_PCM) {
				// Format = PCM
			}
			else if (_subTypeGUID == MFAudioFormat_Float) {
				// Format = IEEE Float
			}
			pType->Release();
		}
		else if (*hr == MF_E_NO_MORE_TYPES) {
			*hr = S_OK;
			break;
		}
		++dwMediaTypeIndex;
	}

	SafeRelease(&pConfig);

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;
}

void SourceReader_SinkWritter::SetActiveDevice(AudioRenderDevice& _audioRenderDevice, HRESULT* hr) {
	IMFAttributes* pConfig; // Store the search criteria for the devices

	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	// Create the MediaSink from the IMMDevice
	if (SUCCEEDED(*hr)) *hr = MFCreateAttributes(&pConfig, 1); // Create an attribute store to hold the Audio Capture Device ID
	if (SUCCEEDED(*hr)) *hr = pConfig->SetString(MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID, _audioRenderDevice.GetId(hr).c_str()); 	// Request audio capture devices
	if (SUCCEEDED(*hr)) *hr = MFCreateAudioRenderer(pConfig, &audioRenderSink);

	// Create the SinkWritter from the MediaSink
	if (SUCCEEDED(*hr)) *hr = MFCreateSinkWriterFromMediaSink(audioRenderSink, NULL, &audioRenderDatas);

	SafeRelease(&pConfig);

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;
}

std::vector<unsigned char> SourceReader_SinkWritter::GetAudioCaptureDeviceMediaTypeDatas(HRESULT* hr) {
	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	std::vector<unsigned char> _returnDatas;
	IMFMediaType* _mediaType = NULL;

	if (SUCCEEDED(*hr)) *hr = audioCaptureDatas->GetCurrentMediaType(0, &_mediaType); // Get the MediaType

	unsigned char* _datas;
	unsigned int _datasSize;

	if (SUCCEEDED(*hr)) *hr = MFGetAttributesAsBlobSize(_mediaType, &_datasSize);
	if (SUCCEEDED(*hr)) _datas = new unsigned char[_datasSize];
	if (SUCCEEDED(*hr)) *hr = MFGetAttributesAsBlob(_mediaType, _datas, _datasSize);

	if (SUCCEEDED(*hr)) {
		_returnDatas.reserve(_datasSize);
		for (unsigned int _i = 0; _i < _datasSize; _i++)
			_returnDatas.push_back(_datas[_i]);
	}

	if (SUCCEEDED(*hr)) delete[] _datas;
	SafeRelease(&_mediaType);

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;

	return _returnDatas;
}

void SourceReader_SinkWritter::SetInputMediaType(std::vector<unsigned char> _mediaTypeDatas, HRESULT* hr) {
	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	// Vector of Char --> Media Type
	IMFMediaType* _mediaType;

	if (SUCCEEDED(*hr)) *hr = MFCreateMediaType(&_mediaType);
	if (SUCCEEDED(*hr)) *hr = MFInitAttributesFromBlob(_mediaType, _mediaTypeDatas.data(), _mediaTypeDatas.size());

	// Set the Media Type
	if (SUCCEEDED(*hr)) *hr = audioRenderDatas->SetInputMediaType(0, _mediaType, NULL); // Need to send this information at the begining of a Room or when it change (IMPORTANT)
	if (SUCCEEDED(*hr)) *hr = audioRenderDatas->BeginWriting(); // The SinkWriter is ready to receive and write datas

	SafeRelease(&_mediaType);

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;
}

void SourceReader_SinkWritter::PlayAudioDatas(AudioDatas _audioDatas, HRESULT* hr) {
	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	if (_audioDatas.datas.size() > 0) {
		// Create and fill the Buffer
		unsigned char* _bufferPointer;
		IMFMediaBuffer* _buffer;
		IMFSample* _sample;
		if (SUCCEEDED(*hr)) *hr = MFCreateMemoryBuffer(_audioDatas.datas.size(), &_buffer);

		if (SUCCEEDED(*hr)) *hr = _buffer->Lock(&_bufferPointer, NULL, NULL);
		if (SUCCEEDED(*hr)) for (unsigned long _i = 0; _i < _audioDatas.datas.size(); _i++)
			_bufferPointer[_i] = _audioDatas.datas[_i];
		if (SUCCEEDED(*hr)) *hr = _buffer->SetCurrentLength(_audioDatas.datas.size());
		if (SUCCEEDED(*hr)) *hr = _buffer->Unlock();

		// Create the sample that will be read on the Audio Render Device
		if (SUCCEEDED(*hr)) *hr = MFCreateSample(&_sample);
		if (SUCCEEDED(*hr))*hr = _sample->AddBuffer(_buffer);

		if (SUCCEEDED(*hr))*hr = _sample->SetSampleDuration(_audioDatas.duration);
		if (SUCCEEDED(*hr))*hr = _sample->SetSampleTime(_audioDatas.time);

		// Read the sample
		if (SUCCEEDED(*hr))*hr = audioRenderDatas->WriteSample(0, _sample);

		//audioRenderDatas->Finalize();

		SafeRelease(&_buffer);
		SafeRelease(&_sample);
	}

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;
}

AudioDatas SourceReader_SinkWritter::ReadAudioDatas(HRESULT* hr) {
	AudioDatas _returnAudioDatas;
	//std::vector<unsigned char> _returnAudioDatas;

	IMFSample* pSample = NULL;
	size_t  cSamples = 0;


	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	//if (SUCCEEDED(*hr)) *hr = audioCaptureDatas->GetCurrentMediaType(0, &_returnMediaType); // Get the MediaType and return it

	DWORD streamIndex = 0, streamStatus = 0;
	LONGLONG llTimeStamp = 0;

	if (SUCCEEDED(*hr)) *hr = audioCaptureDatas->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &streamStatus, &llTimeStamp, &pSample);
	if (SUCCEEDED(*hr))
	{
		//wprintf(L"Stream %d (%I64d)\n", streamIndex, llTimeStamp);
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
			//audioRenderDatas->SendStreamTick();
		}

		if (streamStatus & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED)
		{
			// The format changed. Reconfigure the decoder.
			throw "Format Changed";
		}

		if (pSample) {
			++cSamples; // Count how many sample are process (Not NULL)

			unsigned long _captureBufferLength; // How many datas in the buffer
			if (SUCCEEDED(*hr)) *hr = pSample->GetTotalLength(&_captureBufferLength);

			_returnAudioDatas.datas.resize(_captureBufferLength, 0); // Resize to the right amount of datas

			IMFMediaBuffer* _captureBuffer; // Copy of the contiguous buffer from the Capture Device Sample
			if (SUCCEEDED(*hr)) *hr = MFCreateMemoryBuffer(_captureBufferLength, &_captureBuffer);
			if (SUCCEEDED(*hr)) *hr = pSample->CopyToBuffer(_captureBuffer);

			//	Transfer Audio Datas from the buffer to the AudioDatas' datas
			unsigned char* _bufferPointer;

			if (SUCCEEDED(*hr)) *hr = _captureBuffer->Lock(&_bufferPointer, NULL, NULL);
			if (SUCCEEDED(*hr)) for (unsigned long _i = 0; _i < _captureBufferLength; _i++)
				_returnAudioDatas.datas[_i] = _bufferPointer[_i];
			if (SUCCEEDED(*hr)) *hr = _captureBuffer->Unlock();

			if (SUCCEEDED(*hr)) *hr = pSample->GetSampleTime(&_returnAudioDatas.time); // Return the time
			if (SUCCEEDED(*hr)) *hr = pSample->GetSampleDuration(&_returnAudioDatas.duration); // Return the duration

			SafeRelease(&_captureBuffer);
		}
	}
	SafeRelease(&pSample);

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;

	return _returnAudioDatas;
}

#pragma endregion //SourceReader_SinkWritter

#pragma region MediaSession
MediaSession::MediaSession(HRESULT* hr) :
	mediaSession(NULL), audioCaptureSource(NULL), audioRenderSource(NULL) {}

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
	if(audioCaptureSource != NULL)
		audioCaptureSource->Shutdown(); // Shut down IMFMediaSource
	mediaSession->Shutdown(); // Shut down Media Session

	SafeRelease(&audioCaptureSource);

	SafeRelease(&audioRenderSource);
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

	// Create the MediaSource from the IMMDevice
	if (SUCCEEDED(*hr)) *hr = MFCreateAttributes(&pConfig, 2); // Create an attribute store to hold the Audio Capture Device ID
	if (SUCCEEDED(*hr)) *hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID); 	// Request audio capture devices
	if (SUCCEEDED(*hr)) *hr = pConfig->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID, _audioCaptureDevice.GetId(hr).c_str()); 	// Request audio capture devices by ID
	if (SUCCEEDED(*hr)) *hr = MFCreateDeviceSource(pConfig, &audioCaptureSource);

	SafeRelease(&pConfig);

	if (!SUCCEEDED(*hr)) throw* hr;
	if (_localHr) delete hr;
}
void MediaSession::SetActiveDevice(AudioRenderDevice& _audioRenderDevice, HRESULT* hr) {
	//IMFAttributes* pConfig; // Store the search criteria for the devices

	bool _localHr = false;
	if (hr == NULL) {
		hr = new HRESULT(S_OK);
		_localHr = true;
	}

	//AudioRenderDevice -> IMFActivate of an IMFMediaSink (SAR)
	if (SUCCEEDED(*hr)) *hr = MFCreateAudioRendererActivate(&audioRenderSource);
	if (SUCCEEDED(*hr)) *hr = audioRenderSource->SetString(MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID, _audioRenderDevice.GetId(hr).c_str());

	//SafeRelease(&pConfig);

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
	if (SUCCEEDED(*hr)) *hr = audioCaptureSource->CreatePresentationDescriptor(&_presDesc);  // Get/Create presentation descriptor of the MediaSource of the Audio Capture Device
	if (SUCCEEDED(*hr)) *hr = _presDesc->GetStreamDescriptorByIndex(0, &_streamDescSelected, &_streamDesc);  // Get/Create presentation descriptor of the MediaSource of the Audio Capture Device


	if (SUCCEEDED(*hr)) *hr = MFCreateTopology(&pTopology); // Topology : Create

	// Topology : Create Source Node
	if (SUCCEEDED(*hr)) *hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &_sourceNode); // Create the Source Node
	if (SUCCEEDED(*hr)) *hr = _sourceNode->SetUnknown(MF_TOPONODE_SOURCE, audioCaptureSource); // Set the MediaSource of the source node
	if (SUCCEEDED(*hr)) *hr = _sourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, _presDesc); // Set the presentation descriptor of the source node
	if (SUCCEEDED(*hr)) *hr = _sourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, _streamDesc); // Set the stream descriptor of the source node

	if (SUCCEEDED(*hr)) *hr = pTopology->AddNode(_sourceNode); // // Topology : Add Source Node

	 // Topology : Create Output Node
	if (SUCCEEDED(*hr)) *hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &_outNode); // Create the Output Node
	if (SUCCEEDED(*hr)) *hr = _outNode->SetObject(audioRenderSource); // Set the object pointer (Audio Render Device)
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
#pragma endregion //MediaSession
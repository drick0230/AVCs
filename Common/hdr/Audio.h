#pragma once
//#include <locale>

//#include "Console.h"

#include "DevicesManager.h"

#define MF_E_NO_MORE_TYPES 0xc00d36b9

// Contain info about an Audio Sample
class AudioDatas {
public:
	std::vector<unsigned char> datas;
	long long duration;
	long long time;

	AudioDatas() : datas(), duration(0), time(0) {};
	AudioDatas(std::vector<unsigned char> _datas, long long _duration, long long _time) : datas(_datas), duration(_duration), time(_time) {};
	AudioDatas(const AudioDatas& _b) : AudioDatas(_b.datas, _b.duration, _b.time) {};
};

class SourceReader_SinkWritter {
	IMFMediaSource* audioCaptureSource;
	IMFSourceReader* audioCaptureDatas;

	IMFMediaSink* audioRenderSink;
	IMFSinkWriter* audioRenderDatas;
public:
	//std::vector<std::vector<unsigned char>> audioDatas;
	//std::vector<long long> audioDatasTime;

	SourceReader_SinkWritter(HRESULT* hr = NULL);
	~SourceReader_SinkWritter();

	void SetActiveDevice(AudioCaptureDevice& _audioCaptureDevice, HRESULT* hr = NULL);
	void SetActiveDevice(AudioRenderDevice& _audioRenderDevice, HRESULT* hr = NULL);

	std::vector<unsigned char> GetAudioCaptureDeviceMediaTypeDatas(HRESULT* hr = NULL);
	void SetInputMediaType(std::vector<unsigned char> _mediaTypeDatas, HRESULT* hr = NULL);

	void PlayAudioDatas(AudioDatas _audioDatas, HRESULT* hr = NULL);
	AudioDatas ReadAudioDatas(HRESULT* hr = NULL);
};

class MediaSession {
private:
	IMFMediaSession* mediaSession; // Control the audio (Play/Pause/Stop)
	IMFMediaSource* audioCaptureSource;

	IMFActivate* audioRenderSource;
public:
	MediaSession(AudioCaptureDevice& _audioCaptureDevice, AudioRenderDevice& _audioRenderDevice, HRESULT* hr = NULL);
	MediaSession(HRESULT* hr = NULL);
	~MediaSession();
	void Initialize(HRESULT* hr = NULL);

	void SetActiveDevice(AudioCaptureDevice& _audioCaptureDevice, HRESULT* hr = NULL);
	void SetActiveDevice(AudioRenderDevice& _audioRenderDevice, HRESULT* hr = NULL);

	void PlayAudioCaptureDatas(HRESULT* hr = NULL);
};
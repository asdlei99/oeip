#include "FAudioOutput.h"
#include <OeipExport.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <SDL.h>
#include <SDL_audio.h>
#ifdef __cplusplus
}
#endif

using namespace std::placeholders;

FAudioOutput::FAudioOutput() {
}


FAudioOutput::~FAudioOutput() {
}

int32_t FAudioOutput::onStart() {
	if (bMixer) {
		mixer = std::make_unique< AudioMixer>();
		onAudioDataHandle mixHandle = std::bind(&FAudioOutput::onMixData, this, _1, _2);
		mixer->onDataHandle = mixHandle;
		mixer->init(destAudioDesc);
	}
	if (bMic) {
		//是否需要重采样
		bReMic = !(micAudioDesc == destAudioDesc);
		if (bReMic) {
			micResample = std::make_unique< AudioResample>();
			onAudioDataHandle remicHandle = std::bind(&FAudioOutput::onResampleData, this, true, _1, _2);
			micResample->onDataHandle = remicHandle;
			micResample->init(micAudioDesc, destAudioDesc);
		}
	}
	if (bLoopBack) {
		//是否需要重采样
		bReLoopBack = !(loopAudioDesc == destAudioDesc);
		if (bReLoopBack) {
			loopResample = std::make_unique< AudioResample>();
			onAudioDataHandle loopHandle = std::bind(&FAudioOutput::onResampleData, this, false, _1, _2);
			loopResample->onDataHandle = loopHandle;
			loopResample->init(loopAudioDesc, destAudioDesc);
		}
	}
	return 0;
}

int32_t FAudioOutput::onStop() {
	mixer->close();
	return 0;
}

int32_t FAudioOutput::onDataRecord(bool bmic, uint8_t* data, int dataLen, OeipAudioDataType type) {
	if (type != OEIP_Audio_Data && type != OEIP_AudioData_None)
		return 0;
	if (bmic && bReMic) {
		if (micResample)
			micResample->resampleData(data, dataLen);
	}
	else if (!bmic && bReLoopBack) {
		if (loopResample)
			loopResample->resampleData(data, dataLen);
	}
	else if (bMixer) {
		if (mixer)
			mixer->input(bmic ? 0 : 1, data, dataLen);
	}
	else if (onDataHandle != nullptr) {
		onDataHandle(data, dataLen);
	}
	return 0;
}

void FAudioOutput::onResampleData(bool bmic, uint8_t* data, int32_t lenght) {
	if (bMixer) {
		if (mixer)
			mixer->input(bmic ? 0 : 1, data, lenght);
	}
	else if (onDataHandle != nullptr) {
		onDataHandle(data, lenght);
	}
}

void FAudioOutput::onMixData(uint8_t* data, int32_t lenght) {
	if (onDataHandle != nullptr) {
		onDataHandle(data, lenght);
	}
}


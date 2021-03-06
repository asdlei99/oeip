#pragma once
#include "OeipLive.h"
#include "OeipLiveBack.h"

extern "C"
{
	//初始化直播环境
	OEIPLIVEDLL_EXPORT void initOeipLive();
	//销毁直播产生的资源
	OEIPLIVEDLL_EXPORT void shutdownOeipLive();
	//C++ 直接使用相应接口实现
	OEIPLIVEDLL_EXPORT bool initLiveRoom(const OeipLiveContext& liveCtx, OeipLiveBack* liveBack);
	//C# 使用接口转函数实现
	OEIPLIVEDLL_EXPORT bool initLiveRoomWrapper(const OeipLiveContext& liveCtx, const LiveBackWrapper& liveBack);
	//通知服务器,登陆房间
	OEIPLIVEDLL_EXPORT bool loginRoom(const char* roomName, int32_t userId);
	//通知服务器,开始推流
	OEIPLIVEDLL_EXPORT bool pushStream(int32_t index, const OeipPushSetting& setting);
	//推视频流数据
	OEIPLIVEDLL_EXPORT bool pushVideoFrame(int32_t index, const OeipVideoFrame& videoFrame);
	//推音频流数据
	OEIPLIVEDLL_EXPORT bool pushAudioFrame(int32_t index, const OeipAudioFrame& audioFrame);
	//通知服务器,关闭推流
	OEIPLIVEDLL_EXPORT bool stopPushStream(int32_t index);
	//拉流
	OEIPLIVEDLL_EXPORT bool pullStream(int32_t userId, int32_t index);
	//关闭拉流
	OEIPLIVEDLL_EXPORT bool stopPullStream(int32_t userId, int32_t index);
	//登出房间
	OEIPLIVEDLL_EXPORT bool logoutRoom();
}
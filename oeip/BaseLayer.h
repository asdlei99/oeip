#pragma once
#include "OeipCommon.h"
#include <string>
#include <memory>

//每层的width/height根据上层自动变动,而elementCount/elementByte需要每层初始化定义好
struct LayerConnect
{
	int32_t width = 0;
	int32_t height = 0;
	//数据类型,前三BIT表示类型，后三BIT表示通道个数
	int32_t dataType = OEIP_CV_8UC4;
};

class OEIPDLL_EXPORT CommonLayer
{
public:
	CommonLayer() {};
	virtual ~CommonLayer() {};
public:
	OeipLayerType layerType = OEIP_NONE_LAYER;
public:
	static const int layerTypeIndex = -1;
};

template<typename T>
class BaseLayerTemplate : public virtual CommonLayer
{
public:
	typedef T UpdateStruct;
	T layerParamet;
public:
	static const int32_t layerTypeIndex = IndexOf<T, AllLayerParamet>::value;
public:
	BaseLayerTemplate() {
		layerType = OeipLayerType(layerTypeIndex);
	};
	virtual ~BaseLayerTemplate() {};
public:
	//需要重置当前管线看onParametChange的决定
	void updateParamet(const T& t) {
		T oldParamet = layerParamet;
		layerParamet = t;
		onParametChange(oldParamet);
		////memcmp需要注意内存对齐,后续比较替代方案
		//if (memcmp(&layerParamet, &t, sizeof(T)) != 0) {
		//	T oldParamet = layerParamet;
		//	layerParamet = t;
		//	onParametChange(oldParamet);
		//}
	};
	void updateParamet(const void* paramet);
protected:
	//当更新当层Layer层的参数时发生,根据需要决定变更那些buffer,一般在bBufferInit=true后有意义
	virtual void onParametChange(T oldT) {};
};

class OEIPDLL_EXPORT BaseLayer :public virtual CommonLayer
{
public:
	BaseLayer() :BaseLayer(1, 1) {};
	BaseLayer(int32_t inSize, int32_t outSize);
	virtual ~BaseLayer() {};
public:
	std::string layerName = OEIP_EMPTYSTR;
	int32_t layerIndex = -1;
	//每层的输入层名
	std::vector<std::string> forwardNames;
	//对应每层的输出索引
	std::vector<int32_t> forwardOutIndexs;
	//对个每个输入的层索引(结合forwardOutIndexs),假设有三个转入层 可能是[4,0]/[4,1]/[5,0]
	std::vector<int32_t> forwardLayerIndexs;
	//当前层是否关闭
	bool bDisable = false;
	//当前层及链接在后面的层全部关闭运算，一般是有多个输出链路，根据相应条件关闭某些输出链路
	bool bDisableList = false;
	bool bDListChange = false;
protected:
	//当前层buffer有没初始化
	bool bBufferInit = false;
	//当前层只是在层绘制内容，并不需要申请输出,如果为true,请不要在当前层使用outGPU显存
	bool bOnlyDraw = false;
	class ImageProcess* imageProcess = nullptr;
protected:
	int32_t inCount = 1;
	int32_t outCount = 1;
	//每层selfConnect与outConnect的elementByte自己设定
	std::vector<LayerConnect> selfConnects;
	std::vector<LayerConnect> outConnects;
protected:
	//所有层公共的初始化
	virtual void onInitLayer() {};
	//和每层输入有关的初始化
	virtual void onInitLayer(int32_t index) {};
	//相应的GPU显存申请(连接上层的输出,申请当前层的输出)
	virtual bool onInitBuffer() { return true; }
	virtual void onRunLayer() {};
public:
	virtual void setImageProcess(class ImageProcess* process) {
		imageProcess = process;
	};
	virtual int32_t getInputCount() {
		return inCount;
	};
	//得到各个输入层的索引,检查输入层的大小,设置threadSize,outConnect,子类在onInitLayer更改
	virtual bool initLayer();
	bool initBuffer();
	void runLayer();
	void updateParamet(const void* paramet);
	bool onlyDraw() { return bOnlyDraw; };
public:
	int32_t getForwardIndex(int32_t inputIndex) {
		return forwardLayerIndexs[inputIndex];
	};
	void setInputSize(int32_t width, int32_t height, int32_t dataType, int32_t intputIndex = 0);
	void setOutputSize(int32_t width, int32_t height, int32_t dataType, int32_t outputIndex = 0);
	void getInputSize(LayerConnect& inConnect, int32_t inputIndex = 0);
	void getOutputSize(LayerConnect& outConnect, int32_t outputIndex = 0);
	void setForwardLayer(std::string forwardName, int32_t outputIndex = 0, int32_t inputIndex = 0);
};

class OEIPDLL_EXPORT BaseInputLayer //: public virtual CommonLayer
{
public:
	BaseInputLayer() {};
	virtual ~BaseInputLayer() {};
public:
	virtual void inputGpuTex(void* device, void* texture, int32_t inputIndex) = 0;
	virtual void inputCpuData(uint8_t* byteData, int32_t inputIndex) = 0;
};

class OEIPDLL_EXPORT BaseOutputLayer //: public virtual CommonLayer
{
public:
	BaseOutputLayer() {};
	virtual ~BaseOutputLayer() {};

public:
	virtual void outputGpuTex(void* device, void* texture, int32_t outputIndex) = 0;
};

class OEIPDLL_EXPORT InputLayer : public BaseLayerTemplate<InputParamet>, public BaseInputLayer
{
public:
	InputLayer() {};
	virtual ~InputLayer() {};
};

class OEIPDLL_EXPORT OutputLayer : public BaseLayerTemplate<OutputParamet>, public BaseOutputLayer
{
public:
	OutputLayer() {};
	virtual ~OutputLayer() {};
};

typedef BaseLayerTemplate<YUV2RGBAParamet> YUV2RGBALayer;
typedef BaseLayerTemplate<MapChannelParamet> MapChannelLayer;
typedef BaseLayerTemplate<ResizeParamet> ResizeLayer;
typedef BaseLayerTemplate<RGBA2YUVParamet> RGBA2YUVLayer;
typedef BaseLayerTemplate<OperateParamet> OperateLayer;
//需要二张输入图，第一张是主图(底图)，第二张是要混合图
typedef BaseLayerTemplate<BlendParamet> BlendLayer;
typedef BaseLayerTemplate<GuidedFilterParamet> GuidedFilterLayer;
typedef BaseLayerTemplate<GrabcutParamet> GrabcutLayer;
typedef BaseLayerTemplate<DarknetParamet> DarknetLayer;

template<typename T>
inline void BaseLayerTemplate<T>::updateParamet(const void* paramet) {
	T* tparamet = (T*)paramet;
	updateParamet(*tparamet);
}

#define OEIP_UPDATEPARAMET(layerType) \
	using ParametType = LayerParamet<layerType, AllLayerParamet>::ParametType;\
	auto xlayer = dynamic_cast<BaseLayerTemplate<ParametType>*>(this);\
	xlayer->updateParamet(paramet);

template<OeipLayerType layerType>
void updateParametTemplate(BaseLayer* layer, const void* paramet) {
	using ParametType = typename LayerParamet<layerType, AllLayerParamet>::ParametType;
	auto xlayer = dynamic_cast<BaseLayerTemplate<ParametType>*>(layer);
	xlayer->updateParamet(paramet);
}


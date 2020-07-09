// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ScreenPass.h"
#include "PostProcess/RenderingCompositionGraph.h"

class FSceneTextureParameters;



/*****************************************Get Depth Class*******************************************************/

/*	存储一个像素的缓存
	depth   深度缓存
	stencil （抠图缓存）*/
struct DepthPixel
{
	float depth;
	char stencil;
	char unused1;
	char unused2;
	char unused3;
};

/*	存储整个视窗的缓存
	data			像素缓存数组
	bufferSizeX		缓存大小X
	bufferSizeY		缓存大小Y
	pixelSizeBytes	像素缓存字节数*/
struct DepthResult
{
	TArray<DepthPixel> data;
	int bufferSizeX;
	int bufferSizeY;
	int pixelSizeBytes;
};



/*	获取深度缓存的类	 */
class RENDERER_API DepthCapture
{
public:
	/*	静态成员，当用户发出一个获取深度缓存的请求后，waitForCapture长度加1，新增DepthResult内容为空
				当系统完成一个深度缓存的请求后，waitForCapture长度减一 */
	static TQueue<DepthResult *, EQueueMode::Mpsc> waitForCapture;
	/*	静态成员，当系统完成一个深度缓存的请求后，finishedCapture长度加1，
				新增DepthResult含有深度缓存信息	*/
	static TQueue<DepthResult *, EQueueMode::Mpsc> finishedCapture;

public:
	/*用户发出一个获取深度缓存的请求时调用*/
	static void AddCapture()
	{
		waitForCapture.Enqueue(new DepthResult());
	}
	/*系统完成一个深度缓存请求后调用*/
	static void FinishedCapture(DepthResult *result)
	{
		finishedCapture.Enqueue(result);
	}
	/*返回是否存在已经完成的请求*/
	static bool HasFinishedCapture()
	{
		return !finishedCapture.IsEmpty();
	}
	/*如果存在已完成的请求，返回一个深度结果*/
	static DepthResult* GetIfExistFinished()
	{
		DepthResult* result = NULL;
		if (!finishedCapture.IsEmpty())
		{
			finishedCapture.Dequeue(result);
		}
		return result;
	}
	/*返回是否存在等待系统执行的请求*/
	static bool HasCaptureRequest()
	{
		return !waitForCapture.IsEmpty();
	}
	/*如果存在待完成的请求，返回一个深度结果（为空）*/
	static DepthResult* GetIfExistRequest()
	{
		DepthResult* result = NULL;
		if (!waitForCapture.IsEmpty())
		{
			waitForCapture.Dequeue(result);
		}
		return result;
	}

	/////////////////////辅助方法
	/**********		获取某一点的深度数据，返回float
	depthResult		深度数据结果
	point			屏幕某个点的位置*/
	static float GetPositionDepth(DepthResult *depthResult,FIntPoint point) {
		return depthResult->data[point.Y * depthResult->bufferSizeX + point.X].depth;
	}


	/**********		深度数据转化为二维浮点数组，返回float的二维数组
	depthResult		深度数据结果*/
	static TArray<TArray<float> > DepthResult2FloatArray(DepthResult *depthResult) {
		TArray<TArray<float> > result;
		for (size_t i = 0; i < depthResult->bufferSizeY; ++i)
		{
			TArray<float> line;
			for (size_t j = 0; j < depthResult->bufferSizeX; ++j)
			{
				line.Add(depthResult->data[i * depthResult->bufferSizeX + j].depth);
			}
			result.Add(line);
		}
		return result;
	}
	
	/**********		将深度结果转化为bmp灰度图，返回是否存储成功的bool值
	depthResult		深度数据结果
	storePath		存储路径，名字后缀必须为.bmg*/
	static bool DepthResultCapture(DepthResult *depthResult, FString storePath) {
		bool imageSavedOk = false;
		TArray<FColor> RawPixels;
		RawPixels.AddUninitialized(depthResult->bufferSizeX * depthResult->bufferSizeY);

		for (uint64 i = 0; i < depthResult->bufferSizeX * depthResult->bufferSizeY; ++i)
		{
			// Switch Blue changes.
			uint8 v = depthResult->data[i].depth * 10000000;
			const uint8 PR = v > 255 ? 255 : v;
			RawPixels[i].B = PR;
		}
		// 保存为灰度图
		imageSavedOk = FFileHelper::CreateBitmap(*storePath, depthResult->bufferSizeX, depthResult->bufferSizeY, RawPixels.GetData());

		return imageSavedOk;
	}
	//friend void AddPostProcessingPasses(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FPostProcessingInputs& Inputs);
};

/*****************************************end******************************************************/





enum class EPostProcessAAQuality : uint32
{
	Disabled,
	// Faster FXAA
	VeryLow,
	// FXAA
	Low,
	// Faster Temporal AA
	Medium,
	// Temporal AA
	High,
	VeryHigh,
	MAX
};

// Returns the quality of post process anti-aliasing defined by CVar.
EPostProcessAAQuality GetPostProcessAAQuality();

// Returns whether the full post process pipeline is enabled. Otherwise, the minimal set of operations are performed.
bool IsPostProcessingEnabled(const FViewInfo& View);

// Returns whether the post process pipeline supports using compute passes.
bool IsPostProcessingWithComputeEnabled(ERHIFeatureLevel::Type FeatureLevel);

// Returns whether the post process pipeline supports propagating the alpha channel.
bool IsPostProcessingWithAlphaChannelSupported();

struct FPostProcessingInputs
{
	const FSceneTextureParameters* SceneTextures = nullptr;
	FRDGTextureRef ViewFamilyTexture = nullptr;
	FRDGTextureRef SceneColor = nullptr;
	FRDGTextureRef SeparateTranslucency = nullptr;
	FRDGTextureRef SeparateModulation = nullptr;
	FRDGTextureRef CustomDepth = nullptr;

	void Validate() const
	{
		check(ViewFamilyTexture);
		check(SceneTextures);
		check(SceneColor);
		check(SeparateTranslucency);
		check(SeparateModulation);
	}
};

void AddPostProcessingPasses(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FPostProcessingInputs& Inputs);

void AddDebugPostProcessingPasses(FRDGBuilder& GraphBuilder, const FViewInfo& View, const FPostProcessingInputs& Inputs);

// For compatibility with composition graph passes until they are ported to Render Graph.
class FPostProcessVS : public FScreenPassVS
{
public:
	FPostProcessVS() = default;
	FPostProcessVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FScreenPassVS(Initializer)
	{}

	void SetParameters(const FRenderingCompositePassContext&) {}
	void SetParameters(FRHICommandList&, FRHIUniformBuffer*) {}
};

/** The context used to setup a post-process pass. */
class FPostprocessContext
{
public:
	FPostprocessContext(FRHICommandListImmediate& InRHICmdList, FRenderingCompositionGraph& InGraph, const FViewInfo& InView);

	FRHICommandListImmediate& RHICmdList;
	FRenderingCompositionGraph& Graph;
	const FViewInfo& View;

	// 0 if there was no scene color available at constructor call time
	FRenderingCompositePass* SceneColor;
	// never 0
	FRenderingCompositePass* SceneDepth;

	FRenderingCompositeOutputRef FinalOutput;
};

/**
 * The center for all post processing activities.
 */
class FPostProcessing
{
public:
	void ProcessES2(FRHICommandListImmediate& RHICmdList, FScene* Scene, const FViewInfo& View);

	void ProcessPlanarReflection(FRHICommandListImmediate& RHICmdList, const FViewInfo& View, TRefCountPtr<IPooledRenderTarget>& OutFilteredSceneColor);

#if WITH_EDITOR
	void AddSelectionOutline(FPostprocessContext& Context);
#endif // WITH_EDITOR

	void AddGammaOnlyTonemapper(FPostprocessContext& Context);

	void OverrideRenderTarget(FRenderingCompositeOutputRef It, TRefCountPtr<IPooledRenderTarget>& RT, FPooledRenderTargetDesc& Desc);
};

/** The global used for post processing. */
extern FPostProcessing GPostProcessing;

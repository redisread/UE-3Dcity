

# 使用HitProxy

类图：

![HitProxy_UML](https://i.loli.net/2020/06/29/fKmuo8a6ZJXrdsQ.png)



流程图：

![流程图](https://i.loli.net/2020/07/02/gV6fLWGka7pjb34.png)



## Game模式使用HitProxy

### 删除相关WITH_EDITOR宏

#### 文件`Engine\Source\Runtime\Renderer\Private\PrimitiveSceneInfo.cpp`

在创建初始化HitProxy的下面的函数注释掉`WITH_EDITOR`宏

![Hitproxy](https://i.loli.net/2020/06/30/XwnrZ4oTie5VCMS.png)

SetHitProxy函数注释掉`WITH_EDITOR`宏

![SetHitProxy](https://i.loli.net/2020/06/30/fC1MV7T9dWny4rh.png)

#### 文件`Engine\Source\Runtime\Renderer\Public\MeshPassProcessor.h`

声明MeshPass的地方注释掉`WITH_EDITOR`宏

![MeshPass-1](https://i.loli.net/2020/06/30/8hQ1BtclxUNo7Hs.png)

在对应的名字匹配的地方也注释掉`WITH_EDITOR`宏

![MeshPass-2](https://i.loli.net/2020/06/30/XQvPNGSCpUxqila.png)



#### 文件`Engine\Source\Runtime\Renderer\Private\SceneHitProxyRendering.h`

去除类`FHitProxyMeshProcessor`的`WITH_EDITOR`宏约束，去除该文件下的所有`WITH_EDITOR`宏

![FHitProxyMeshProcessor](https://i.loli.net/2020/06/30/ZBzmXRxQ5qsgWk6.png)



#### 文件`Engine\Source\Runtime\Renderer\Private\SceneHitProxyRendering.cpp`

去除该文件下的所有`WITH_EDITOR`宏，并且修改下面的判断：

![AddMeshBatch](https://i.loli.net/2020/06/30/5f2IFZpH6BPk9EO.png)



#### 文件`Engine\Source\Runtime\Renderer\Private\SceneVisibility.cpp`

在HitProxy Pass添加静态网格的地方去掉`WITH_EDITOR`宏

![Vis-1](https://i.loli.net/2020/06/30/V8lAg1RoU5KGr7m.png)



在HitProxy Pass添加动态网格的地方去掉`WITH_EDITOR`宏

![Vis-2](https://i.loli.net/2020/06/30/erCh6Wz8lZYJHmN.png)

其他需要取消注释的地方

![Vis-3](https://i.loli.net/2020/06/30/8kH24jTCnqbdPct.png)



#### 文件`Engine\Source\Runtime\Renderer\Private\SceneRendering.cpp`

![setupPass](https://i.loli.net/2020/06/30/KYu8CXrL9okfTyc.png)

其他需要取消注释的地方

![Other](https://i.loli.net/2020/06/30/wyDBm3uVAavxn9i.png)



#### 文件 `\Engine\Source\Runtime\Renderer\Private\ScenePrivate.h`

去除WITH_EDITOR

![image-20200705102705329](https://i.loli.net/2020/07/05/CL2xUrbASXhEz6H.png)



#### 文件`Engine\Source\Runtime\Renderer\Private\RendererScene.cpp`

去掉注释

![image-20200705102806041](https://i.loli.net/2020/07/05/2sUhY9o1ODvQtSP.png)

![image-20200705102903675](https://i.loli.net/2020/07/05/dQYOia3egKWJIhx.png)



#### 文件`Engine\Source\Runtime\Renderer\Private\Renderer.cpp`

![image-20200705103021390](https://i.loli.net/2020/07/05/GNoYAfD1XmkSyIr.png)



#### 文件`Engine\Source\Runtime\Engine\Public\SceneView.h`

![image-20200705103130883](https://i.loli.net/2020/07/05/LcTMC4H89BgV3pt.png)



#### 文件`Engine\Source\Runtime\Engine\Public\PrimitiveSceneProxy.h`

![image-20200705105540137](https://i.loli.net/2020/07/05/Q3LwRBs7EMJ4ITr.png)



#### 文件`Engine\Source\Runtime\Engine\Private\EngineGlobals.cpp`

取消注释

![image-20200705103914680](https://i.loli.net/2020/07/05/BpRNmPgSdjJrxv3.png)

#### 文件：`Engine\Source\Runtime\Engine\Public\EditorSupportDelegates.h的宏`

![image-20200705105257476](https://i.loli.net/2020/07/05/7xSHVm4qRIXZfaw.png)

#### 文件`Engine\Source\Runtime\Engine\Private\UnrealClient.cpp`

![image-20200705105417836](https://i.loli.net/2020/07/05/hGecNHO3bPsy6Zo.png)



#### 文件`Engine\Source\Runtime\Engine\Private\SceneView.cpp`

![image-20200705105508218](https://i.loli.net/2020/07/05/pTrY1tvnKmfuZHl.png)





### 修改源码部分

#### 在FViewportClient类中新建DrawHitProxy函数

文件`UnrealClient.h`

![DrawHitProxy](https://i.loli.net/2020/06/30/rxO23jIFpPaSJXR.png)

#### 在GameViewportClient类中声明并且实现

声明：`\Source\Runtime\Engine\Private\GameViewportClient.h`

![声明](https://i.loli.net/2020/06/30/fasoLE8imnBpWFP.png)

将`GameViewportClient`类中的函数`Draw()`内容复制到该函数`DrawHitProxy`，修改下面的的地方：

![修改](https://i.loli.net/2020/06/30/gTOkPwcWBs41zNu.png)



#### 在GameViewportClient类中重写下面的函数

修改返回值为`true`,路径：`\Source\Runtime\Engine\Private\GameViewportClient.h`

```c++
virtual bool RequiresHitProxyStorage() override { return true; }
```

#### 修改FViewport类中的GetRawHitProxyData函数

在`GetRawHitProxyData`函数中进行以下的修改：`Engine\Source\Runtime\Engine\Private\UnrealClient.cpp`

![修改](https://i.loli.net/2020/06/30/PuyFWDxzkEfmncv.png)



### 调用

可以使用以下的代码

```c++
UGameViewportClient* gameViewport = GWorld->GetGameViewport();
gameViewport->GetViewportSize(viewportSize);	// 获取视口大小
TArray<FColor> outData;
FIntRect rect(0, 0, viewportSize.X, viewportSize.Y);
TSet<AActor*> OutActors;
TSet<UModel*> OutModels;
// 设置获取HitProxy的请求
GWorld->GetGameViewport()->GetEngineShowFlags()->HitProxies = true;
gameViewport->Viewport->InvalidateHitProxy();	// 请求刷新HitProxy

// 获取指定像素的HitProxy
HHitProxy* hitProxy = gameViewport->Viewport->GetHitProxy(viewportSize.X / 2, viewportSize.Y / 2);
// 获取当前视口下所有可见的Actor与Models
// gameViewport->Viewport->GetActorsAndModelsInHitProxy(rect, OutActors, OutModels);  
// 获取指定矩阵下的所有像素的HitProxy
// TArray<HHitProxy*> outMap;
// gameViewport->Viewport->GetHitProxyMap(FIntRect(100, 100), outMap);	
// 关闭HitProxy的请求
GWorld->GetGameViewport()->GetEngineShowFlags()->HitProxies = false;
if(hitProxy)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("HitProxy Name: x: %s;Pos (%f,%f)"), *static_cast<HActor*>(hitProxy)->Actor->GetName(), MousePosition.X, MousePosition.Y));
```


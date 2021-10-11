// Fill out your copyright notice in the Description page of Project Settings.
// publisher name:Frey
// 2021 of publication
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Materials/MaterialInstance.h"
#include "Materials/Material.h"
#include "Math/Color.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/TextureCube.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/NameTypes.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "WaterBase.h"
#include "WeatherSystemMaster.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UAudioComponent;
class USoundCue;
class UMaterialInstance;
class UTimelineComponent;
class UCurveFloat;

UCLASS()
class WEATHERSYSTEM_API AWeatherSystemMaster : public AActor
{
	GENERATED_BODY()
		// 根组件
		class USceneComponent* RootComponent;
	// 太阳光
	UPROPERTY(EditAnywhere)
		class UDirectionalLightComponent* DirectionalLight;
	// 天空光
	UPROPERTY(EditAnywhere)
		class USkyLightComponent* SkyLight;
	class UTextureCube* CubemapTemp;
	// 天空大气
	UPROPERTY(EditAnywhere)
		class USkyAtmosphereComponent* SkyAtmosphere;
	// 后处理
	UPROPERTY(EditAnywhere)
		class UPostProcessComponent* PostProcessComp;
	class UTexture* LUT;
	// 体积云
	UPROPERTY(EditAnywhere)
		class UVolumetricCloudComponent* VolumetricCloud;
	UMaterialInstance* CloudMaterial;
	// 高度雾
	UPROPERTY(EditAnywhere)
		class UExponentialHeightFogComponent* ExponentialHeightFog;
	//天空球
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* SkySphere;
	class UStaticMesh* SkySphereMesh;
	class UMaterial* SkyMaterial;
	//粒子
	UPROPERTY(EditAnywhere)
		UNiagaraComponent* RainFX;
	UNiagaraSystem* NS_RainFall;
	UPROPERTY(EditAnywhere)
		UNiagaraComponent* SnowFX;
	UNiagaraSystem* NS_SnowFall;
	//音效
	UPROPERTY(EditAnywhere)
		UAudioComponent* RainAudio;
	USoundCue* RainSound;
	UPROPERTY(EditAnywhere)
		UAudioComponent* ThunderAudio;
	USoundCue* ThunderSound;
	UPROPERTY(EditAnywhere)
		UAudioComponent* SnowAudio;
	USoundCue* SnowSound;
	UPROPERTY(EditAnywhere)
		UAudioComponent* BirdsAudio;
	USoundCue* BirdsSound;
	UPROPERTY(EditAnywhere)
		UAudioComponent* WindAudio;
	USoundCue* WindSound;
	UPROPERTY(EditAnywhere)
		UAudioComponent* CicadaAudio;
	USoundCue* CicadaSound;
public:
	// Sets default values for this actor's properties
	AWeatherSystemMaster();
	//获取水的基类
	TSubclassOf<class AWaterBase> WaterBase;
	//存储当前场景AWaterBase数量
	TArray<AActor*> WaterArray;
	//天气随机函数
	void WeatherRandomFunc();
	//获取角色位置
	void GetPlayerLocation();
	//播放雷声
	void ThunderAudioPlay();
	//默认参数配置
	void SetupDefaults();
	//记录下雪状态
	UPROPERTY(BlueprintReadOnly)
		bool bSnowIsComing = false;
	//记录下雨状态
	UPROPERTY(BlueprintReadOnly)
		bool bRainIsComing = false;
	//记录是否在夜晚
	UPROPERTY(BlueprintReadOnly)
		bool bIsNight = false;
	//天气随机是否循环
	bool WeatherRandomTimeLoop = true;
	//天气状态
	int WeatherSet = 4;
	//天气最小随机值
	int WeatherMin = 0;
	//天气最大随机值
	int WeatherMax = 10;
	//材质参数集
	class UMaterialParameterCollection* MPC;//MPC_Weather
	class UMaterialParameterCollection* MPC_Tree;//MPC_Tree_Wind


/*Randomly switch weather interval (seconds)
The first random weather is one second after the start of the game
随机切换天气的间隔时间（秒）
第一次随机天气是游戏开始后一秒*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Core")
		float WeatherRandomTime = 60.0f;
/*If enabled will keep raining
When AlwaysRain and AlwaysSnow are turned on at the same time, it is sleet weather
开启后将一直下雨
当AlwaysRain跟AlwaysSnow同时开启时是雨夹雪的天气*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Core")
		bool AlwaysRain = false;
/*If enabled will keep snowing
When AlwaysRain and AlwaysSnow are turned on at the same time, it is sleet weather
开启后将一直下雪
当AlwaysRain跟AlwaysSnow同时开启时是雨夹雪的天气*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Core")
		bool AlwaysSnow = false;
/*If enabled will keep sunny
When AlwaysRain, AlwaysSnow, and AlwaysSunny are turned on at the same time, AlwaysSunny does not take effect
开启后将一直晴天
当AlwaysRain跟AlwaysSnow跟AlwaysSunny同时开启时AlwaysSunny不生效*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Core")
		bool AlwaysSunny = false;
/*If enabled will keep Daytime,No day-night cycle
When AlwaysDayTime and AlwaysNight are turned on at the same time, AlwaysNight does not take effect
开启后将一直白天，没有昼夜更替
当AlwaysDayTime跟AlwaysNight同时开启AlwaysNight不生效*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Core")
		bool AlwaysDayTime = false;
/*If enabled will keep night, No day-night cycle
When AlwaysDayTime and AlwaysNight are turned on at the same time, AlwaysNight does not take effect
开启后将一直黑夜，没有昼夜更替
当AlwaysDayTime跟AlwaysNight同时开启AlwaysNight不生效*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Core")
		bool AlwaysNight = false;
/*The color of dark clouds when it rains or snows
It will transition from the color of SnowColorOfDay to this color when it rains or snows to a sunny day
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
当下雨或下雪时乌云的颜色
下雨或下雪到晴天会从SnowColorOfDay的颜色过渡到这个颜色
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Core")
		FLinearColor DarkClouds;


/*Maximum number when it snows
It will transition from zero to this value when it enters the snowing state, and transition to 0 when the snow stops.
The time of this process is determined by the value of Time in SnowFalling in the Curve folder (seconds)
下雪时最大的数量
当进入下雪状态后会从零过渡到这个值，雪停后过渡到0
这个过程的时间由Curve文件中SnowFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Snow")
		float MaxSnowFalling = 20000.0f;
/*Maximum snow cover when it snows
When it enters the snowing state, it will transition from 3 to this value, and transition to 3 when the snow stops.
The time of this process is determined by the value of Time in Frozen in the Curve folder (seconds)
下雪时最大的积雪覆盖率
当进入下雪状态后会从3过渡到这个值，雪停后过渡到3
这个过程的时间由Curve文件中Frozen里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Snow")
		float SnowBlendMax = 7.0f;
/*The color of snow during the day
When entering the daytime state, it will transition from the color of SnowColorOfNight to this color
The time of this process is determined by the value of Time in IsNightLerp in the Curve folder (seconds)
白天雪的颜色
当进入白天的状态后会从SnowColorOfNight的颜色过渡到这个颜色
这个过程的时间由Curve文件中IsNightLerp里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Snow")
		FLinearColor SnowColorOfDay;
/*Snow color at night
When entering the night state, it will transition from the color of SnowColorOfDay to this color
The time of this process is determined by the value of Time in IsNightLerp in the Curve folder (seconds)
晚上雪的颜色
当进入晚上的状态后会从SnowColorOfDay的颜色过渡到这个颜色
这个过程的时间由Curve文件中IsNightLerp里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Snow")
		FLinearColor SnowColorOfNight;


/*The color of the rain during the day
When entering the daytime state, it will transition from the color of RainColorOfNight to this color
The time of this process is determined by the value of Time in IsNightLerp in the Curve file (seconds)
白天雨的颜色
当进入白天的状态后会从RainColorOfNight的颜色过渡到这个颜色
这个过程的时间由Curve文件中IsNightLerp里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Rain")
		FLinearColor RainColorOfDay;
/*Color of rain at night
When entering the night state, it will transition from the color of RainColorOfDay to this color
The time of this process is determined by the value of Time in IsNightLerp in the Curve file (seconds)
晚上雨的颜色
当进入晚上的状态后会从RainColorOfDay的颜色过渡到这个颜色
这个过程的时间由Curve文件中IsNightLerp里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Rain")
		FLinearColor RainColorOfNight;
/*Maximum number when it rains
When it is raining, it will transition from zero to this value, and transition to zero when the rain stops.
The time of this process is determined by the value of Time in RainFalling in the Curve file (seconds)
下雨时最大的数量
当进入下雨状态后会从零过渡到这个值，雨停后过渡到零
这个过程的时间由Curve文件中RainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Rain")
		float MaxRainFalling = 20000.0f;
/*Maximum amount of rain and fog when it rains
When it is raining, it will transition from zero to this value, and transition to zero when the rain stops.
The time of this process is determined by the value of Time in RainFalling in the Curve file (seconds)
下雨时最大的雨雾数量
当进入下雨状态后会从零过渡到这个值，雨停后过渡到零
这个过程的时间由Curve文件中RainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Rain")
		float MaxRainFogFalling = 100.0f;
/*The intensity of the mixing of the ripples on the water surface when it rains
When it is raining, it will transition from zero to this value, and transition to zero when the rain stops.
The time of this process is determined by the value of Time in RainFalling in the Curve file (seconds)
下雨时水面的涟漪混合的强度
当进入下雨状态后会从零过渡到这个值，雨停后过渡到零
这个过程的时间由Curve文件中RainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Rain")
		float RainDropsBlendMax = 0.1f;
/*The roughness when it rains (current object roughness * GetWetRoughness) reduces the roughness to simulate the effect of getting wet
The MF_WeatherBlend function needs to be added to the material, and it will transition from 1 to this value when it enters the raining state
Transition to 1 after the rain stops. The time of this process is determined by the value of Time in GetWet in the Curve file (seconds)
下雨时的粗糙度（当前物体粗糙度 * GetWetRoughness）降低粗糙度模拟淋湿的效果
需要在材质中添加MF_WeatherBlend函数，当进入下雨状态后会从1过渡到这个值
雨停后过渡到1，这个过程的时间由Curve文件中GetWet里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Rain")
		float GetWetRoughness = 0.3f;
/*The specular intensity when it rains (the current object specular intensity * GetWetSpecular) to enhance the effect of the specular simulating wet
Need to add the MF_WeatherBlend function to the material
When it enters the rainy state, it will transition from 1 to this value
Transition to 1 after the rain stops
The time of this process is determined by the value of Time in GetWet in the Curve file (seconds)
下雨时的高光强度（当前物体高光强度 * GetWetSpecular）加强高光模拟淋湿的效果
需要在材质中添加MF_WeatherBlend函数
当进入下雨状态后会从1过渡到这个值
雨停后过渡到1
这个过程的时间由Curve文件中GetWet里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Rain")
		float GetWetSpecular = 2.0f;
/*Interval of random thunder sound when it is raining (seconds)
下雨时随机播放雷声的间隔时间（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Rain")
		float ThunderWaitingTime = 30.0f;
/*The minimum volume of thunder to play randomly when it is raining
Randomly select and play at the middle value of ThunderMinVolume and ThunderMaxVolume
下雨时随机播放雷声的最小音量
在ThunderMinVolume跟ThunderMaxVolume的中间值随机取值播放*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Rain")
		float ThunderMinVolume = 0.5f;
/*The maximum volume of random thunder when it is raining
Randomly select and play at the middle value of ThunderMinVolume and ThunderMaxVolume
下雨时随机播放雷声的最大音量
在ThunderMinVolume跟ThunderMaxVolume的中间值随机取值播放*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Rain")
		float ThunderMaxVolume = 1.5f;


/*The brightness of the sun on a sunny day
It will transition from the value of SunIntensityOfRainOrSnow to this value when it rains or snows to a sunny day
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
晴天的太阳光亮度
下雨或下雪到晴天会从SunIntensityOfRainOrSnow的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Sun")
		float SunIntensityOfSunny = 10.0f;
/*The brightness of the sun when it rains or when it rains
From sunny days to rain or snow, the value of SunIntensityOfSunny will transition to this value
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
下雨或下雨时太阳光的亮度
晴天到下雨或下雪会从SunIntensityOfSunny的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Sun")
		float SunIntensityOfRainOrSnow = 3.0f;
/*Color temperature during daytime and sunny day
It will transition from the value of SunTemperatureOfNight to this value when it rains or snows to a sunny day
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
白天跟晴天的色温
下雨或下雪到晴天会从SunTemperatureOfNight的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Sun")
		float SunTemperatureOfDaytime = 5000.0f;
/*The color temperature at night and in rainy and snowy days
From sunny days to rain or snow, the value of SunTemperatureOfDaytime will transition to this value
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
晚上跟雨雪天的色温
晴天到下雨或下雪会从SunTemperatureOfDaytime的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Sun")
		float SunTemperatureOfNight = 8000.0f;


/*Fog intensity on a sunny day
It will transition from the value of FogDensityOfRainOrSnow to this value when it rains or snows to a sunny day
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
晴天的雾强度
下雨或下雪到晴天会从FogDensityOfRainOrSnow的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Fog")
		float FogDensityOfSunny = 0.02f;
/*Fog intensity in rainy or snowy days
From sunny days to rain or snow, the value of FogDensityOfSunny will transition to this value
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
雨天或雪天的雾强度
晴天到下雨或下雪会从FogDensityOfSunny的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Fog")
		float FogDensityOfRainOrSnow = 0.05f;
/*Attenuation intensity of fog on a sunny day
It will transition from the value of FogHeightFalloffOfRainOrSnow to this value when it rains or snows to a sunny day
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
晴天的雾衰减强度
下雨或下雪到晴天会从FogHeightFalloffOfRainOrSnow的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Fog")
		float FogHeightFalloffOfSunny = 0.2f;
/*Fog attenuation intensity when it rains or snows
From sunny to rain or snow, the value of FogHeightFalloffOfSunny will transition to this value
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
下雨或下雪时雾衰减强度
晴天到下雨或下雪会从FogHeightFalloffOfSunny的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Fog")
		float FogHeightFalloffOfRainOrSnow = 0.3f;


/*Wind speed when it rains or snows
From a sunny day to rain or snow, it will transition from the value of WindSpeedOfSunny to this value
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
下雨或下雪时的风速
晴天到下雨或下雪会从WindSpeedOfSunny的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Wind")
		float WindSpeedOfSnowOrRain = 6.0f;
/*Wind speed on a sunny day
It will transition from the value of WindSpeedOfSnowOrRain to this value when it rains or snows to a sunny day
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
晴天的风速
下雨或下雪到晴天会从WindSpeedOfSnowOrRain的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Wind")
		float WindSpeedOfSunny = 3.0f;
/*Wind intensity when it rains or snows
From a sunny day to rain or snow, the value of WindWeightOfSunny will transition to this value
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
下雨或下雪时风强度
晴天到下雨或下雪会从WindWeightOfSunny的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Wind")
		float WindWeightOfSnowOrRain = 8.0f;
/*Wind intensity on a sunny day
It will transition from the value of WindWeightOfSnowOrRain to this value when it rains or snows to a sunny day
The time of this process is determined by the value of Time in SnowOrRainFalling in the Curve folder (seconds)
晴天的风强度
下雨或下雪到晴天会从WindWeightOfSnowOrRain的值过渡到这个值
这个过程的时间由Curve文件夹中SnowOrRainFalling里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | Wind")
		float WindWeightOfSunny = 2.0f;


/*Sky brightness during the day
From day to night, it will transition from the value of SkyLightIntensityOfNight to this value
The time of this process is determined by the value of Time in SunAngle in the Curve folder (seconds)
白天天空光亮度
白天到晚上会从SkyLightIntensityOfNight的值过渡到这个值
这个过程的时间由Curve文件夹中SunAngle里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | SkyLight")
		float SkyLightIntensityOfDay = 1.2f;
/*Night sky brightness
It will transition from the value of SkyLightIntensityOfDay to this value from night to day
The time of this process is determined by the value of Time in SunAngle in the Curve folder (seconds)
晚上天空光亮度
晚上到白天会从SkyLightIntensityOfDay的值过渡到这个值
这个过程的时间由Curve文件夹中SunAngle里Time的值决定（秒）*/
	UPROPERTY(EditAnywhere, Category = "WeatherSystem | SkyLight")
		float SkyLightIntensityOfNight = 0.3f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//定时器
	FTimerHandle RandomWeatherTimerHandle;
	FTimerHandle ThunderTimerHandle;
	//更新时间轴浮点轨道
	FOnTimelineFloat UpdateIsNightFloat;
	FOnTimelineFloat UpdateSunFloat;
	FOnTimelineFloat UpdateSnowOrRainFallingFloat;
	FOnTimelineFloat UpdateRainFallingFloat;
	FOnTimelineFloat UpdateSnowFallingFloat;
	FOnTimelineFloat UpdateGetWetFloat;
	FOnTimelineFloat UpdateFrozenFloat;
	//时间轴相关函数
	UFUNCTION()//更新夜晚状态
		void UpdateIsNightTimeLine(float IsNightOutput);
	UFUNCTION()//更新昼夜更替
		void UpdateSunTimeLine(float SunOutput);
	UFUNCTION()//更新雨雪天相关
		void UpdateSnowOrRainFallingTimeLine(float SnowOrRainFallingOutput);
	UFUNCTION()//更新雨天淋湿效果
		void UpdateGetWetTimeLine(float GetWetOutput);
	UFUNCTION()//更新雪天积雪冰冻效果
		void UpdateFrozenTimeLine(float FrozenOutput);
	UFUNCTION()//更新下雨
		void UpdateRainFallingTimeLine(float RainFallingOutput);
	UFUNCTION()//更新下雪
		void UpdateSnowFallingTimeLine(float SnowFallingOutput);
	//时间轴组件
	UPROPERTY(EditAnywhere)
		UTimelineComponent* IsNightTimeLine;
	UPROPERTY(EditAnywhere)
		UTimelineComponent* SunTimeLine;
	UPROPERTY(EditAnywhere)
		UTimelineComponent* SnowOrRainFallingTimeLine;
	UPROPERTY(EditAnywhere)
		UTimelineComponent* RainFallingTimeLine;
	UPROPERTY(EditAnywhere)
		UTimelineComponent* SnowFallingTimeLine;
	UPROPERTY(EditAnywhere)
		UTimelineComponent* GetWetTimeLine;
	UPROPERTY(EditAnywhere)
		UTimelineComponent* FrozenTimeLine;
	//曲线
	UPROPERTY(EditAnywhere)
		UCurveFloat* IsNightCurve;
	UPROPERTY(EditAnywhere)
		UCurveFloat* SunAngleCurve;
	UPROPERTY(EditAnywhere)
		UCurveFloat* SnowOrRainFallingCurve;
	UPROPERTY(EditAnywhere)
		UCurveFloat* RainFallingCurve;
	UPROPERTY(EditAnywhere)
		UCurveFloat* SnowFallingCurve;
	UPROPERTY(EditAnywhere)
		UCurveFloat* GetWetCurve;
	UPROPERTY(EditAnywhere)
		UCurveFloat* FrozenCurve;
private:

};

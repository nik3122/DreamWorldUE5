// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Components/ActorComponent.h"
#include "WorldTimerComponent.generated.h"

class ASkyLight;
class ADirectionalLight;

/**
 * 时间管理组件
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class DREAMWORLD_API UWorldTimerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWorldTimerComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	ASkyLight* SkyLight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	ADirectionalLight* SunLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float SecondsOfDay;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float SunriseTime;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float SunsetTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	int TimeSeconds;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	int CurrentDay;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	int CurrentHour;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	int CurrentMinute;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	int CurrentSeconds;
		
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateTimer();
	
	UFUNCTION(BlueprintCallable)
	void SetTimeSeconds(int InTimeSeconds, bool bUpdateTimer = true);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetSecondsOfDay() const { return SecondsOfDay; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetSunriseTime() const { return SunriseTime; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetSunsetTime() const { return SunsetTime; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetTimeSeconds() const { return TimeSeconds; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetCurrentDay() const { return CurrentDay; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetCurrentHour() const { return CurrentHour; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetCurrentMinute() const { return CurrentMinute; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetCurrentSeconds() const { return CurrentSeconds; }
};
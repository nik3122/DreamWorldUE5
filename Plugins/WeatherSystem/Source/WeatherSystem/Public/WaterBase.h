// Fill out your copyright notice in the Description page of Project Settings.
// publisher name:Frey
// 2021 of publication
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"
#include "Materials/Material.h"
#include "WaterBase.generated.h"

UCLASS()
class WEATHERSYSTEM_API AWaterBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaterBase();
	//根组件
	class USceneComponent* RootComponent;
	UPROPERTY(EditAnywhere)
	class UMaterialInstance* WaterMaterial;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* WaterComp;
	UPROPERTY(EditAnywhere)
	class UStaticMesh* WaterMesh;
	void SetWaterBloackAll();
	void SetWaterNoCollision();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Components/BoxComponent.h"

#include "DWCharacterPart.generated.h"

/**
 * 角色部位组件
 */
UCLASS()
class DREAMWORLD_API UDWCharacterPart : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	UDWCharacterPart(const FObjectInitializer& ObjectInitializer);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ECharacterPartType CharacterPartType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UVoxel* LastOverlapVoxel;

public:
	UFUNCTION(BlueprintPure)
	ECharacterPartType GetCharacterPartType() const { return CharacterPartType; }

	UFUNCTION(BlueprintCallable)
	void SetCharacterPartType(ECharacterPartType InCharacterPartType) { this->CharacterPartType = InCharacterPartType; }

	UFUNCTION(BlueprintPure)
	UVoxel* GetLastOverlapVoxel() const { return LastOverlapVoxel; }

	UFUNCTION(BlueprintPure)
	ADWCharacter* GetOwnerCharacter() const;
};

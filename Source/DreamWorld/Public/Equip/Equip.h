// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/Actor.h"
#include "Equip.generated.h"

class ADWCharacter;
class UStaticMeshComponent;

/**
 * ×°±¸
 */
UCLASS()
class DREAMWORLD_API AEquip : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquip();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	FName EquipID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	ADWCharacter* OwnerCharacter;

	FActiveGameplayEffectHandle EffectHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
		
	UFUNCTION(BlueprintCallable)
	virtual void Initlize(ADWCharacter* InOwnerCharacter);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetVisible(bool bVisible);

	UFUNCTION(BlueprintNativeEvent)
	void OnAssemble();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnDischarge();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FEquipData GetEquipData();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FName GetEquipID() const { return EquipID; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ADWCharacter* GetOwnerCharacter() const { return OwnerCharacter; }
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "GameFramework/Actor.h"
#include "Vitality/Vitality.h"
#include "AbilitySystemInterface.h"
#include "Abilities/DWAbilitySystemComponent.h"
#include "VitalityObject.generated.h"

class AChunk;
class ADWCharacter;
class UBoxComponent;
class UWidgetVitalityHPComponent;
class UInventory;
class UDWAttributeSet;

/**
 * ������������
 */
UCLASS()
class DREAMWORLD_API AVitalityObject : public AActor, public IVitality, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVitalityObject();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VitalityStates")
	bool bDead;

protected:
	// stats
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VitalityStats")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VitalityStats")
	FString RaceID;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VitalityStats")
	int32 Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VitalityStats")
	int32 EXP;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VitalityStats")
	int32 BaseEXP;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VitalityStats")
	int32 EXPFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VitalityStats")
	FInventoryData InventoryData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VitalityStats")
	AChunk* OwnerChunk;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UBoxComponent* BoxComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UWidgetVitalityHPComponent* WidgetVitalityHP;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Component")
	UDWAbilitySystemComponent* AbilitySystem;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Component")
	UDWAttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	UInventory* Inventory;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void LoadData(FVitalityObjectData InSaveData);

	UFUNCTION(BlueprintPure)
	FVitalityObjectData ToData();

	UFUNCTION(BlueprintCallable)
	virtual void SpawnWidgetWorldText(EWorldTextType InContextType, FString InContext) override;

	UFUNCTION(BlueprintCallable)
	virtual void Death(ADWCharacter* InKiller = nullptr) override;

	UFUNCTION(BlueprintCallable)
	virtual void Revive() override;
	
	UFUNCTION(BlueprintCallable)
	virtual FGameplayAbilitySpecHandle AcquireAbility(TSubclassOf<UDWGameplayAbility> InAbility, int32 InLevel = 1) override;

	UFUNCTION(BlueprintCallable)
	virtual bool ActiveAbility(FGameplayAbilitySpecHandle AbilityHandle, bool bAllowRemoteActivation = false) override;

	virtual bool ActiveAbility(TSubclassOf<UDWGameplayAbility> AbilityClass, bool bAllowRemoteActivation = false) override;

	virtual bool ActiveAbility(const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation = false) override;

	UFUNCTION(BlueprintCallable)
	virtual void CancelAbility(UDWGameplayAbility* Ability) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle& AbilityHandle) override;

	virtual void CancelAbilities(const FGameplayTagContainer* WithTags=nullptr, const FGameplayTagContainer* WithoutTags=nullptr, UDWGameplayAbility* Ignore=nullptr) override;
	
	virtual void CancelAllAbilities(UDWGameplayAbility* Ignore=nullptr) override;

	UFUNCTION(BlueprintCallable)
	virtual void ModifyHealth(float InDetlaValue) override;
				
	UFUNCTION(BlueprintCallable)
	virtual void ModifyEXP(float InDetlaValue) override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void Refresh() override;

	UFUNCTION(BlueprintPure)
	bool IsDead() const override;

	UFUNCTION(BlueprintPure)
	FString GetName() const override { return Name; }

	UFUNCTION(BlueprintCallable)
	void SetName(const FString& InName) override;
	
	UFUNCTION(BlueprintPure)
	FString GetRaceID() const override { return RaceID; }

	UFUNCTION(BlueprintCallable)
	void SetRaceID(const FString& InRaceID) override;

	UFUNCTION(BlueprintCallable)
	void SetLevelC(int32 InLevel);

	UFUNCTION(BlueprintPure)
	int32 GetLevelC() const override { return Level; }

	UFUNCTION(BlueprintPure)
	int32 GetEXP() const override { return EXP; }
		
	UFUNCTION(BlueprintCallable)
	void SetEXP(int32 InEXP);
		
	UFUNCTION(BlueprintPure)
	int32 GetBaseEXP() const override { return BaseEXP; }
			
	UFUNCTION(BlueprintPure)
	int32 GetEXPFactor() const override { return EXPFactor; }

	UFUNCTION(BlueprintPure)
	int32 GetMaxEXP() const override;

	UFUNCTION(BlueprintPure)
	int32 GetTotalEXP() const override;

	UFUNCTION(BlueprintPure)
	FString GetHeadInfo() const override;
			
	UFUNCTION(BlueprintPure)
	float GetHealth() const override;
					
	UFUNCTION(BlueprintCallable)
	void SetHealth(float InValue) override;

	UFUNCTION(BlueprintPure)
	float GetMaxHealth() const override;
		
	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float InValue) override;
	
	UFUNCTION(BlueprintPure)
	float GetPhysicsDamage() const override;
	
	UFUNCTION(BlueprintPure)
	float GetMagicDamage() const override;

	UFUNCTION(BlueprintPure)
	AChunk* GetOwnerChunk() const { return OwnerChunk; }
	
	UFUNCTION(BlueprintCallable)
	void SetOwnerChunk(AChunk* InOwnerChunk) { OwnerChunk = InOwnerChunk; }

	UFUNCTION(BlueprintPure)
	UInventory* GetInventory() const { return Inventory; }
		
	UFUNCTION(BlueprintPure)
	UWidgetVitalityHP* GetWidgetVitalityHPWidget();

	UFUNCTION(BlueprintPure)
	UAbilitySystemComponent* GetAbilitySystemComponent() const {return AbilitySystem;}

public:
	virtual void HandleDamage(const float LocalDamageDone, FHitResult HitResult, const struct FGameplayTagContainer& SourceTags, ADWCharacter* SourceCharacter, AActor* SourceActor) override;
		
	virtual void HandleNameChanged(const FString& NewValue) override;

	virtual void HandleRaceIDChanged(const FString& NewValue) override;

	virtual void HandleLevelChanged(int32 NewValue, int32 DeltaValue = 0) override;

	virtual void HandleEXPChanged(int32 NewValue, int32 DeltaValue = 0) override;

	virtual void HandleHealthChanged(float NewValue, float DeltaValue = 0.f) override;
	
	virtual void HandleMaxHealthChanged(float NewValue, float DeltaValue = 0.f) override;
};

#pragma once

#include "DreamWorld.h"
#include "Interface.h"
#include "Vitality.generated.h"

class ADWCharacter;
class UInventorySlot;
class UInventorySkillSlot;
class UDWGameplayAbility;

UINTERFACE()
class DREAMWORLD_API UVitality : public UInterface
{
	GENERATED_BODY()
};

class DREAMWORLD_API IVitality
{
	GENERATED_BODY()

public:
	virtual void Spawn() = 0;
	
	virtual void Revive() = 0;

	virtual void Death(ADWCharacter* InKiller = nullptr) = 0;
		
	virtual void ResetData(bool bRefresh = false) = 0;

	virtual void RefreshData() = 0;
	
	virtual void ModifyHealth(float InDeltaValue) = 0;
	
	virtual void ModifyEXP(float InDeltaValue) = 0;

	virtual bool IsDead() const = 0;
	
	virtual FString GetNameC() const = 0;

	virtual void SetNameC(const FString& InName) = 0;
		
	virtual FString GetRaceID() const = 0;

	virtual void SetRaceID(const FString& InRaceID) = 0;
	
	virtual int32 GetLevelC() const = 0;

	virtual void SetLevelC(int32 InLevel) = 0;

	virtual int32 GetEXP() const = 0;
				
	virtual void SetEXP(int32 InEXP) = 0;

	virtual int32 GetBaseEXP() const = 0;
	
	virtual int32 GetEXPFactor() const = 0;

	virtual int32 GetMaxEXP() const = 0;

	virtual int32 GetTotalEXP() const = 0;

	virtual FString GetHeadInfo() const = 0;
		
	virtual float GetHealth() const = 0;
				
	virtual void SetHealth(float InValue)  = 0;

	virtual float GetMaxHealth() const = 0;
			
	virtual void SetMaxHealth(float InValue) = 0;

	virtual float GetPhysicsDamage() const = 0;
		
	virtual float GetMagicDamage() const = 0;
	
	virtual class UInventory* GetInventory() const = 0;

	virtual FGameplayAbilitySpecHandle AcquireAbility(TSubclassOf<UDWGameplayAbility> InAbility, int32 InLevel = 1) = 0;

	virtual bool ActiveAbility(FGameplayAbilitySpecHandle AbilityHandle, bool bAllowRemoteActivation = false) = 0;

	virtual bool ActiveAbility(TSubclassOf<UDWGameplayAbility> AbilityClass, bool bAllowRemoteActivation = false) = 0;

	virtual bool ActiveAbility(const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation = false) = 0;

	virtual void CancelAbility(UDWGameplayAbility* Ability) = 0;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle& AbilityHandle) = 0;

	virtual void CancelAbilities(const FGameplayTagContainer* WithTags=nullptr, const FGameplayTagContainer* WithoutTags=nullptr, UDWGameplayAbility* Ignore=nullptr) = 0;
	
	virtual void CancelAllAbilities(UDWGameplayAbility* Ignore=nullptr) = 0;

	virtual void SpawnWidgetWorldText(EWorldTextType InContextType, FString InContext) = 0;

	virtual void HandleDamage(const float LocalDamageDone, FHitResult HitResult, const struct FGameplayTagContainer& SourceTags, ADWCharacter* SourceCharacter, AActor* SourceActor) = 0;
		
	virtual void HandleNameChanged(const FString& NewValue) = 0;

	virtual void HandleRaceIDChanged(const FString& NewValue) = 0;
	
	virtual void HandleLevelChanged(int32 NewValue, int32 DeltaValue = 0) = 0;

	virtual void HandleEXPChanged(int32 NewValue, int32 DeltaValue = 0) = 0;

	virtual void HandleHealthChanged(float NewValue, float DeltaValue = 0.f) = 0;

	virtual void HandleMaxHealthChanged(float NewValue, float DeltaValue = 0.f) = 0;
};
#pragma once

#include "DreamWorld.h"
#include "Abilities/DWAttributeSet.h"
#include "DWCharacterAttributeSet.generated.h"

/**
 * ½ÇÉ«ÊôÐÔ¼¯
 */
UCLASS()
class DREAMWORLD_API UDWCharacterAttributeSet : public UDWAttributeSet
{
	GENERATED_BODY()

public:
	UDWCharacterAttributeSet();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, Mana)
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, MaxMana)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, Stamina)
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, MaxStamina)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, MoveSpeed)
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData SwimSpeed;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, SwimSpeed)
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData RideSpeed;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, RideSpeed)
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData FlySpeed;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, FlySpeed)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData RotationSpeed;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, RotationSpeed)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData JumpForce;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, JumpForce)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData DodgeForce;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, DodgeForce)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData AttackForce;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, AttackForce)
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData RepulseForce;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, RepulseForce)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, AttackSpeed)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData AttackCritRate;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, AttackCritRate)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData AttackStealRate;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, AttackStealRate)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData DefendRate;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, DefendRate)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData DefendScope;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, DefendScope)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData PhysicsDefRate;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, PhysicsDefRate)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData MagicDefRate;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, MagicDefRate)
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData ToughnessRate;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, ToughnessRate)
			
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData StaminaRegenSpeed;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, StaminaRegenSpeed)
			
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterAttributes")
	FGameplayAttributeData StaminaExpendSpeed;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, StaminaExpendSpeed)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interrupt")
	FGameplayAttributeData Interrupt;
	ATTRIBUTE_ACCESSORS(UDWCharacterAttributeSet, Interrupt)

public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;
};
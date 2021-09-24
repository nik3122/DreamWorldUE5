#pragma once

#include "DreamWorld.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DWAttributeSet.generated.h"

#define	ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 属性集
 */
UCLASS()
class DREAMWORLD_API UDWAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UDWAttributeSet();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDWAttributeSet, Health)
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDWAttributeSet, MaxHealth)

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	FGameplayAttributeData PhysicsDamage;
	ATTRIBUTE_ACCESSORS(UDWAttributeSet, PhysicsDamage)
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	FGameplayAttributeData MagicDamage;
	ATTRIBUTE_ACCESSORS(UDWAttributeSet, MagicDamage)

public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;

protected:
	/** 当MaxAttribute发生改变时按比例设置Attribute */
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

public:
	FGameplayAttributeData* GetAttributeData(const FGameplayAttribute& Attribute);
};
#include "Abilities/Character/DWCharacterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Character/DWCharacter.h"

UDWCharacterAttributeSet::UDWCharacterAttributeSet()
:	Mana(100.f),
	MaxMana(100.f),
	Stamina(100.f),
	MaxStamina(100.f),
	MoveSpeed(350.f),
	SwimSpeed(350.f),
	RideSpeed(350.f),
	FlySpeed(350.f),
	RotationSpeed(360.f),
	JumpForce(420.f),
	DodgeForce(500.f),
	AttackForce(10.f),
	RepulseForce(0.f),
	AttackSpeed(1.f),
	AttackCritRate(0.f),
	AttackStealRate(0.f),
	DefendRate(0.f),
	DefendScope(0.f),
	PhysicsDefRate(0.f),
	MagicDefRate(0.f),
	ToughnessRate(0.f),
	StaminaRegenSpeed(10.f),
	StaminaExpendSpeed(5.f)
{
	
}

void UDWCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	ADWCharacter* TargetCharacter = Cast<ADWCharacter>(AbilityComp->GetAvatarActor());
	
	const float CurrentValue = Attribute.GetGameplayAttributeData(this)->GetCurrentValue();
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
		if (TargetCharacter)
		{
			TargetCharacter->HandleManaChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		if (TargetCharacter)
		{
			TargetCharacter->HandleMaxManaChanged(NewValue, NewValue - CurrentValue);
		}
		AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetMaxStaminaAttribute());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
		if (TargetCharacter)
		{
			TargetCharacter->HandleStaminaChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		if (TargetCharacter)
		{
			TargetCharacter->HandleMaxStaminaChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleMoveSpeedChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetSwimSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleSwimSpeedChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetRideSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleRideSpeedChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetFlySpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleFlySpeedChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetRotationSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleRotationSpeedChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetJumpForceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleJumpForceChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetDodgeForceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleDodgeForceChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetAttackForceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleAttackForceChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetRepulseForceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleRepulseForceChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetAttackSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleAttackSpeedChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetAttackCritRateAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.f);
		if (TargetCharacter)
		{
			TargetCharacter->HandleAttackCritRateChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetAttackStealRateAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.f);
		if (TargetCharacter)
		{
			TargetCharacter->HandleAttackStealRateChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetDefendRateAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.f);
		if (TargetCharacter)
		{
			TargetCharacter->HandleDefendRateChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetDefendScopeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.f);
		if (TargetCharacter)
		{
			TargetCharacter->HandleDefendScopeChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetPhysicsDefRateAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.f);
		if (TargetCharacter)
		{
			TargetCharacter->HandlePhysicsDefRateChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetMagicDefRateAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.f);
		if (TargetCharacter)
		{
			TargetCharacter->HandleMagicDefRateChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetToughnessRateAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.f);
		if (TargetCharacter)
		{
			TargetCharacter->HandleToughnessRateChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetStaminaRegenSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleRegenSpeedAttribute(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetStaminaExpendSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
		if (TargetCharacter)
		{
			TargetCharacter->HandleExpendSpeedAttribute(NewValue, NewValue - CurrentValue);
		}
	}
	else
	{
		Super::PreAttributeChange(Attribute, NewValue);
	}
}

void UDWCharacterAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	AActor* TargetActor = nullptr;
	ADWCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetCharacter = Cast<ADWCharacter>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetInterruptAttribute())
	{
		if (TargetCharacter)
		{
			TargetCharacter->HandleInterrupt(GetInterrupt());
		}
		SetInterrupt(0.f);
	}
	else
	{
		Super::PostGameplayEffectExecute(Data);
	}
}

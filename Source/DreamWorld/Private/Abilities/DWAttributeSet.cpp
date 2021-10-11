#include "Abilities/DWAttributeSet.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"
#include "Vitality/Vitality.h"
#include "GameplayEffectExtension.h"
#include "GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "Character/DWCharacter.h"

UDWAttributeSet::UDWAttributeSet()
:	Health(100.f),
	MaxHealth(100.f)
{
	
}

void UDWAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	IVitality* TargetVitality = Cast<IVitality>(AbilityComp->GetAvatarActor());
	
	const float CurrentValue = Attribute.GetGameplayAttributeData(this)->GetCurrentValue();
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
		if (TargetVitality)
		{
			TargetVitality->HandleHealthChanged(NewValue, NewValue - CurrentValue);
		}
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		if (TargetVitality)
		{
			TargetVitality->HandleMaxHealthChanged(NewValue, NewValue - CurrentValue);
		}
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
}

void UDWAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	AActor* TargetActor = nullptr;
	IVitality* TargetVitality = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetVitality = Cast<IVitality>(TargetActor);
	}
	
	if (Data.EvaluatedData.Attribute.GetName().EndsWith("Damage"))
	{
		float AttackForce = 0.f;
		float AttackCritRate = 0.f;
		float DefendRate = 0.f;
		float DefendScope = 0.f;
		float PhysicsDefRate = 0.f;
		float MagicDefRate = 0.f;

		float LocalDamageDone = 0.f;
		float DefendRateDone = 0.f;
		
		AActor* SourceActor = nullptr;
		ADWCharacter* SourceCharacter = nullptr;
		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceCharacter = Cast<ADWCharacter>(SourceActor);
			if(SourceCharacter)
			{
				AttackForce = SourceCharacter->GetAttackForce();
				AttackCritRate = SourceCharacter->GetAttackCritRate();
			}
		}
		ADWCharacter* TargetCharacter = Cast<ADWCharacter>(TargetActor);
		if (TargetCharacter)
		{
			DefendRate = TargetCharacter->GetDefendRate();
			DefendScope = TargetCharacter->GetDefendScope();
			PhysicsDefRate = TargetCharacter->GetPhysicsDefRate();
			MagicDefRate = TargetCharacter->GetMagicDefRate();

			FVector DamageDirection = SourceActor->GetActorLocation() - TargetActor->GetActorLocation();
			if (FVector::DotProduct(DamageDirection, TargetActor->GetActorForwardVector()) / 90 > (1 - DefendScope))
			{
				DefendRateDone = DefendRate * (TargetCharacter->IsDefending() ? 1 : 0);
				if(DefendRateDone > 0.f && !TargetCharacter->DoAction(ECharacterActionType::DefendBlock))
				{
					DefendRateDone = 0.f;
				}
			}
		}

		if(Data.EvaluatedData.Attribute.GetName().StartsWith("Physics"))
		{
			LocalDamageDone = AttackForce * GetPhysicsDamage() * (1 - PhysicsDefRate) * (1 - DefendRateDone) * (FMath::FRand() <= AttackCritRate ? 2 : 1);
			if(SourceCharacter)
			{
				SourceCharacter->DoAction(LocalDamageDone > 0.f ? ECharacterActionType::AttackHit : ECharacterActionType::AttackMiss);
			}
			SetPhysicsDamage(0.f);
		}
		else if(Data.EvaluatedData.Attribute.GetName().StartsWith("Magic"))
		{
			LocalDamageDone = GetMagicDamage() * (1 - MagicDefRate) * (1 - DefendRateDone) * (FMath::FRand() <= AttackCritRate ? 2 : 1);
			SetMagicDamage(0.f);
		}

		if (LocalDamageDone > 0.f)
		{
			if (TargetVitality && !TargetVitality->IsDead())
			{
				TargetVitality->ModifyHealth(-LocalDamageDone);

				FHitResult HitResult;
				if (Context.GetHitResult())
				{
					HitResult = *Context.GetHitResult();
				}
				TargetVitality->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor);

				if(TargetCharacter && DefendRateDone == 0.f)
				{
					TargetCharacter->DoAction(ECharacterActionType::GetHit);
				}
			}
		}
	}
}

void UDWAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = CurrentMaxValue > 0.f ? (CurrentValue / CurrentMaxValue * NewMaxValue - CurrentValue) : NewMaxValue;
		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

FGameplayAttributeData* UDWAttributeSet::GetAttributeData(const FGameplayAttribute& Attribute)
{
	return Attribute.GetGameplayAttributeData(this);
}

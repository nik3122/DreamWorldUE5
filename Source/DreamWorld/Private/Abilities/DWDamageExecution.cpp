#include "Abilities/DWDamageExecution.h"
#include "Abilities/DWAbilitySystemComponent.h"
#include "Abilities/DWAttributeSet.h"
#include "Abilities/Character/DWCharacterAttributeSet.h"
#include "Character/DWCharacter.h"

struct DWDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackForce);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackCritRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicsDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicDamage);

	DECLARE_ATTRIBUTE_CAPTUREDEF(DefendRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefendScope);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicsDefRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicDefRate);

	DWDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDWAttributeSet, PhysicsDamage, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDWAttributeSet, MagicDamage, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDWCharacterAttributeSet, AttackForce, Target, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDWCharacterAttributeSet, AttackCritRate, Target, true);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UDWCharacterAttributeSet, DefendRate, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDWCharacterAttributeSet, DefendScope, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDWCharacterAttributeSet, PhysicsDefRate, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDWCharacterAttributeSet, MagicDefRate, Target, false);
	}
};

static const DWDamageStatics& DamageStatics()
{
	static DWDamageStatics DmgStatics;
	return DmgStatics;
}

UDWDamageExecution::UDWDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackForceDef);
	RelevantAttributesToCapture.Add(DamageStatics().AttackCritRateDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicsDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicDamageDef);

	RelevantAttributesToCapture.Add(DamageStatics().DefendRateDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefendScopeDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicsDefRateDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicDefRateDef);
}

void UDWDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UDWAbilitySystemComponent* TargetAbilitySystemComp = Cast<UDWAbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	UDWAbilitySystemComponent* SourceAbilitySystemComp = Cast<UDWAbilitySystemComponent>(ExecutionParams.GetSourceAbilitySystemComponent());

	AActor* TargetActor = TargetAbilitySystemComp ? TargetAbilitySystemComp->GetAvatarActor() : nullptr;
	AActor* SourceActor = SourceAbilitySystemComp ? SourceAbilitySystemComp->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.TargetTags = TargetTags;
	EvaluateParameters.SourceTags = SourceTags;

	float AttackForce = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackForceDef, EvaluateParameters, AttackForce);
	float AttackCritRate = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackCritRateDef, EvaluateParameters, AttackCritRate);
	float PhysicsDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicsDamageDef, EvaluateParameters, PhysicsDamage);
	float MagicDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicDamageDef, EvaluateParameters, MagicDamage);

	float DefendRate = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefendRateDef, EvaluateParameters, DefendRate);
	float DefendScope = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefendScopeDef, EvaluateParameters, DefendScope);
	float PhysicsDefRate = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicsDefRateDef, EvaluateParameters, PhysicsDefRate);
	float MagicDefRate = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicDefRateDef, EvaluateParameters, MagicDefRate);

	float DefendRateDone = 0.f;
	ADWCharacter* SourceCharacter = Cast<ADWCharacter>(SourceActor);
	if (SourceCharacter)
	{
		AttackForce = SourceCharacter->GetAttackForce();
		AttackCritRate = SourceCharacter->GetAttackCritRate();
	}

	ADWCharacter* TargetCharacter = Cast<ADWCharacter>(TargetActor);
	if (TargetCharacter)
	{
		DefendRate = TargetCharacter->GetDefendRate();
		DefendScope = TargetCharacter->GetDefendScope();
		PhysicsDefRate = TargetCharacter->GetPhysicsDefRate();
		MagicDefRate = TargetCharacter->GetMagicDefRate();
	}

	IVitality* TargetVitality = Cast<ADWCharacter>(TargetActor);
	if (TargetVitality)
	{
		PhysicsDamage = TargetVitality->GetPhysicsDamage();
		MagicDamage = TargetVitality->GetMagicDamage();
	}

	const FVector DamageDirection = SourceActor->GetActorLocation() - TargetActor->GetActorLocation();
	if (FVector::DotProduct(DamageDirection, -SourceActor->GetActorForwardVector()) > (1 - DefendScope))
	{
		DefendRateDone = DefendRate * (TargetCharacter->IsDefending() ? 1 : 0);
	}

	const float PhysicsDamageDone = AttackForce * PhysicsDamage * (1 - PhysicsDefRate) * (1 - DefendRateDone) * (FMath::FRand() <= AttackCritRate ? 2 : 1);
	if (PhysicsDamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().PhysicsDamageProperty, EGameplayModOp::Override, PhysicsDamageDone));
	}

	const float MagicDamageDone = MagicDamage * (1 - MagicDefRate) * (1 - DefendRateDone) * (FMath::FRand() <= AttackCritRate ? 2 : 1);
	if (MagicDamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().MagicDamageProperty, EGameplayModOp::Override, MagicDamageDone));
	}
}

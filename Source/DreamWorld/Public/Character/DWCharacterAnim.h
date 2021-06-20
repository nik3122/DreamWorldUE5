// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Animation/AnimInstance.h"
#include "DWCharacterAnim.generated.h"

class ADWCharacter;

/**
 * ½ÇÉ«¶¯»­¿ØÖÆÆ÷
 */
UCLASS()
class DREAMWORLD_API UDWCharacterAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UDWCharacterAnim();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bDefending;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bFlying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bRiding;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bClimbing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bCrouching;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStates")
	bool bSwimming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float MoveDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float HorizontalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStats")
	float VerticalSpeed;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	ADWCharacter* OwnerCharacter;

protected:
	virtual void NativeInitializeAnimation() override;

	virtual bool HandleNotify(const FAnimNotifyEvent& AnimNotifyEvent) override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	virtual void UpdateAnimParams(float DeltaSeconds);
};

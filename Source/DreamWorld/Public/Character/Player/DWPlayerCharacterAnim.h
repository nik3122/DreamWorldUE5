// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "Character/Human/DWHumanCharacterAnim.h"
#include "DWPlayerCharacterAnim.generated.h"

/**
 * 玩家角色动画控制器
 */
UCLASS()
class DREAMWORLD_API UDWPlayerCharacterAnim : public UDWHumanCharacterAnim
{
	GENERATED_BODY()
	
public:
	UDWPlayerCharacterAnim();

protected:
	virtual void UpdateAnimParams(float DeltaSeconds) override;
};

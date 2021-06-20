// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DreamWorld.h"
#include "AI/DWAIBlackboard.h"
#include "DWAIHumanBlackboard.generated.h"

/**
 * 人类黑板基类
 */
UCLASS()
class DREAMWORLD_API UDWAIHumanBlackboard : public UDWAIBlackboard
{
	GENERATED_BODY()
	
public:
	UDWAIHumanBlackboard();
};

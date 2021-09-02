// Fill out your copyright notice in the Description page of Project Settings.


#include "Vitality/VitalityObject.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/DWCharacter.h"
#include "WidgetComponent.h"
#include "Widget/Worlds/WidgetWorldText.h"
#include "Widget/Worlds/WidgetVitalityHP.h"
#include "Widget/Components/WidgetVitalityHPComponent.h"
#include "Widget/Components/WidgetWorldTextComponent.h"
#include "World/Chunk.h"
#include "Inventory/Inventory.h"
#include "Abilities/DWAbilitySystemComponent.h"
#include "Abilities/DWAttributeSet.h"
#include "Abilities/DWGameplayAbility.h"

// Sets default values
AVitalityObject::AVitalityObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetCollisionProfileName(TEXT("DW_Vitality"));
	BoxComponent->SetBoxExtent(FVector(20, 20, 20));
	SetRootComponent(BoxComponent);

	WidgetVitalityHP = CreateDefaultSubobject<UWidgetVitalityHPComponent>(TEXT("WidgetVitalityHP"));
	WidgetVitalityHP->SetupAttachment(RootComponent);
	WidgetVitalityHP->SetWidgetClass(UDWHelper::LoadWidgetVitalityHPClass());
	WidgetVitalityHP->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetVitalityHP->SetDrawSize(FVector2D(220, 60));
	WidgetVitalityHP->SetPivot(FVector2D(0.5f, 1));
	WidgetVitalityHP->SetRelativeLocation(FVector(0, 0, 50));
	WidgetVitalityHP->SetVisibility(false);
	
	AbilitySystem = CreateDefaultSubobject<UDWAbilitySystemComponent>(TEXT("AbilitySystem"));

	AttributeSet = CreateDefaultSubobject<UDWAttributeSet>(TEXT("AttributeSet"));

	// states
	bDead = true;

	// stats
	Name = TEXT("");
	RaceID = TEXT("");
	Level = 0;
	EXP = 50;
	BaseEXP = 100;
	EXPFactor = 2.f;

	InventoryData = FInventoryData();

	OwnerChunk = nullptr;
	Inventory = nullptr;
}

// Called when the game starts or when spawned
void AVitalityObject::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystem->InitAbilityActorInfo(this, this);

	if (GetWidgetVitalityHPWidget() && !GetWidgetVitalityHPWidget()->GetOwnerObject())
	{
		GetWidgetVitalityHPWidget()->SetOwnerObject(this);
	}

	Revive();
}

// Called every frame
void AVitalityObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVitalityObject::LoadData(FVitalityObjectData InSaveData)
{
	if (!Inventory) Inventory = NewObject<UInventory>(this);

	if (InSaveData.bInitialized)
	{
		SetName(InSaveData.Name);
		SetRaceID(InSaveData.RaceID);
		SetLevelC(InSaveData.Level);
		SetEXP(InSaveData.EXP);

		//AttributeSet = InSaveData.AttributeSet;
		Inventory->LoadData(InSaveData.InventoryData, this);

		SetActorLocation(InSaveData.Location);
		SetActorRotation(InSaveData.Rotation);
	}
	else
	{
		SetRaceID(InSaveData.RaceID);

		const auto ItemDatas = UDWHelper::LoadItemDatas();
		if(FMath::FRandRange(0, 1) < 0.2f)
		{
			InventoryData.Items.Add(FItem(ItemDatas[FMath::RandRange(0, ItemDatas.Num() - 1)].ID));
		}
		Inventory->LoadData(InventoryData, this);

		SetActorLocation(InSaveData.Location);
		SetActorRotation(InSaveData.Rotation);
	}
}

FVitalityObjectData AVitalityObject::ToData()
{
	auto SaveData = FVitalityObjectData();

	SaveData.bInitialized = true;

	SaveData.Name = Name;
	SaveData.RaceID = RaceID;
	SaveData.Level = Level;
	SaveData.EXP = EXP;
	
	SaveData.AttributeSet = AttributeSet;
	SaveData.InventoryData = Inventory->ToData();

	SaveData.Location = GetActorLocation();
	SaveData.Rotation = GetActorRotation();

	SaveData.Class = GetClass();

	return SaveData;
}

void AVitalityObject::Death(ADWCharacter* InKiller /*= nullptr*/)
{
	if (!bDead)
	{
		bDead = true;
		if(InKiller != nullptr)
		{
			InKiller->ModifyEXP(GetTotalEXP());
		}
		SetEXP(0);
		SetHealth(0.f);
		Inventory->DiscardAll();
		OwnerChunk->DestroyVitalityObject(this);
	}
}

void AVitalityObject::Revive()
{
	if (bDead)
	{
		bDead = false;
		SetHealth(GetMaxHealth());
	}
}

void AVitalityObject::Refresh()
{
	HandleHealthChanged(GetHealth());
	HandleNameChanged(GetName());
	HandleRaceIDChanged(GetRaceID());
}

bool AVitalityObject::IsDead() const
{
	return bDead;
}

void AVitalityObject::SetName(const FString& InName)
{
	Name = InName;
	HandleNameChanged(InName);
}

void AVitalityObject::SetRaceID(const FString& InRaceID)
{
	RaceID = InRaceID;
	HandleRaceIDChanged(InRaceID);
}

void AVitalityObject::SetLevelC(int32 InLevel)
{
	Level = InLevel;
	HandleLevelChanged(InLevel);
}

void AVitalityObject::SetEXP(int32 InEXP)
{
	EXP = InEXP;
	HandleEXPChanged(InEXP);
}

int32 AVitalityObject::GetMaxEXP() const
{
	int32 MaxEXP = BaseEXP;
	for (int i = 0; i < Level - 1; i++)
	{
		MaxEXP *= EXPFactor;
	}
	return MaxEXP;
}

int32 AVitalityObject::GetTotalEXP() const
{
	int32 TotalEXP = BaseEXP;
	for (int i = 0; i < Level - 1; i++)
	{
		TotalEXP += TotalEXP * EXPFactor;
	}
	return EXP + TotalEXP - GetMaxEXP();
}

FString AVitalityObject::GetHeadInfo() const
{
	return FString::Printf(TEXT("Lv.%d \"%s\" "), Level, *Name);
}

float AVitalityObject::GetHealth() const
{
	return AttributeSet->GetHealth();
}

void AVitalityObject::SetHealth(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetHealthAttribute(), EGameplayModOp::Override, InValue);
}

float AVitalityObject::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

void AVitalityObject::SetMaxHealth(float InValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetMaxHealthAttribute(), EGameplayModOp::Override, InValue);
}

float AVitalityObject::GetPhysicsDamage() const
{
	return AttributeSet->GetPhysicsDamage();
}

float AVitalityObject::GetMagicDamage() const
{
	return AttributeSet->GetMagicDamage();
}

void AVitalityObject::SpawnWidgetWorldText(EWorldTextType InContextType, FString InContext)
{
	auto contextHUD = NewObject<UWidgetWorldTextComponent>(this);
	contextHUD->RegisterComponent();
	contextHUD->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	contextHUD->SetRelativeLocation(FVector(0, 0, 50));
	contextHUD->SetVisibility(false);
	if (contextHUD && contextHUD->GetUserWidgetObject())
	{
		auto tmpWidget = Cast<UWidgetWorldText>(contextHUD->GetUserWidgetObject());
		if (tmpWidget)
		{
			tmpWidget->InitWidgetWorldText(InContextType, InContext);
		}
	}
}

FGameplayAbilitySpecHandle AVitalityObject::AcquireAbility(TSubclassOf<UDWGameplayAbility> InAbility, int32 InLevel /*= 1*/)
{
	if (AbilitySystem && InAbility)
	{
		FGameplayAbilitySpecDef SpecDef = FGameplayAbilitySpecDef();
		SpecDef.Ability = InAbility;
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SpecDef, InLevel);
		return AbilitySystem->GiveAbility(AbilitySpec);
	}
	return FGameplayAbilitySpecHandle();
}

bool AVitalityObject::ActiveAbility(FGameplayAbilitySpecHandle SpecHandle, bool bAllowRemoteActivation /*= false*/)
{
	if (AbilitySystem)
	{
		return AbilitySystem->TryActivateAbility(SpecHandle, bAllowRemoteActivation);
	}
	return false;
}

bool AVitalityObject::ActiveAbility(TSubclassOf<UDWGameplayAbility> AbilityClass, bool bAllowRemoteActivation /*= false*/)
{
	if (AbilitySystem)
	{
		return AbilitySystem->TryActivateAbilityByClass(AbilityClass, bAllowRemoteActivation);
	}
	return false;
}

bool AVitalityObject::ActiveAbility(const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation /*= false*/)
{
	if (AbilitySystem)
	{
		return AbilitySystem->TryActivateAbilitiesByTag(GameplayTagContainer, bAllowRemoteActivation);
	}
	return false;
}

void AVitalityObject::CancelAbility(UDWGameplayAbility* Ability)
{
	if (AbilitySystem)
	{
		AbilitySystem->CancelAbility(Ability);
	}
}

void AVitalityObject::CancelAbility(const FGameplayAbilitySpecHandle& AbilityHandle)
{
	if (AbilitySystem)
	{
		AbilitySystem->CancelAbilityHandle(AbilityHandle);
	}
}

void AVitalityObject::CancelAbilities(const FGameplayTagContainer* WithTags/*=nullptr*/, const FGameplayTagContainer* WithoutTags/*=nullptr*/, UDWGameplayAbility* Ignore/*=nullptr*/)
{
	if (AbilitySystem)
	{
		AbilitySystem->CancelAbilities(WithTags, WithoutTags, Ignore);
	}
}

void AVitalityObject::CancelAllAbilities(UDWGameplayAbility* Ignore/*=nullptr*/)
{
	if (AbilitySystem)
	{
		AbilitySystem->CancelAllAbilities(Ignore);
	}
}

void AVitalityObject::ModifyHealth(float InDetlaValue)
{
	AbilitySystem->ApplyModToAttributeUnsafe(AttributeSet->GetHealthAttribute(), EGameplayModOp::Additive, InDetlaValue);
}

void AVitalityObject::ModifyEXP(float InDetlaValue)
{
	const int32 MaxEXP = GetMaxEXP();
	EXP += InDetlaValue;
	if (InDetlaValue > 0.f)
	{
		if (EXP >= MaxEXP)
		{
			Level++;
			EXP -= MaxEXP;
		}
	}
	else
	{
		if (EXP < 0.f)
		{
			EXP = 0.f;
		}
	}
	HandleEXPChanged(EXP);
}

UWidgetVitalityHP* AVitalityObject::GetWidgetVitalityHPWidget()
{
	if (WidgetVitalityHP->GetUserWidgetObject())
	{
		return Cast<UWidgetVitalityHP>(WidgetVitalityHP->GetUserWidgetObject());
	}
	return nullptr;
}

void AVitalityObject::HandleDamage(const float LocalDamageDone, FHitResult HitResult, const struct FGameplayTagContainer& SourceTags, ADWCharacter* SourceCharacter, AActor* SourceActor)
{
	if (GetHealth() <= 0.f)
	{
		Death(SourceCharacter);
	}
	if(SourceCharacter)
	{
		SourceCharacter->ModifyHealth(LocalDamageDone * SourceCharacter->GetAttackStealRate());
	}
}

void AVitalityObject::HandleNameChanged(const FString& NewValue)
{
	if (GetWidgetVitalityHPWidget())
	{
		GetWidgetVitalityHPWidget()->SetHeadInfo(GetHeadInfo());
	}
}

void AVitalityObject::HandleRaceIDChanged(const FString& NewValue)
{
	if (GetWidgetVitalityHPWidget())
	{
		GetWidgetVitalityHPWidget()->SetHeadInfo(GetHeadInfo());
	}
}

void AVitalityObject::HandleLevelChanged(int32 NewValue, int32 DeltaValue /*= 0*/)
{
	if (GetWidgetVitalityHPWidget())
	{
		GetWidgetVitalityHPWidget()->SetHeadInfo(GetHeadInfo());
	}
}

void AVitalityObject::HandleEXPChanged(int32 NewValue, int32 DeltaValue /*= 0*/)
{
	if (GetWidgetVitalityHPWidget())
	{
		GetWidgetVitalityHPWidget()->SetHeadInfo(GetHeadInfo());
	}
}

void AVitalityObject::HandleHealthChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	if (!FMath::IsNearlyZero(DeltaValue))
	{
		SpawnWidgetWorldText(DeltaValue > 0.f ? EWorldTextType::Recover : EWorldTextType::DamageOther, FString::FromInt(FMath::Abs(DeltaValue)));
	}
	if (GetWidgetVitalityHPWidget())
	{
		GetWidgetVitalityHPWidget()->SetHealthPercent(NewValue, GetMaxHealth());
	}
}

void AVitalityObject::HandleMaxHealthChanged(float NewValue, float DeltaValue /*= 0.f*/)
{
	if (GetWidgetVitalityHPWidget())
	{
		GetWidgetVitalityHPWidget()->SetHealthPercent(NewValue, GetMaxHealth());
	}
}

#pragma once

#include "Engine/DataTable.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffectTypes.h"
#include "DWTypes.generated.h"

class AWorldManager;
class ADWPlayerCharacter;
class ADWGameMode;
class UParticleSystem;
class USoundBase;
class UAnimMontage;
class UGameUI;
class AChunk;
class UVoxel;
class UMaterialInterface;
class UTexture2D;
class ADWCharacter;
class UWidgetCharacterHP;
class UWidgetVitalityHP;
class UWidgetWorldText;
class AVitalityObject;
class AVoxelAuxiliary;
class ADWPlayerCharacterController;
class UWidgetInventorySlot;
class UDWGameInstance;
class UInventory;
class UCharacterInventory;
class ADWGameState;
class AEquip;
class ASkill;
class UPropEffectBase;
class UEquipEffectBase;
class UDWGameplayAbility;
class UDWAttributeSet;
class UDWCharacterAttributeSet;
class UDWCharacterAttackAbility;
class UDWCharacterActionAbility;
class UDWCharacterSkillAbility;
class UDWTargetType;
class UInventorySlot;

#define Vector_Empty FVector(MAX_flt)

/**
 * 游戏状态
 */
UENUM(BlueprintType)
enum class EGameState : uint8
{
	// ??
	None,
	// ?????
	Initializing,
	// ?????
	MainMenu,
	// ?????
	ChoosingRole,
	// ???????
	ChoosingWorld,
	// ??????
	Loading,
	// ??????
	Playing,
	// ?????
	Pausing,
	// ?????
	Overed
};

/**
 * ??????
 */
UENUM(BlueprintType)
enum class EInputMode : uint8
{
	None,
	UIOnly,
	GameAndUI,
	GameOnly
};

/**
 * UI???????
 */
UENUM(BlueprintType)
enum class EWidgetPanelType : uint8
{
	// ???
	Temporary,
	// ???
	Permanent
};

/**
 * ????
 */
UENUM(BlueprintType)
enum class EMouseButton : uint8
{
	Left,
	Middle,
	Right
};

/**
 * ??????
 */
UENUM(BlueprintType)
enum class EControlMode : uint8
{
	// ???
	Fighting,
	// ????
	Creating
};

/**
 * ???????
 */
UENUM(BlueprintType)
enum class ECharacterNature : uint8
{
	// ???
	Player,
	// ???????
	NPC,
	// ????AI
	AIFriendly,
	// ??????AI
	AINeutral,
	// ?ж??AI
	AIHostile
};

/**
 * ???????????
 */
UENUM(BlueprintType)
enum class ECharacterInteract : uint8
{
	// ???
	Fighting,
	// ???
	Damage,
	// ???
	Dialogue,
	// ????
	Transaction
};

/**
 * ???????
 */
UENUM(BlueprintType)
enum class EItemType : uint8
{
	// ??
	None,
	// ????
	Voxel,
	// ????
	Prop,
	// ???
	Equip,
	// ????
	Skill
};

/**
 * ????????
 */
UENUM(BlueprintType)
enum class EVoxelType : uint8
{
	Empty, //??
	Bedrock, //????
	Dirt, //????
	Stone, //??
	Sand, //???
	Grass, //???
	Snow, //???
	Oak, //???
	Birch, //?????
	Oak_Plank, //?????
	Birch_Plank, //???????
	Cobble_Stone, //??
	Stone_Brick, //??
	Red_Brick, //???
	Sand_Stone, //??
	Oak_Leaves, //?????
	Birch_Leaves, //???????
	Ice, //????
	Glass, //????
	Oak_Door, //?????
	Birch_Door, //???????
	Torch, //???
	Water, // ?
	Tall_Grass, //?????
	Flower_Allium, 
	Flower_Blue_Orchid,
	Flower_Dandelion,
	Flower_Houstonia,
	Flower_Oxeye_Daisy,
	Flower_Paeonia,
	Flower_Rose,
	Flower_Tulip_Orange,
	Flower_Tulip_Pink,
	Flower_Tulip_Red,
	Flower_Tulip_White,
	Unknown //δ?
};

UENUM(BlueprintType)
enum class EPropType : uint8
{
	// ??
	Potion,
	// ???
	Food,
	// ????
	Container
};

UENUM(BlueprintType)
enum class EEquipType : uint8
{
	// ????
	Weapon,
	// ????
	Shield,
	// ????
	Armor
};

UENUM(BlueprintType)
enum class EEquipPartType : uint8
{
	// ???
	Head,
	// ???
	Chest,
	// ???
	Arm,
	// ???
	Leg,
	// ????
	LeftHand,
	// ????
	RightHand
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	// 无
	None,
	// 物理近战
	PhysicsMelee,
	// 物理远程
	PhysicsRemote,
	// 魔法近战
	MagicMelee,
	// 魔法远程
	MagicRemote,
};

UENUM(BlueprintType)
enum class EShieldType : uint8
{
	// 无
	None,
	// 物理
	Physics,
	// 魔法
	Magic
};

UENUM(BlueprintType)
enum class EWeaponHandType : uint8
{
	// ????
	Single,
	// ???
	Both
};

/**
 * ????
 */
UENUM(BlueprintType)
enum class EFacing : uint8
{
	Up,
	Down,
	Forward,
	Back,
	Left,
	Right
};

/**
 * ????
 */
UENUM(BlueprintType)
enum class EDirection : uint8
{
	Up,
	Down,
	Forward,
	Back,
	Left,
	Right
};

/**
 * ?????
 */
UENUM(BlueprintType)
enum class ETransparency : uint8
{
	// ???
	Solid,
	// ?????
	SemiTransparent,
	// ???
	Transparent
};

/**
 * ????????????
 */
UENUM(BlueprintType)
enum class EVoxelMeshType : uint8
{
	// ?????
	Chunk,
	// ??????
	PickUp,
	// ?????
	PreviewItem,
	// ????????
	VitalityVoxel
};

/**
 * ???????
 */
UENUM(BlueprintType)
enum class EDWDamageType : uint8
{
	// ????
	Any,
	// ????
	Physics,
	// ???
	Magic
};

/**
 * ?????????????
 */
UENUM(BlueprintType)
enum class EInventoryActionType : uint8
{
	// ???
	Addition,
	// ???
	Remove,
	// ???
	Clear
};

/**
 * ???????????
 */
UENUM(BlueprintType)
enum class EGameTraceType : uint8
{
	// ??
	None = 0,
	// ?????
	Chunk = (int32)ECC_GameTraceChannel6,
	// ????
	Voxel = (int32)ECC_GameTraceChannel7,
	// ????
	Sight = (int32)ECC_GameTraceChannel8,
	// ???
	Step = (int32)ECC_GameTraceChannel9
};

/**
 * ???????????
 */
UENUM(BlueprintType)
enum class EWorldTextType : uint8
{
	// ??????
	DamagePlayer,
	// ???????
	DamageOther,
	// ???
	Recover
};

/**
 * ????????
 */
UENUM(BlueprintType)
enum class EAttackType : uint8
{
	// ??
	None,
	// ???????
	NormalAttack,
	// ??乥??
	FallingAttack,
	// ??乥??
	RemoteAttack,
	// ???????
	SkillAttack
};

/**
* 技能类型
*/
UENUM(BlueprintType)
enum class ESkillType : uint8
{
	// 无
	None,
	// 近战
	Melee,
	// ???????
	Remote
};

/**
* 技能模式
*/
UENUM(BlueprintType)
enum class ESkillMode : uint8
{
	// 无
	None,
	// 被动
	Passive,
	// ???????
	Initiative
};

/**
 * ???AI??
 */
UENUM(BlueprintType)
enum class ECharacterAIState : uint8
{
	// ??
	None,
	// AI???
	AIPatrol,
	// AI????
	AIFollow,
	// AI???
	AITrack,
	// AI????
	AIAttack,
	// AI????
	AIDefend
};

/**
 * ??????????
 */
UENUM(BlueprintType)
enum class ECharacterActionType : uint8
{
	// ??
	None,
	// ????
	Death,
	// ????
	Revive,
	// ???
	Jump,
	// ???
	Crouch,
	// ????
	Dodge,
	// ????
	Climb,
	// ???
	Swim,
	// ???
	Ride,
	// ????
	Fly,
	// ???
	Take,
	// ???
	Use,
	// ????
	Discard,
	// ????
	Generate,
	// ????
	Destroy,
	// ????????
	AttackHit,
	// ???????
	AttackMiss,
	// ???????
	DefendBlock,
	// ???
	Interrupt
};

///**
// * ???????Effect????
// */
//UENUM(BlueprintType)
//enum class ECharacterPassiveEffectType : uint8
//{
//	
//};

/**
 * ???????????
 */
UENUM(BlueprintType)
enum class EAbilityCostType : uint8
{
	// ??
	None,
	// ?????
	Health,
	// ????
	Mana,
	// ?????
	Stamina
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FIndex
{
	GENERATED_BODY()

public:
	int X, Y, Z;

	static const FIndex ZeroIndex;

	static const FIndex OneIndex;

	FORCEINLINE FIndex()
	{
		X = 0;
		Y = 0;
		Z = 0;
	}

	FORCEINLINE FIndex(int InX, int InY, int InZ)
	{
		X = InX;
		Y = InY;
		Z = InZ;
	}

	FORCEINLINE FIndex(FVector InVector)
	{
		X = FMath::CeilToInt(InVector.X);
		Y = FMath::CeilToInt(InVector.Y);
		Z = FMath::CeilToInt(InVector.Z);
	}

	FORCEINLINE FIndex(const FString& InValue)
	{
		TArray<FString> tmpArr;
		InValue.ParseIntoArray(tmpArr, TEXT(","));
		X = FCString::Atoi(*tmpArr[0]);
		Y = FCString::Atoi(*tmpArr[1]);
		Z = FCString::Atoi(*tmpArr[2]);
	}

	FORCEINLINE FVector ToVector()
	{
		return FVector(X, Y, Z);
	}

	FORCEINLINE FString ToString()
	{
		return FString::Printf(TEXT("%d,%d,%d"), X, Y, Z);
	}

	FORCEINLINE static float Distance(FIndex A, FIndex B, bool bIgnoreZ = false)
	{
		if (bIgnoreZ) A.Z = B.Z = 0;
		return FVector::Distance(A.ToVector(), B.ToVector());
	}

	FORCEINLINE friend bool operator==(const FIndex& A, const FIndex& B)
	{
		return (A.X == B.X) && (A.Y == B.Y) && (A.Z == B.Z);
	}

	FORCEINLINE friend bool operator!=(const FIndex& A, const FIndex& B)
	{
		return (A.X != B.X) || (A.Y != B.Y) || (A.Z != B.Z);
	}

	FORCEINLINE FIndex operator+(const FIndex& InIndex) const
	{
		return FIndex(X + InIndex.X, Y + InIndex.Y, Z + InIndex.Z);
	}

	FORCEINLINE FIndex operator-(const FIndex& InIndex) const
	{
		return FIndex(X - InIndex.X, Y - InIndex.Y, Z - InIndex.Z);
	}

	FORCEINLINE FIndex operator*(const FIndex& InIndex) const
	{
		return FIndex(X * InIndex.X, Y * InIndex.Y, Z * InIndex.Z);
	}

	FORCEINLINE FIndex operator/(const FIndex& InIndex) const
	{
		return FIndex(X / InIndex.X, Y / InIndex.Y, Z / InIndex.Z);
	}

	FORCEINLINE FIndex operator*(const int& InValue) const
	{
		return FIndex(X * InValue, Y * InValue, Z * InValue);
	}

	FORCEINLINE FIndex operator/(const int& InValue) const
	{
		return FIndex(X / InValue, Y / InValue, Z / InValue);
	}

	friend void operator<<(FStructuredArchive::FSlot Slot, FIndex& InValue)
	{
		FStructuredArchive::FRecord Record = Slot.EnterRecord();
		Record << SA_VALUE(TEXT("X"), InValue.X);
		Record << SA_VALUE(TEXT("Y"), InValue.Y);
		Record << SA_VALUE(TEXT("Z"), InValue.Z);
	}
};

FORCEINLINE uint32 GetTypeHash(const FIndex& InIndex)
{
	return FCrc::MemCrc_DEPRECATED(&InIndex, sizeof(InIndex));
}

/**
 * ????
 */
USTRUCT(BlueprintType)
struct DREAMWORLD_API FRaceItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Range;

	FORCEINLINE FRaceItem()
	{
		Range = FVector(1, 1, 1);
	}
};

/**
 * ??????????????
 */
USTRUCT(BlueprintType)
struct DREAMWORLD_API FVitalityRaceItem : public FRaceItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AVitalityObject> Class;

	FORCEINLINE FVitalityRaceItem()
	{
		Class = nullptr;
	}
};

/**
 * ???????
 */
USTRUCT(BlueprintType)
struct DREAMWORLD_API FCharacterRaceItem : public FRaceItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ADWCharacter> Class;

	FORCEINLINE FCharacterRaceItem()
	{
		Class = nullptr;
	}
};

/**
 * ???????
 */
USTRUCT(BlueprintType)
struct DREAMWORLD_API FRaceData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FName ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Detail;
				
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Proportion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxNum;

	FORCEINLINE FRaceData()
	{
		ID = NAME_None;
		Name = NAME_None;
		Detail = TEXT("");
		Proportion = 1;
		MinNum = 0;
		MaxNum = 1;
	}

	FORCEINLINE bool IsValid()
	{
		return !ID.IsNone();
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FVitalityRaceData : public FRaceData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVitalityRaceItem> RaceItems;

	FORCEINLINE FVitalityRaceData()
	{
		RaceItems = TArray<FVitalityRaceItem>();
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FCharacterRaceData : public FRaceData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCharacterRaceItem> RaceItems;

	FORCEINLINE FCharacterRaceData()
	{
		RaceItems = TArray<FCharacterRaceItem>();
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FTeamData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FName ID;

	UPROPERTY(BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Detail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ADWCharacter* Captain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ADWCharacter*> Members;
	
	static FTeamData Empty;

	FORCEINLINE FTeamData()
	{
		ID = NAME_None;
		Name = NAME_None;
		Detail = TEXT("");
		Captain = nullptr;
		Members = TArray<ADWCharacter*>();
	}

	void AddMember(ADWCharacter* InMember);

	void RemoveMember(ADWCharacter* InMember);

	void DissolveTeam();

	TArray<ADWCharacter*> GetMembers(ADWCharacter* InMember = nullptr);

	FORCEINLINE int GetNumMember()
	{
		return Members.Num();
	}

	FORCEINLINE bool IsCaptain(ADWCharacter* InMember)
	{
		return Captain == nullptr || Captain == InMember;
	}

	FORCEINLINE bool IsValid()
	{
		return !ID.IsNone();
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FMeshUVData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D UVCorner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D UVSpan;


	FORCEINLINE FMeshUVData()
	{
		UVCorner = FVector2D::ZeroVector;
		UVSpan = FVector2D::UnitVector;
	}

	FORCEINLINE FMeshUVData(FVector2D InUVCorner, FVector2D InUVSpan)
	{
		UVCorner = InUVCorner;
		UVSpan = InUVSpan;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FName ID;

	UPROPERTY(BlueprintReadWrite)
	EItemType Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Detail;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon;
	//FSlateBrush Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxLevel;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UDWItemAbility> AbilityClass;

	FORCEINLINE FItemData()
	{
		ID = NAME_None;
		Type = EItemType::None;
		Icon = nullptr;
		//Icon = FSlateBrush();
		Price = 0;
		MaxCount = -1;
		Level = 1;
		MaxLevel = -1;
		AbilityClass = nullptr;
	}

	FORCEINLINE bool IsValid()
	{
		return !ID.IsNone();
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FSkillData : public FItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkillType SkillType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkillMode SkillMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASkill> SkillClass;

	FORCEINLINE FSkillData()
	{
		SkillType = ESkillType::None;
		SkillMode = ESkillMode::None;
		SkillClass = nullptr;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FVoxelData : public FItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EVoxelType VoxelType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UVoxel> VoxelClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AVoxelAuxiliary> AuxiliaryClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETransparency Transparency;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCustomMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Range;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> MeshVertices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> MeshNormals;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMeshUVData> MeshUVDatas;
				
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* GenerateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundBase*> OperationSounds;

	FORCEINLINE FVoxelData()
	{
		MaxCount = 64;
		VoxelType = EVoxelType::Empty;
		VoxelClass = nullptr;
		AuxiliaryClass = nullptr;
		Transparency = ETransparency::Solid;
		bCustomMesh = false;
		Range = FVector::OneVector;
		Offset = FVector::ZeroVector;
		MeshVertices = TArray<FVector>();
		MeshNormals = TArray<FVector>();
		for (int i = 0; i < 6; i++)
		{
			MeshUVDatas.Add(FMeshUVData());
		}
		GenerateSound = nullptr;
		OperationSounds = TArray<USoundBase*>();
	}

	FORCEINLINE bool IsComplex()
	{
		return GetCeilRange() != FVector::OneVector;
	}

	FVector GetCeilRange(UVoxel* InVoxel);

	FVector GetFinalRange(UVoxel* InVoxel);

	FORCEINLINE FVector GetCeilRange(FRotator InRotation = FRotator::ZeroRotator, FVector InScale = FVector::OneVector)
	{
		FVector range = GetFinalRange(InRotation, InScale);
		range.X = FMath::CeilToFloat(range.X);
		range.Y = FMath::CeilToFloat(range.Y);
		range.Z = FMath::CeilToFloat(range.Z);
		return range;
	}

	FORCEINLINE FVector GetFinalRange(FRotator InRotation = FRotator::ZeroRotator, FVector InScale = FVector::OneVector)
	{
		FVector range = InRotation.RotateVector(Range * InScale);
		range.X = FMath::Abs(range.X);
		range.Y = FMath::Abs(range.Y);
		range.Z = FMath::Abs(range.Z);
		return range;
	}

	FORCEINLINE FVector2D GetUVCorner(EFacing InFacing, FVector2D InUVSize)
	{
		return GetUVCorner((int)InFacing, InUVSize);
	}

	FORCEINLINE FVector2D GetUVCorner(int InFaceIndex, FVector2D InUVSize)
	{
		FMeshUVData uvData = FMeshUVData();
		if (MeshUVDatas.Num() > InFaceIndex)
			uvData = MeshUVDatas[InFaceIndex];
		return FVector2D(uvData.UVCorner.X	 * InUVSize.X, (1 / InUVSize.Y - uvData.UVCorner.Y - uvData.UVSpan.Y) * InUVSize.Y);
	}

	FORCEINLINE FVector2D GetUVSpan(EFacing InFacing, FVector2D InUVSize)
	{
		return GetUVSpan((int)InFacing, InUVSize);
	}

	FORCEINLINE FVector2D GetUVSpan(int InFaceIndex, FVector2D InUVSize)
	{
		FMeshUVData uvData = FMeshUVData();
		if (MeshUVDatas.Num() > InFaceIndex)
			uvData = MeshUVDatas[InFaceIndex];
		return FVector2D(uvData.UVSpan.X * InUVSize.X, uvData.UVSpan.Y * InUVSize.Y);
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FPropData : public FItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPropType PropType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* PropMesh;

	FORCEINLINE FPropData()
	{
		MaxCount = 10;
		PropType = EPropType::Potion;
		PropMesh = nullptr;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FEquipData : public FItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipType EquipType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipPartType PartType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* EquipMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEquip> EquipClass;
			
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> EffectClass;

	FORCEINLINE FEquipData()
	{
		MaxCount = 1;
		EquipType = EEquipType::Weapon;
		PartType = EEquipPartType::Head;
		EquipMesh = nullptr;
		EquipClass = nullptr;
		EffectClass = nullptr;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FEquipWeaponData : public FEquipData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponHandType HandType;

	FORCEINLINE FEquipWeaponData()
	{
		EquipType = EEquipType::Weapon;
		HandType = EWeaponHandType::Single;
		WeaponType = EWeaponType::None;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FEquipShieldData : public FEquipData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EShieldType ShieldType;

	FORCEINLINE FEquipShieldData()
	{
		EquipType = EEquipType::Shield;
		ShieldType = EShieldType::None;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FEquipArmorData : public FEquipData
{
	GENERATED_BODY()

public:
	FORCEINLINE FEquipArmorData()
	{
		EquipType = EEquipType::Armor;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 Count;

	static FItem Empty;

	FORCEINLINE static FItem Clone(FItem InItem, int InCount = -1)
	{
		if (InItem == Empty) return Empty;
		FItem tmpItem = FItem();
		tmpItem.ID = InItem.ID;
		tmpItem.Level = InItem.Level;
		tmpItem.Count = InCount == -1 ? InItem.Count : InCount;
		return tmpItem;
	}

public:
	FORCEINLINE FItem()
	{
		ID = NAME_None;
		Level = 1;
		Count = 0;
	}
		
	FORCEINLINE FItem(const FName& InID, int32 InLevel = 1, int32 InCount = 1)
	{
		ID = InID;
		Level = InLevel;
		Count = InCount;
	}
	
	FORCEINLINE bool IsValid() const
	{
		return !ID.IsNone();
	}

	FORCEINLINE bool EqualType(FItem InItem) const
	{
		return InItem.IsValid() && InItem.ID == ID;
	}
	
	FORCEINLINE FItemData GetData() const;

	FORCEINLINE friend bool operator==(const FItem& A, const FItem& B)
	{
		return (A.ID == B.ID) && (A.GetData().Type == B.GetData().Type) && (A.Count == B.Count);
	}

	FORCEINLINE friend bool operator!=(const FItem& A, const FItem& B)
	{
		return (A.ID != B.ID) || (A.GetData().Type != B.GetData().Type) || (A.Count != B.Count);
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FVoxelItem
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	FString VoxelData;

	UPROPERTY(Transient)
	AVoxelAuxiliary* VoxelAuxiliary;

	FVoxelItem()
	{
		VoxelData = TEXT("");
		VoxelAuxiliary = nullptr;
	}

	FVoxelItem(FString InVoxelData, AVoxelAuxiliary* InVoxelAuxiliary)
	{
		VoxelData = InVoxelData;
		VoxelAuxiliary = InVoxelAuxiliary;
	}

	FVoxelItem(UVoxel* InVoxel);
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FVoxelHitResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Point;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Normal;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UVoxel> Voxel;

	FVoxelHitResult();
	
	explicit FVoxelHitResult(UVoxel* InVoxel, FVector InPoint, FVector InNormal);

	FORCEINLINE UVoxel* GetVoxel() const
	{
		return Voxel.Get();
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FChunkMaterial
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D BlockUVSize;

	FORCEINLINE FChunkMaterial()
	{
		Material = nullptr;
		BlockUVSize = FVector2D(0.0625f, 0.0625f);
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	bool bInitialized;

	FORCEINLINE FSaveData()
	{
		bInitialized = false;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FGameData : public FSaveData
{
	GENERATED_BODY()

public:
	FORCEINLINE FGameData()
	{
		
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FWorldData : public FSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Seed;
	
	FORCEINLINE FWorldData()
	{
		Name = TEXT("");
		Seed = 0;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FPickUpData : public FSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FItem Item;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Location;
	
	FORCEINLINE FPickUpData()
	{
		Item = FItem::Empty;
		Location = FVector::ZeroVector;
	}
};

UENUM(BlueprintType)
enum class ESplitSlotType : uint8
{
	Default,
	Shortcut,
	Auxiliary,
	Generate,
	Equip,
	Skill
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FSplitSlotInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 StartIndex;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 TotalCount;

	FORCEINLINE FSplitSlotInfo()
	{
		StartIndex = 0;
		TotalCount = 0;
	}

	FORCEINLINE FSplitSlotInfo(int32 InStartIndex, int32 InTotalCount)
	{
		StartIndex = InStartIndex;
		TotalCount = InTotalCount;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FSplitSlotData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<UInventorySlot*> Slots;

	FORCEINLINE FSplitSlotData()
	{
		Slots = TArray<UInventorySlot*>();
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FInventoryData : public FSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<ESplitSlotType, FSplitSlotInfo> SplitInfos;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FItem> Items;
		
	FORCEINLINE FInventoryData()
	{
		SplitInfos = TMap<ESplitSlotType, FSplitSlotInfo>();
		Items = TArray<FItem>();
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FCharacterData : public FSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString RaceID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString TeamID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 EXP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Location;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator Rotation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FInventoryData InventoryData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UDWCharacterAttributeSet* AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<ADWCharacter> Class;

	FORCEINLINE FCharacterData()
	{
		Name = TEXT("");
		RaceID = TEXT("");
		TeamID = TEXT("");
		Level = 0;
		EXP = 0;
		Location = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
		InventoryData = FInventoryData();
		AttributeSet = nullptr;
		Class = nullptr;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FVitalityObjectData : public FSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString RaceID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 EXP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Location;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator Rotation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FInventoryData InventoryData;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UDWAttributeSet* AttributeSet;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<AVitalityObject> Class;

	FORCEINLINE FVitalityObjectData()
	{
		Name = TEXT("");
		RaceID = TEXT("");
		Level = 0;
		EXP = 0;
		Location = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
		InventoryData = FInventoryData();
		AttributeSet = nullptr;
		Class = nullptr;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FChunkData : public FSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FIndex Index;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString> VoxelDatas;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FPickUpData> PickUpDatas;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FCharacterData> CharacterDatas;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FVitalityObjectData> VitalityObjectDatas;

	FORCEINLINE FChunkData()
	{
		Index = FIndex::ZeroIndex;
		VoxelDatas = TArray<FString>();
		PickUpDatas = TArray<FPickUpData>();
		CharacterDatas = TArray<FCharacterData>();
		VitalityObjectDatas = TArray<FVitalityObjectData>();
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FWorldRecordData : public FSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString WorldName;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector PlayerLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator PlayerRotation;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int TimeSeconds;

	FORCEINLINE FWorldRecordData()
	{
		WorldName = TEXT("");
		PlayerLocation = FVector::ZeroVector;
		PlayerRotation = FRotator::ZeroRotator;
		TimeSeconds = 0;
	}
};

/**
 * GameplayEffect????
 */
USTRUCT(BlueprintType)
struct FDWGameplayEffectContainer
{
	GENERATED_BODY()

public:
    FDWGameplayEffectContainer() {}

public:
    /** ??????? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffectContainer")
    TSubclassOf<UDWTargetType> TargetType;

    /** ??????????GameplayEffect?б? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffectContainer")
    TArray<TSubclassOf<UGameplayEffect>> TargetGameplayEffectClasses;
};

/**
 * GameplayEffect?????淶
 */
USTRUCT(BlueprintType)
struct FDWGameplayEffectContainerSpec
{
    GENERATED_BODY()

public:
    FDWGameplayEffectContainerSpec() {}

public:
    /** ??????? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffectContainer")
    FGameplayAbilityTargetDataHandle TargetData;

    /** ?????????GameplayEffect?б? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffectContainer")
    TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;
    
    /** ???????Ч?????*/
    bool HasValidTargets() const;

    /** ???????Ч??Effect*/
    bool HasValidEffects() const;

    /** ???????????????? */
    void AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FDWAbilityInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EAbilityCostType CostType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Cost;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Cooldown;

	FORCEINLINE FDWAbilityInfo()
	{
		CostType = EAbilityCostType::None;
		Cost = 0.f;
		Cooldown = -1.f;
	}

	FORCEINLINE FDWAbilityInfo(EAbilityCostType InCostType, float InCost, float InCooldown)
	{
		CostType = InCostType;
		Cost = InCost;
		Cooldown = InCooldown;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FDWAbilityData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AbilityLevel;

	FGameplayAbilitySpecHandle AbilityHandle;

	FORCEINLINE FDWAbilityData()
	{
		AbilityName = NAME_None;
		AbilityLevel = 1;
		AbilityHandle = FGameplayAbilitySpecHandle();
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FDWCharacterActionAbilityData : public FDWAbilityData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterActionType ActionType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDWCharacterActionAbility> AbilityClass;

	FORCEINLINE FDWCharacterActionAbilityData()
	{
		ActionType = ECharacterActionType::None;
		AbilityClass = nullptr;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FCharacterAttackAbilityData : public FDWAbilityData
{
	GENERATED_BODY()

	public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDWCharacterAttackAbility> AbilityClass;

	FORCEINLINE FCharacterAttackAbilityData()
	{
		WeaponType = EWeaponType::None;
		AbilityClass = nullptr;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FDWCharacterAttackAbilityData : public FDWAbilityData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDWCharacterAttackAbility> AbilityClass;

	FORCEINLINE FDWCharacterAttackAbilityData()
	{
		WeaponType = EWeaponType::None;
		AbilityClass = nullptr;
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FDWCharacterSkillAbilityData : public FDWAbilityData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDWCharacterSkillAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCancelAble;

	FORCEINLINE FDWCharacterSkillAbilityData()
	{
		WeaponType = EWeaponType::None;
		AbilityClass = nullptr;
		bCancelAble = false;
	}
	
	FORCEINLINE FSkillData GetItemData() const;
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FDWEffectData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EffectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AbilityLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> EffectClass;

	FActiveGameplayEffectHandle EffectHandle;

	FORCEINLINE FDWEffectData()
	{
		EffectName = NAME_None;
		AbilityLevel = 1;
		EffectHandle = FActiveGameplayEffectHandle();
	}
};

USTRUCT(BlueprintType)
struct DREAMWORLD_API FDWCharacterPassiveEffectData : public FDWEffectData
{
	GENERATED_BODY()
};

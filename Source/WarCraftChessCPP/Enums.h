#pragma once

#include "CoreMinimal.h"
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class EDirections : uint8
{
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
	Forward UMETA(DisplayName = "Forward"),
	ForwardLeft UMETA(DisplayName = "Forward Left"),
	ForwardRight UMETA(DisplayName = "Forward Right"),
	Backward UMETA(DisplayName = "Backward"),
	BackwardLeft UMETA(DisplayName = "Backward Left"),
	BackwardRight UMETA(DisplayName = "Backward Right"),
};

UENUM(BlueprintType)
enum class EPieceTypes : uint8
{
	Pawn UMETA(DisplayName = "Pawn"),
	Rook UMETA(DisplayName = "Rook"),
	Knight UMETA(DisplayName = "Knight"),
	Bishop UMETA(DisplayName = "Bishop"),
	Queen UMETA(DisplayName = "Queen"),
	King UMETA(DisplayName = "King"),
	INVALID UMETA(DisplayName = "INVALID"),
};

UENUM(BlueprintType)
enum class ERaces : uint8
{
	Undead UMETA(DisplayName = "Undead"),
	Human UMETA(DisplayName = "Human"),
	Orc UMETA(DisplayName = "Orc"),
	NightElf UMETA(DisplayName = "Night Elf"),
	Dwarf UMETA(DisplayName = "Dwarf"),
	BloodElf UMETA(DisplayName = "Blood Elf"),
	Draenei UMETA(DisplayName = "Draenei"),
	Worgen UMETA(DisplayName = "Worgen"),
	Tauren UMETA(DisplayName = "Tauren"),
	Troll UMETA(DisplayName = "Troll"),
	Goblin UMETA(DisplayName = "Goblin"),
	Gnome UMETA(DisplayName = "Gnome"),
};

UENUM(BlueprintType)
enum class ETeams : uint8
{
	Black UMETA(DisplayName = "Black"),
	White UMETA(DisplayName = "White"),

};

UENUM(BlueprintType)
enum class ETileName : uint8
{
	A1,	A2,	A3,	A4,	A5,	A6,	A7,	A8,
	B1,	B2,	B3,	B4,	B5,	B6,	B7,	B8,
	C1, C2, C3, C4, C5, C6, C7, C8,
	D1, D2, D3, D4, D5, D6, D7, D8,
	E1, E2, E3, E4, E5, E6, E7, E8,
	F1, F2, F3, F4, F5, F6, F7, F8,
	G1, G2, G3, G4, G5, G6, G7, G8,
	H1, H2, H3, H4, H5, H6, H7, H8
};

UENUM(BlueprintType)
enum class EAttackTypes : uint8
{
	Meele,
	Ranged,
	Channeled,
	Notify,
	MeeleNotify
};
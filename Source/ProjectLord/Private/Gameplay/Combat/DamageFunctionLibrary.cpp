// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/DamageFunctionLibrary.h"

static int CalculateDamageFromCharacteristic(int HeroLevel, int Characteristic, int BonusDamage)
{
	return (HeroLevel - 1) + FMath::CeilToInt((float)Characteristic / 8.f) + BonusDamage;
}

/*static*/ int UDamageFunctionLibrary::CalculateBaseMeleeDamageDirect(int HeroLevel, int Strength, int BonusDamage)
{
	return CalculateDamageFromCharacteristic(HeroLevel, Strength, BonusDamage);
}

/*static*/ int UDamageFunctionLibrary::CalculateBaseRangedDamageDirect(int HeroLevel, int Agility, int BonusDamage)
{
	return CalculateDamageFromCharacteristic(HeroLevel, Agility, BonusDamage);
}

/*static*/ int UDamageFunctionLibrary::CalculateBaseMagicDamageDirect(int HeroLevel, int Intelligence, int BonusDamage)
{
	return CalculateDamageFromCharacteristic(HeroLevel, Intelligence, BonusDamage);
}
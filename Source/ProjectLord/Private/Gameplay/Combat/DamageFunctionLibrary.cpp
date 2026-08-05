// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/DamageFunctionLibrary.h"


/*static*/ int UDamageFunctionLibrary::CalculateDamage(int AttackStrength, int Defense)
{
	// Apparently Majesty 2 uses
	// reduction = defense / (defense + K)
	// and settled on K = 40. And then damage is just strength * (1 - reduction),
	// with a min value of 1.
	constexpr float K = 40.0f;
	const float Reduc = (float)Defense / ((float)Defense + K);
	return FMath::Max(1, FMath::Floor((float) AttackStrength * (1.0 - Reduc)));
}

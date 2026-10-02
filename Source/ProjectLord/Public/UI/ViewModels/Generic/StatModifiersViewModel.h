// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "StatModifiersViewModel.generated.h"

UENUM(BlueprintType)
enum class EModifierType : uint8
{
    None,
    Add,
    Scale,
};

USTRUCT(BlueprintType)
struct PROJECTLORD_API FStatModifierRow
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Value;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EModifierType Type;
};

USTRUCT(BlueprintType)
struct PROJECTLORD_API FStatModifiers
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly)
    TOptional<FStatModifierRow> MeleeDamage;

    UPROPERTY(EditDefaultsOnly)
    TOptional<FStatModifierRow> RangedDamage;

    UPROPERTY(EditDefaultsOnly)
    TOptional<FStatModifierRow> MagicDamage;

    UPROPERTY(EditDefaultsOnly)
    TOptional<FStatModifierRow> MeleeDefense;

    UPROPERTY(EditDefaultsOnly)
    TOptional<FStatModifierRow> RangedDefense;

    UPROPERTY(EditDefaultsOnly)
    TOptional<FStatModifierRow> MagicDefense;

    UPROPERTY(EditDefaultsOnly)
    TOptional<FStatModifierRow> Movement;

    UPROPERTY(EditDefaultsOnly)
    TOptional<FStatModifierRow> Sight;

};

UCLASS(BlueprintType, EditInlineNew)
class PROJECTLORD_API UVMStatModifierRow : public UVMLordBase
{
    GENERATED_BODY()
public:

    float GetValue() const { return Value; }
    EModifierType GetModType() const { return ModType; }
    void SetValue(float InValue, EModifierType InType)
    {
        UE_MVVM_SET_PROPERTY_VALUE(Value, InValue);
        UE_MVVM_SET_PROPERTY_VALUE(ModType, InType);
    }

    static UVMStatModifierRow* Make(UObject* Outer, const FStatModifierRow& Row);

protected:
    UPROPERTY(FieldNotify, EditDefaultsOnly, BlueprintReadOnly, Getter, Category = "Stat Modifier")
    float Value = 0;

    UPROPERTY(FieldNotify, EditDefaultsOnly, BlueprintReadOnly, Getter, Category = "Stat Modifier")
    EModifierType ModType = EModifierType::Add;

};


UCLASS(BlueprintType, EditInlineNew)
class PROJECTLORD_API UVMStatModifiers : public UVMLordBase
{
    GENERATED_BODY()

public:
    UVMStatModifiers();
    static UVMStatModifiers* Make(UObject* Outer, const FStatModifiers& Modifiers);
    
    UVMStatModifierRow* GetMeleeDefense() const { return MeleeDefense; }
    UVMStatModifierRow* GetRangedDefense() const { return RangedDefense; }
    UVMStatModifierRow* GetMagicDefense() const { return MagicDefense; }

    UVMStatModifierRow* GetMeleeDamage() const { return MeleeDamage; }
    UVMStatModifierRow* GetRangedDamage() const { return RangedDamage; }
    UVMStatModifierRow* GetMagicDamage() const { return MagicDamage; }

    UVMStatModifierRow* GetSight() const { return Sight; }
    UVMStatModifierRow* GetMovement() const { return Movement; }

protected:

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced, Getter, Category = "Stat Modifiers")
    TObjectPtr<UVMStatModifierRow> MeleeDefense;
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced, Getter, Category = "Stat Modifiers")
    TObjectPtr<UVMStatModifierRow> RangedDefense;
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced, Getter, Category = "Stat Modifiers")
    TObjectPtr<UVMStatModifierRow> MagicDefense;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced, Getter, Category = "Stat Modifiers")
    TObjectPtr<UVMStatModifierRow> MeleeDamage;
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced, Getter, Category = "Stat Modifiers")
    TObjectPtr<UVMStatModifierRow> RangedDamage;
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced, Getter, Category = "Stat Modifiers")
    TObjectPtr<UVMStatModifierRow> MagicDamage;

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced, Getter, Category = "Stat Modifiers")
    TObjectPtr<UVMStatModifierRow> Sight;
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Getter, Category = "Stat Modifiers")
    TObjectPtr<UVMStatModifierRow> Movement;

    friend FStatModifiers;

};

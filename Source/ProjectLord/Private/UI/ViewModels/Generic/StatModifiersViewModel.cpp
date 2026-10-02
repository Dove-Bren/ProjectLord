// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/Generic/StatModifiersViewModel.h"

UVMStatModifiers::UVMStatModifiers()
{
    /*MeleeDefense = CreateLordVM<UVMStatModifierRow>(this);
    RangedDefense = CreateLordVM<UVMStatModifierRow>(this);
    MagicDefense = CreateLordVM<UVMStatModifierRow>(this);

    MeleeDamage = CreateLordVM<UVMStatModifierRow>(this);
    RangedDamage = CreateLordVM<UVMStatModifierRow>(this);
    MagicDamage = CreateLordVM<UVMStatModifierRow>(this);

    Sight = CreateLordVM<UVMStatModifierRow>(this);
    Movement = CreateLordVM<UVMStatModifierRow>(this);*/
}

/*static*/ UVMStatModifierRow* UVMStatModifierRow::Make(UObject* Outer, const FStatModifierRow& Row)
{
    auto VM = CreateLordVM<UVMStatModifierRow>(Outer);
    VM->SetValue(Row.Value, Row.Type);
    return VM;
}

/*static*/ UVMStatModifiers* UVMStatModifiers::Make(UObject* Outer, const FStatModifiers& Modifiers)
{
    auto VM = CreateLordVM<UVMStatModifiers>(Outer);
    
    if (Modifiers.MeleeDamage) VM->MeleeDamage = UVMStatModifierRow::Make(Outer, Modifiers.MeleeDamage.GetValue());
    if (Modifiers.RangedDamage) VM->RangedDamage = UVMStatModifierRow::Make(Outer, Modifiers.RangedDamage.GetValue());
    if (Modifiers.MagicDamage) VM->MagicDamage = UVMStatModifierRow::Make(Outer, Modifiers.MagicDamage.GetValue());

    if (Modifiers.MeleeDefense) VM->MeleeDefense = UVMStatModifierRow::Make(Outer, Modifiers.MeleeDefense.GetValue());
    if (Modifiers.RangedDefense) VM->RangedDefense = UVMStatModifierRow::Make(Outer, Modifiers.RangedDefense.GetValue());
    if (Modifiers.MagicDefense) VM->MagicDefense = UVMStatModifierRow::Make(Outer, Modifiers.MagicDefense.GetValue());

    if (Modifiers.Movement) VM->Movement = UVMStatModifierRow::Make(Outer, Modifiers.Movement.GetValue());
    if (Modifiers.Sight) VM->Sight = UVMStatModifierRow::Make(Outer, Modifiers.Sight.GetValue());

    return VM;
}

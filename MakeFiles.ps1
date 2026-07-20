param(
    [Parameter(Mandatory = $true)]
    [string]$ClassPath
)

# Easy-to-configure project source prefix
$Prefix = "Source/ProjectLord"

# Normalize path separators
$RelativePath = $ClassPath -replace '\\', '/'

# Build output paths relative to the script location
$PublicRoot  = Join-Path $PSScriptRoot "$Prefix/Public"
$PrivateRoot = Join-Path $PSScriptRoot "$Prefix/Private"

$HeaderPath = Join-Path $PublicRoot "$RelativePath.h"
$CppPath    = Join-Path $PrivateRoot "$RelativePath.cpp"

# Create parent directories if needed
$HeaderDir = Split-Path $HeaderPath -Parent
$CppDir    = Split-Path $CppPath -Parent

New-Item -ItemType Directory -Force -Path $HeaderDir | Out-Null
New-Item -ItemType Directory -Force -Path $CppDir | Out-Null

# Extract class/file name
$ClassName = Split-Path $RelativePath -Leaf

# Generate header
$HeaderContent = @"
// Copyright (c) Skyler Manzanares. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "$ClassName.generated.h"

UCLASS()
class PROJECTLORD_API U$ClassName : public UObject
{
    GENERATED_BODY()

public:
    U$ClassName();
};
"@

# Path used in the #include (relative to Public)
$IncludePath = "$RelativePath.h"

# Generate cpp
$CppContent = @"
// Copyright (c) Skyler Manzanares. All Rights Reserved.

#include "$IncludePath"

U$ClassName::U$ClassName()
{
}
"@

# Write files
Set-Content -Path $HeaderPath -Value $HeaderContent -Encoding UTF8
Set-Content -Path $CppPath -Value $CppContent -Encoding UTF8

Write-Host "Created:"
Write-Host "  $HeaderPath"
Write-Host "  $CppPath"
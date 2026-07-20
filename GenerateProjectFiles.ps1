$uproject = Get-ChildItem *.uproject | Select-Object -First 1
& "${env:ProgramFiles(x86)}\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe" `
    /projectfiles $uproject.FullName
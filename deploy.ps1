$buildDll = "build\windows\x64\release\MuteNPC.dll"
$buildPdb = "build\windows\x64\release\MuteNPC.pdb"

$testTargetDir = "C:\Games\Skyrim_Test\Mod Organizer\mods\MuteNPC\SKSE\Plugins"

if (Test-Path $buildDll) {
    New-Item -ItemType Directory -Force -Path $testTargetDir | Out-Null
    Copy-Item -Force $buildDll $testTargetDir
    if (Test-Path $buildPdb) {
        Copy-Item -Force $buildPdb $testTargetDir
    }
    Write-Host "[+] MuteNPC deployed to Skyrim_Test: $testTargetDir" -ForegroundColor Green
} else {
    Write-Host "[-] Build DLL not found at $buildDll" -ForegroundColor Red
}

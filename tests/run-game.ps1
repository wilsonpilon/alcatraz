param([string]$OpenMSX = "openmsx.exe")
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$out = Join-Path $root "out"
$work = Join-Path $out "game-test"
$msxtar = Join-Path $root "MSXgl\tools\build\msxtar\msxtar.exe"
$dos = Join-Path $root "MSXgl\tools\build\DOS"
foreach ($file in @("alcatraz.com", "ALCATRAZ.SC5", "alcatraz.noi")) {
    if (!(Test-Path (Join-Path $out $file))) { throw "Compile o projeto antes dos testes." }
}
New-Item -ItemType Directory -Force $work | Out-Null
Copy-Item (Join-Path $dos "MSXDOS2.SYS"), (Join-Path $dos "COMMAND2.COM") $work
Copy-Item (Join-Path $out "alcatraz.com"), (Join-Path $out "ALCATRAZ.SC5") $work
[IO.File]::WriteAllText((Join-Path $work "AUTOEXEC.BAT"), "ALCATRAZ`r`n", [Text.Encoding]::ASCII)
Push-Location $work
try {
    & $msxtar -cf game.dsk --dos2 --size=720K MSXDOS2.SYS COMMAND2.COM AUTOEXEC.BAT alcatraz.com ALCATRAZ.SC5
    if ($LASTEXITCODE -ne 0) { throw "Falha ao criar disco de teste." }
} finally { Pop-Location }
foreach ($case in @("pt", "en")) {
    $env:ALCATRAZ_GAME_CASE = $case
    $log = Join-Path $work "$case.log"
    if (Test-Path $log) { Remove-Item $log }
    $arguments = @("-machine", "Panasonic_FS-A1WX", "-ext", "slotexpander", "-ext", "msxdos2",
        "-ext", "ram2mb", "-diska", "`"$(Join-Path $work 'game.dsk')`"",
        "-script", "`"$(Join-Path $PSScriptRoot 'game.tcl')`"")
    $process = Start-Process $OpenMSX -ArgumentList $arguments -PassThru
    if (!$process.WaitForExit(90000)) {
        Stop-Process -Id $process.Id
        throw "openMSX excedeu o tempo no caso $case."
    }
    if ($process.ExitCode -ne 0 -or !(Test-Path $log)) { throw "openMSX falhou no caso $case." }
    $result = (Get-Content $log -Raw).Trim()
    if ($result -ne "PASS") { throw "${case}: $result" }
    Write-Output "${case}: PASS"
}

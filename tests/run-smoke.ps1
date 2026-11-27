param([string]$OpenMSX = "openmsx.exe")
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$out = Join-Path $root "out"
$binary = Join-Path $out "alcatraz.com"
$msxtar = Join-Path $root "MSXgl\tools\build\msxtar\msxtar.exe"
$dos = Join-Path $root "MSXgl\tools\build\DOS"
if (!(Test-Path $binary)) { throw "Compile o projeto antes de executar os testes." }
Copy-Item (Join-Path $dos "MSXDOS2.SYS"), (Join-Path $dos "COMMAND2.COM") $out
[IO.File]::WriteAllText((Join-Path $out "AUTOEXEC.BAT"), "ALCATRAZ`r`n", [Text.Encoding]::ASCII)
Push-Location $out
try {
    & $msxtar -cf smoke.dsk --dos2 --size=720K MSXDOS2.SYS COMMAND2.COM AUTOEXEC.BAT alcatraz.com
    if ($LASTEXITCODE -ne 0) { throw "Falha ao criar disco de teste." }
} finally {
    Pop-Location
}

$cases = @(
    @{Name="compatible"; Machine="Panasonic_FS-A1WX"; Ram="ram2mb"; Mapper="1"; Fm="1"},
    @{Name="lowram"; Machine="Panasonic_FS-A1WX"; Ram="ram1mb"; Mapper="0"; Fm="1"},
    @{Name="nofm"; Machine="Panasonic_FS-A1FX"; Ram="ram2mb"; Mapper="1"; Fm="0"},
    @{Name="4mb"; Machine="Panasonic_FS-A1WX"; Ram="ram4mb"; Mapper="1"; Fm="1"}
)
foreach ($case in $cases) {
    $env:ALCATRAZ_TEST_NAME = $case.Name
    $env:ALCATRAZ_TEST_MAPPER = $case.Mapper
    $env:ALCATRAZ_TEST_FM = $case.Fm
    $log = Join-Path $out "$($case.Name).log"
    if (Test-Path $log) { Remove-Item $log }
    $arguments = @("-machine", $case.Machine, "-ext", "slotexpander", "-ext", "msxdos2",
        "-ext", $case.Ram, "-diska", "`"$(Join-Path $out 'smoke.dsk')`"",
        "-script", "`"$(Join-Path $PSScriptRoot 'smoke.tcl')`"")
    $process = Start-Process $OpenMSX -ArgumentList $arguments -PassThru
    if (!$process.WaitForExit(90000)) {
        Stop-Process -Id $process.Id
        throw "openMSX excedeu o tempo no caso $($case.Name)."
    }
    if ($process.ExitCode -ne 0 -or !(Test-Path $log)) {
        throw "openMSX falhou no caso $($case.Name)."
    }
    $result = Get-Content $log
    Write-Output "$($case.Name): $($result -join ' ')"
    if ($result[-1] -ne "PASS") { throw "Teste falhou: $($case.Name)." }
}

param([string]$OpenMSX = "openmsx.exe")
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$out = Join-Path $root "out"
$work = Join-Path $out "sc5-test"
$msxtar = Join-Path $root "MSXgl\tools\build\msxtar\msxtar.exe"
$dos = Join-Path $root "MSXgl\tools\build\DOS"
foreach ($file in @("alcatraz.com", "ALCATRAZ.SC5")) {
    if (!(Test-Path (Join-Path $out $file))) { throw "Compile o projeto antes dos testes." }
}
New-Item -ItemType Directory -Force $work | Out-Null
Copy-Item (Join-Path $dos "MSXDOS2.SYS"), (Join-Path $dos "COMMAND2.COM") $work
Copy-Item (Join-Path $out "alcatraz.com"), (Join-Path $out "ALCATRAZ.SC5") $work
[IO.File]::WriteAllText((Join-Path $work "AUTOEXEC.BAT"), "ALCATRAZ`r`n", [Text.Encoding]::ASCII)
$valid = [IO.File]::ReadAllBytes((Join-Path $work "ALCATRAZ.SC5"))
$cases = @("missing", "badheader", "shortheader", "truncated", "incompatible", "valid", "valid-escape", "animation-escape")
foreach ($case in $cases) {
    $env:ALCATRAZ_SC5_CASE = $case
    $image = Join-Path $work "ALCATRAZ.SC5"
    if ($case -eq "missing") {
        Remove-Item $image
    } elseif ($case -eq "badheader") {
        $bytes = [byte[]]$valid.Clone()
        $bytes[0] = 0
        [IO.File]::WriteAllBytes($image, $bytes)
    } elseif ($case -eq "shortheader") {
        [IO.File]::WriteAllBytes($image, $valid[0..2])
    } elseif ($case -eq "truncated") {
        [IO.File]::WriteAllBytes($image, $valid[0..1030])
    } else {
        [IO.File]::WriteAllBytes($image, $valid)
    }
    $disk = Join-Path $work "$case.dsk"
    $log = Join-Path $work "$case.log"
    if (Test-Path $log) { Remove-Item $log }
    Push-Location $work
    try {
        $files = @("MSXDOS2.SYS", "COMMAND2.COM", "AUTOEXEC.BAT", "alcatraz.com")
        if ($case -ne "missing") { $files += "ALCATRAZ.SC5" }
        & $msxtar -cf "$case.dsk" --dos2 --size=720K @files
        if ($LASTEXITCODE -ne 0) { throw "Falha ao criar disco: $case" }
    } finally { Pop-Location }
    $machine = if ($case -eq "incompatible") { "Panasonic_FS-A1FX" } else { "Panasonic_FS-A1WX" }
    $arguments = @("-machine", $machine, "-ext", "slotexpander", "-ext", "msxdos2",
        "-ext", "ram2mb", "-diska", "`"$disk`"",
        "-script", "`"$(Join-Path $PSScriptRoot 'sc5.tcl')`"")
    $process = Start-Process $OpenMSX -ArgumentList $arguments -PassThru
    if (!$process.WaitForExit(90000)) {
        Stop-Process -Id $process.Id
        throw "openMSX excedeu o tempo no caso $case."
    }
    if ($process.ExitCode -ne 0 -or !(Test-Path $log) -or
        ((Get-Content $log) -join ",") -ne "PASS,PASS_EXIT") {
        throw "Teste de imagem falhou: $case."
    }
    Write-Output "${case}: PASS"
}

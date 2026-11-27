param([string]$OpenMSX = "openmsx.exe")
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$out = Join-Path $root "out"
$work = Join-Path $out "logo-test"
$source = Join-Path $root "docs\disprosoft.bas"
$msxtar = Join-Path $root "MSXgl\tools\build\msxtar\msxtar.exe"
$dos = Join-Path $root "MSXgl\tools\build\DOS"
if (!(Test-Path $source)) { throw "A comparacao requer a listagem local docs\disprosoft.bas." }
if (!(Test-Path (Join-Path $out "alcatraz.com"))) { throw "Compile o projeto antes do teste." }
New-Item -ItemType Directory -Force $work | Out-Null

$basic = Get-Content $source -Raw
if (($basic | Select-String -Pattern "241 CIRCLE" -AllMatches).Matches.Count -ne 1) {
    throw "A junção esperada das linhas 40 e 41 nao foi encontrada."
}
$basic = $basic.Replace("241 CIRCLE", "2`r`n41 CIRCLE") + "`r`n75 GOTO 75`r`n"
[IO.File]::WriteAllText((Join-Path $work "DISPRO.BAS"), $basic, [Text.Encoding]::ASCII)
Copy-Item (Join-Path $dos "MSXDOS2.SYS"), (Join-Path $dos "COMMAND2.COM") $work
Copy-Item (Join-Path $out "alcatraz.com") $work
[IO.File]::WriteAllText((Join-Path $work "AUTOEXEC.BAT"), "ALCATRAZ`r`n", [Text.Encoding]::ASCII)

Push-Location $work
try {
    & $msxtar -cf basic.dsk --dos2 --size=720K DISPRO.BAS
    if ($LASTEXITCODE -ne 0) { throw "Falha ao criar disco BASIC." }
    & $msxtar -cf logo.dsk --dos2 --size=720K MSXDOS2.SYS COMMAND2.COM AUTOEXEC.BAT alcatraz.com
    if ($LASTEXITCODE -ne 0) { throw "Falha ao criar disco do programa." }
} finally { Pop-Location }

foreach ($case in @("basic", "logo")) {
    foreach ($extension in @("vram", "palette", "png", "log")) {
        $file = Join-Path $work "$case.$extension"
        if (Test-Path $file) { Remove-Item $file }
    }
    $script = if ($case -eq "basic") { "basic-oracle.tcl" } else { "logo.tcl" }
    $arguments = @("-machine", "Panasonic_FS-A1WX", "-ext", "slotexpander", "-ext", "msxdos2",
        "-ext", "ram2mb", "-diska", "`"$(Join-Path $work "$case.dsk")`"",
        "-script", "`"$(Join-Path $PSScriptRoot $script)`"")
    $process = Start-Process $OpenMSX -ArgumentList $arguments -PassThru
    if (!$process.WaitForExit(90000)) {
        Stop-Process -Id $process.Id
        throw "openMSX excedeu o tempo no caso $case."
    }
    if ($process.ExitCode -ne 0 -or !(Test-Path (Join-Path $work "$case.vram")) -or
        !(Test-Path (Join-Path $work "$case.palette"))) {
        throw "Falha ao capturar a tela $case."
    }
}
if ((Get-Content (Join-Path $work "logo.log") -Raw).Trim() -ne "PASS") {
    throw "A abertura nao avancou para o diagnostico."
}
$basicPalette = [IO.File]::ReadAllBytes((Join-Path $work "basic.palette"))
$logoPalette = [IO.File]::ReadAllBytes((Join-Path $work "logo.palette"))
if ([Convert]::ToBase64String($basicPalette) -ne [Convert]::ToBase64String($logoPalette)) {
    throw "A paleta difere da referencia BASIC."
}
$reference = [IO.File]::ReadAllBytes((Join-Path $work "basic.vram"))
$actual = [IO.File]::ReadAllBytes((Join-Path $work "logo.vram"))
if ($reference.Length -ne 12288 -or $actual.Length -ne 12288) { throw "Captura VRAM incompleta." }
$different = 0
for ($y = 0; $y -lt 192; ++$y) {
    for ($x = 0; $x -lt 256; ++$x) {
        $pos = (($y -band 248) * 32) + ($x -band 248) + ($y -band 7)
        $mask = 128 -shr ($x -band 7)
        $expected = if ($reference[$pos] -band $mask) { $reference[6144 + $pos] -shr 4 } else { $reference[6144 + $pos] -band 15 }
        $pixel = if ($actual[$pos] -band $mask) { $actual[6144 + $pos] -shr 4 } else { $actual[6144 + $pos] -band 15 }
        if ($pixel -ne $expected) { ++$different }
    }
}
if ($different -gt 19) { throw "A tela difere da referencia em $different pixels (limite: 19)." }
Write-Output "Abertura: PASS, $different pixels diferentes em 49152; diagnostico: PASS"

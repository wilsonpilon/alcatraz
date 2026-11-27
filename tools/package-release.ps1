param(
    [string]$OutputPath
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$version = Get-Content -LiteralPath (Join-Path $root 'version.json') -Raw | ConvertFrom-Json
if (-not $OutputPath) {
    $OutputPath = Join-Path $root "out\ALCATRAZ-$($version.major).$($version.build).zip"
}
if (Test-Path -LiteralPath $OutputPath) {
    throw "O arquivo de destino ja existe: $OutputPath"
}

$files = @(
    'out\alcatraz.com'
    'out\ALCATRAZ.SC5'
    'README.md'
    'MANUAL.md'
    'OUTLINE.md'
    'resume.txt'
    'REFERENCIA_ORIGINAL.md'
    'LICENSE'
    'MSXgl\LICENSE.md'
) | ForEach-Object {
    $file = Join-Path $root $_
    if (-not (Test-Path -LiteralPath $file -PathType Leaf)) {
        throw "Arquivo necessario ausente: $file"
    }
    $file
}

Compress-Archive -LiteralPath $files -DestinationPath $OutputPath
Write-Host "Pacote criado: $OutputPath"

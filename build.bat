@echo off
setlocal
cd /d "%~dp0"
if not exist "MSXgl\tools\build\Node\node.exe" (
  echo MSXgl nao encontrado. Consulte o README.
  exit /b 1
)
"MSXgl\tools\build\Node\node.exe" "tools\build.js" %*
exit /b %errorlevel%

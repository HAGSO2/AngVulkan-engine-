@ECHO OFF
REM Build Everything

ECHO "Building everything..."


@REM PUSHD engine
@REM CALL build.bat
@REM POPD
@REM IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)



REM Engine
make -f "Makefile.engine.windows.mak" all
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

REM Testgame
PUSHD testgame
CALL build.bat
POPD
REM IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)
@make -f "Makefile.testgame.windows.mak" all
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "All assemblies built successfully."
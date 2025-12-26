#!/bin/bash
# Build script for testgame
set echo on

mkdir -p ../bin

# Get a list of all the .cpp files.
cFilenames=$(find . -type f -name "*.cpp")

# echo "Files:" $cFilenames

assembly="testgame"
compilerFlags="-g -std=c++17 -fdeclspec -fPIC" 
# -fms-extensions 
# -Wall -Werror
includeFlags="-Isrc -I../engine/src/"
linkerFlags="-L../bin/ -lengine -Wl,-rpath,."
defines="-D_DEBUG -DKIMPORT"

echo "Building $assembly..."
echo clang++ $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags
clang++ $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags
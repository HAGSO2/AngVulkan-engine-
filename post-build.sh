#!/bin/bash

# Run from root directory!
mkdir -p bin/assets
mkdir -p bin/assets/shaders

echo "Compiling shaders..."

echo "assets/shaders/shader.vert -> bin/assets/shaders/shader.vert.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=vert assets/shaders/shader.vert -o bin/assets/shaders/shader.vert.vert.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "assets/shaders/shader.frag -> bin/assets/shaders/shader.frag.spv"
$VULKAN_SDK/bin/glslc -fshader-stage=frag assets/shaders/shader.frag -o bin/assets/shaders/shader.frag.spv
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "Copying assets..."
echo cp -R "assets" "bin"
cp -R "assets" "bin"

echo "Done."
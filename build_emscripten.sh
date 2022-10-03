#!/bin/bash
set -e
pwd


ARG1=${1:-BUILD}
ARG2=${2:-2.0.32}

if [ $ARG1 = "BUILD" ]; then
    echo "BUILD: docker build:"
    docker build -f BuildWasmDockerfile -t test_xeus_wasm  --build-arg USER_ID=$(id -u) --build-arg EMSCRIPTEN_VER=$ARG2 .
else
    echo "SKIP BUILD:"
fi

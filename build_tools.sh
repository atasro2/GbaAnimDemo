#!/bin/sh
make -C tools/gbafix CXX=${1:-g++}
make -C tools/gbagfx CXX=${1:-g++}
make -C tools/scaninc CXX=${1:-g++}
make -C tools/aif2pcm CXX=${1:-g++}
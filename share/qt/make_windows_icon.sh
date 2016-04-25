#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/lomocoin.png
ICON_DST=../../src/qt/res/icons/lomocoin.ico
convert ${ICON_SRC} -resize 16x16 lomocoin-16.png
convert ${ICON_SRC} -resize 32x32 lomocoin-32.png
convert ${ICON_SRC} -resize 48x48 lomocoin-48.png
convert lomocoin-48.png lomocoin-32.png lomocoin-16.png ${ICON_DST}


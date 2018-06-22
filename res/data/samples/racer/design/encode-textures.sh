#!/bin/sh

GPLAY_ROOT_DIR=../../../../..
TEXTUREC_BIN=$GPLAY_ROOT_DIR/tools/bin/linux/texturec
DIR_OUT=encoded

# create output directory
mkdir -p $DIR_OUT


# convert every .png to .dds compressed BC3 with mipmap
EXT=png
for i in *.$EXT; do
    base=`basename $i .$EXT`
    echo $i
    $TEXTUREC_BIN -f $i -o $DIR_OUT/$base.dds -m -t BC3
done


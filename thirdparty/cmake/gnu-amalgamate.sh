#!/bin/bash

if [ $# -ne 2 ];
then
    echo "Usage: gnu-amalgamate.sh <ar command> <target directory>"
    exit 1
fi

CWD=`pwd`
AR="$1"
TARGET="$2"

if [ ! -e $AR ];
then
    echo "No such file: $AR"
    exit 1
fi

if [ ! -d $TARGET ];
then
    echo "No such target directory: $AR"
    exit 1
fi

echo "Using ar : $AR"
echo "Amalgamating $TARGET"

cd $TARGET
rm -f libgplay-deps.a
mkdir -p tmp
cp *.a ./tmp/

# Build an MRI script file
MRI="$CWD/deps.mri"
echo "create libgplay-deps.a" >$MRI
for file in ./tmp/* ; do
    if [ -e "$file" ];
    then
        echo "addlib $file" >>$MRI
    fi
done
echo "save" >>$MRI
echo "end" >>$MRI

echo "Executing..."
cat $MRI

# Now actually do something
$AR -M < $MRI

# Copy amalgamated lib to output dir
mv libgplay-deps.a ../external-deps/lib/

# Clean up
rm $MRI
rm -rf ./tmp

cd $CWD

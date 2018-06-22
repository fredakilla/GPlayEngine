#!/bin/bash

# encode sponza.fbx to sponza.gpb and create sponza.material 

GPENCODER='../../../../../bin/linux/gameplay-encoder'

$GPENCODER -m sponza.fbx

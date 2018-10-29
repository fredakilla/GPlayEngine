QMAKE_CLEAN += $$DESTDIR/$$TARGET

TEMPLATE = subdirs

CONFIG = ordered

SUBDIRS += \
    src/gplay-engine.pro \
    samples/browser/sample-browser.pro \
    samples/graphics/sample-graphics.pro \
    samples/minimal/sample-minimal.pro \
    samples/spaceship/sample-spaceship.pro \
    samples/racer/sample-racer.pro \
    samples/character/sample-character.pro \
    tools/encoder/gplay-encoder.pro \
    #tools/luagen/gplay-luagen.pro \


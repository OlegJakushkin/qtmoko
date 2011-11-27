TEMPLATE=plugin
TARGET=neoaudiohardware

PLUGIN_FOR=qtopia
PLUGIN_TYPE=audiohardware

CONFIG+=qtopia
QTOPIA+=audio
enable_bluetooth:QTOPIA+=comm

HEADERS		=  neoaudioplugin.h
SOURCES	        =  neoaudioplugin.cpp

conf [
    hint=image
    files=gta04_initial_alsa.state
    path=/etc/alsa
]

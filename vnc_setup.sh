#!/bin/sh
vncserver :1
vncserver  -kill :1
cat vnc_settings.sh > ~/.vnc/xstartup
vncserver :1

export  QTDIR=/opt/Trolltech/
export	PATH=$QTDIR/bin:$PATH
export	QWS_DISPLAY=LinuxFb:/dev/fb0
export	QWS_SIZE=800x480
export  LD_LIBRARY_PATH=$QTDIR/lib
if [ -e "/dev/input/event3" ]; then
		export QWS_MOUSE_PROTO="LinuxInput:/dev/input/event3"
	elif [ -e "/dev/input/event2" ]; then
		export QWS_MOUSE_PROTO="LinuxInput:/dev/input/event2"
	elif [ -e "/dev/input/event1" ]; then
		export QWS_MOUSE_PROTO="LinuxInput:/dev/input/event1"
fi

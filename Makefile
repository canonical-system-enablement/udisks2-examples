CC = gcc

all: query-block-devices detect-hotplug probe-usb-devices

query-block-devices: query-block-devices.c
	$(CC) query-block-devices.c -o query-block-devices `pkg-config --cflags --libs udisks2`

detect-hotplug: detect-hotplug.c
	$(CC) detect-hotplug.c -o detect-hotplug `pkg-config --cflags --libs udisks2`

probe-usb-devices: probe-usb-devices.c
	$(CC) probe-usb-devices.c -o probe-usb-devices `pkg-config --cflags --libs udisks2`

install: query-block-devices detect-hotplug probe-usb-devices
	install -d ${DESTDIR}/bin/
	install -t ${DESTDIR}/bin/ -m755 query-block-devices detect-hotplug probe-usb-devices

clean:
	rm query-block-devices detect-hotplug probe-usb-devices

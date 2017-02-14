CC = gcc

all: query-block-devices detect-hotplug

query-block-devices: query-block-devices.c
	$(CC) query-block-devices.c -o query-block-devices `pkg-config --cflags --libs udisks2`

detect-hotplug: detect-hotplug.c
	$(CC) detect-hotplug.c -o detect-hotplug `pkg-config --cflags --libs udisks2`

clean:
	rm query-block-devices detect-hotplug

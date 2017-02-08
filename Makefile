CC = gcc

all: query-block-devices

query-block-devices: query-block-devices.c
	$(CC) query-block-devices.c -o query-block-devices `pkg-config --cflags --libs udisks2`

clean:
	rm query-block-devices

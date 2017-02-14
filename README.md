# udisks2-examples
Example code for interfacing with udisks2

## Building
Install the udisks development files
```
$ sudo apt install libudisks2-dev
```

Build the source
```
$ make
```

## Running the examples
### query-block-devices
List info about the block devices on your system 
```
$ ./query-block-devices
```

### detect-hotplug
Detect when a device such as a USB thumb drive is plugged into or removed from your system
```
$ ./detect-hotplug
```

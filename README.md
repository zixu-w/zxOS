# zxOS
Bare bone kernel based on tutorial http://wiki.osdev.org/

## How to build
### Pre-requisites
#### Cross-platform Tool-chain
You need a set of cross-platform tools to build the kernel. To build such a tool-chain, you can run `make dependencies`. The tool-chain will be installed at `/usr/local/cross` by default. If you want to install to another place, run
```
export CROSS_PATH=<Your path here>
make dependencies
```
, or `CROSS_PATH=<Your path here> make dependencies` directly.

If you prefer to build a cross-platform tool-chain by yourself, make sure to export its path prefix to `CROSS_PATH` before building the kernel.
#### Other Tools
To build an image of zxOS, you will need to install `xorriso`:
###### Ubuntu/Debian
```
sudo apt-get install xorriso
```
<!-- TODO: commands for other systems -->
And you can also launch zxOS locally in a virtual machine with [`qemu`](https://www.qemu.org/). To install:
###### Ubuntu/Debian
```
sudo apt-get install qemu
```
<!-- TODO: commands for other systems -->
### Build the Kernel
With all prerequisites met, you can build zxOS by simply running
```
make
make install
```
The system image can be built by running
```
make image
```
The generated iso image can be found under `image/`.

To start a qemu virtual machine, run
```
make qemu-run
```

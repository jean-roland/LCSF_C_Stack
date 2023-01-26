## Presentation

LCSF C Stack is a C implementation of the LCSF (Light Command Set Format), designed for, but not limited to, small embedded systems (bare metal friendly, static memory allocation, small footprint <3kB data, 1kB ram).

This adds a software layer to easily encode/decode custom command sets based on LCSF to your project.

## About LCSF

LCSF is a specification to easily create and deploy custom command sets.

For more information on the project, check the official LCSF documentation [here](https://jean-roland.github.io/LCSF_Doc/).

## How to use
First, include all the source files in your project. You can find example set-up code in `custom_main.c`.

Modules must be initialized once in your application before you can use them. Further initialization will only waste memory and using before initializing will result in a bad address fault.

Then, to interface with your project:
* Call the function `LCSF_TranscoderReceive` with your module receiving data in lcsf format (eg: data coming from a network port, an UART or a CAN bus...)
* Give a callback to the function that will send the transcoder data.

Finally, to fit your application's needs, create a custom protocol either by modifying the example protocol files or by using the [LCSF Generator](https://github.com/jean-roland/LCSF_Generator) (recommended).

You can change the memory allocation function used by the stack to use your own or change other parameters (buffer sizes, filo sizes) in `LCSF_config.h`.

## How the stack works

The stack itself is composed of two main files:
* `LCSF_Transcoder.c`: This module is in charge of decoding and encoding message respecting the LCSF format.
* `LCSF_Validator.c`: This module is in charge of verifying if messages correspond to a known command set protocol.

They are accompanied by two support files:
* `Filo.c`: A module that creates memory pool that can be used as filo memory.
* `MemAlloc.c`: An _optional_ module that handles the static memory allocation.

Then the actual protocols are fed into the stack at initialization time.

The following diagram helps understanding how it works:

![LCSF C Stack](./Doc/img/Stack.png)

## Representations

The default LCSF representation used by the transcoder is the standard one. You can switch to the smaller representation by uncommenting the `#define LCSF_SMALL` symbol declaration found in `LCSF_Config.h`

## Protocol files

Each protocol used by the LCSF C stack is composed of 5 files:
* `LCSF_protocol_Desc.h`: This file contains the tables describing the protocol commands and attributes.
* `LCSF_Bridge_protocol.c/.h`: This files contain the LCSF abstraction layer.
* `<protocol>_Main.c/.h`: This files contain the application code associated with the protocol.

## Note on recursivity

Since LCSF is based on nested structures, the stack use recursive functions.

Recursivity is generally frowned upon in embedded applications, which is why the stack is made to limit the issue:
* The number of calls is directly linked to the number of sub-attribute layers in a protocol, that means the user has direct control.
* The stack is linear in its recursivity (one call will only lead to a maximum of one other call).
* The stack is limited by the depth of its filo when making new calls, it is guaranteed to not infinitely loop.

## Build & tests

If you want to build the project as is you need to install [CMake](https://cmake.org/) 3.14 or above, and a buildsystem ([Ninja](https://ninja-build.org/) is recommended).

Build commands, if using Ninja, at project root:
 ```
 cmake -B build/ -G Ninja -DCMAKE_BUILD_TYPE="Release"
 cmake --build build/
 ```

To run the test suite, you need to install [Cpputest](http://cpputest.github.io/). Then, use the following commands still at project root:

`cmake --build build/ --target run_tests`

If you have library errors, you might need to modify the root `CMakeLists.txt` to specify the Cpputest location.

## Resource usage

This section aims at providing information regarding the LCSF_C_Stack resource consumption on an embedded target.

The lcsf stack was tested on a EFM32 leopard gecko dev board with a si446x rf transceiver. Resource usage will differ depending on your target, toolchain and application.

For more information, check the full resource usage report [here](./Resource_usage.md).

The rough values are:
* Program memory: `~6kB`
* Heap memory: `<1kB`
* Stack memory: `~250 bytes`

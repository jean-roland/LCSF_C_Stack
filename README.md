## Presentation

LCSF C Stack is a C implementation of the LCSF (Light Command Set Format), designed for, but not limited to, small embedded systems.

This adds a software layer to easily encode/decode custom command sets/applicative protocols based on LCSF to your project.

## How to use
First, include all the source files in your project. You can find example set-up code in `custom_main.c`.

Modules must be initialized once in your application before you can use them. Further initialization will only waste memory and using before initializing will result in a bad address fault.

Then, to interface with your project:
* Call the function `LCSF_TranscoderReceive` with your module receiving data in lcsf format (eg: data coming from a network port, an UART or a CAN bus...)
* Give a callback to the function that will send the transcoder data.

Finally, to fit your application's needs, create a custom protocol either by modifying the example protocol files or by using the [LCSF Generator](https://github.com/jean-roland/LCSF_Generator) (recommanded).

You can change the memory allocation function used by the stack to use your own or change other parameters (buffer sizes, filo sizes) in `LCSF_config.h`.

## About LCSF

LCSF is a specification to easily create and deploy custom command sets.

For more information, check the official LCSF documentation [here](https://jean-roland.github.io/LCSF_Doc/).

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

## Resource usage

This section aims at providing information regarding the LCSF_C_Stack resource consumption on an embedded target.

The stack was tested on an EFM32 leopard gecko dev board with a si446x rf transceiver. Resource usage will differ depending on your target, toolchain and application.

### Program memory

Program memory usage was evaluated by looking at binary sizes and elf symbol table with and without the stack, using arm-gcc with `-O2` optimization.

The test application included the stack and the protocol used in unit testing which has complex and large commands by design. The results were:

* Lcsf stack: `~3kB`.
* Test protocol: `~2kB` from generated code, `~1kB` from user protocol code.

### Heap usage

The heap memory consumption was measured by looking at heap usage statistics with and without the stack.

LCSF Stack heap usage is mostly due to:
* Transcoder transmit buffer.
* Transcoder receive filo.
* Validator tx and rx filos.
All of which have configurable sizes in `LCSF_Config.h`. There is also the validator protocol array size which is a parameter of `LCSF_ValidatorInit`.

Protocol heap usage is mostly due to:
* Bridge transmit filo
* Bridge receive command payload
* Protocol main transmit command payload
The filo size is a parameter of the bridge init function, the command payload size is equal to the size of the command with the largest payload. It will also vary if the user specific code has heavy use of heap memory.

There is also the static variables in each module of the stack and protocol that consumes a bit of heap memory.

For this test, a 12 items value was used for all filos and a 256 value for the buffer size. The results were:
* LCSF stack heap usage: `632 bytes` (static + alloc)
* Protocol heap usage: `208 bytes` (static + alloc)

### Stack usage

Stack usage was evaluated with the values obtained vith the compilation flag `fstack-usage` and sanity checked by evaluating the stack pointer value at key points during run time.

Those key points are:
* The peak stack usage which should be, since the protocol send a message upon recieving one, in `LCSF_TranscoderSend` with a callstack starting from `LCSF_TranscoderReceive` and crossing the whole stack both ways.
* The stack recursive functions, to monitor the stack cost of each layer.

The results were:
* `LCSF_ValidateAttribute_Rec: 56 bytes/call`
* `LCSF_FillAttribute_Rec: 48 bytes/call`
* `LCSF_DecodeAtt_Rec: 40 bytes/call`
* `LCSF_EncodeAtt_Rec: 24 bytes/call`
* Lcsf stack, compiler measured stack usage: `160 bytes`
* Protocol compiler measured stack usage: `112 bytes`
* Maximum run-time measured stack usage: `340 bytes` because of four nested calls to `LCSF_ValidateAttribute_Rec`

With a simpler protocol, we can expect a maximum of `200-250 bytes` of stack usage.

### Processing time

### Protocol overhead

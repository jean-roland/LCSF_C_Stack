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

The lcsf stack was tested on a EFM32 leopard gecko dev board with a si446x rf transceiver. Resource usage will differ depending on your target, toolchain and application.

### Program memory

Program memory usage was evaluated by looking at binary sizes and elf symbol table with and without the lcsf stack, using arm-gcc with `-O2` optimization.

The test application included the stack and the protocol used in unit testing which has complex and large commands by design. The results were:

* LCSF stack: `2816 bytes`.
* Test protocol: `2130 bytes` from generated code, `1368 bytes` from user protocol code.

### Heap usage

The heap memory consumption was measured by looking at all the static variables and memory allocations of the LCSF stack and verified with heap usage statistics at run time.

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

There is also the static variables in each module of the LCSF stack and protocol that consumes a bit of heap memory.

For this test, the command payload size is `40 bytes`, a 12 items value was used for all filos and a 256 value for the buffer size. This represents `548 bytes` for the LCSF stack, `176 bytes` for the protocol.

The results were:
* LCSF stack heap usage: `632 bytes` (static + alloc),
* Protocol heap usage: `208 bytes` (static + alloc)

### Stack usage

Stack usage was evaluated with the values obtained vith the compilation flag `fstack-usage` and sanity checked by evaluating the stack pointer value at key points during run time.

Those key points are:
* The peak stack usage which should be, since the protocol send a message upon recieving one, in `LCSF_TranscoderSend` with a callstack starting from `LCSF_TranscoderReceive` and crossing the whole LCSF stack both ways.
* The LCSF stack recursive functions, to monitor the stack cost of each layer.

The results were:
* `LCSF_ValidateAttribute_Rec: 56 bytes/call`
* `LCSF_FillAttribute_Rec: 48 bytes/call`
* `LCSF_DecodeAtt_Rec: 40 bytes/call`
* `LCSF_EncodeAtt_Rec: 24 bytes/call`
* LCSF stack non-rec, compiler measured stack usage: `160 bytes`
* Protocol compiler measured stack usage: `112 bytes`

The values for the recursive functions were correct but because of optimization, the value outside of recursive functions was lower than expected with the compiler values (`180 bytes` vs `272 bytes`).

The maximum run-time measured stack usage was `340 bytes` because of four nested calls to `LCSF_ValidateAttribute_Rec`

With a simpler protocol, we can expect a maximum of `200-250 bytes` of stack usage.

### Processing time

Processing time was measured by measuring the value of a timer before and after processing a message, both for decoding and encoding. Measures were repeated 50 times to get a mean. Tests were made with a simple command (no attributes), a large command (10 attributes) and a complex command (4 layers of sub-attributes). The results were:

Decoding:
* Simple command: `40µs @14MHz, 17µs @28MHz`
* Large command: `191µs @14MHz, 98µs @28MHz`
* Complex command: `241µs @14MHz, 121µs @28MHz`

Encoding:
* Simple command: `54µs @14MHz, 28µs @28MHz`
* Large command: `206µs @14MHz, 104µs @28MHz`
* Complex command: `244µs @14MHz, 120µs @28MHz`

Measurements were made using a timer with a `31.25µs` resolution, which implies measurement uncertainty in the `~10µs` range, without accounting for timer inaccuracy.

Actual processing time will be longer, depending on user application code. This values only account for the lcsf stack + bridge processing time.

### Message Sizes

This section evaluates message sizes and protocol overhead. Measurements were made with the same 3 commands used in processing time section. 

Overhead was calculated by assuming that everything but the following blocks are essential data:
* Protocol id
* Command id
* Data payloads

Results are:
* Simple command: `6 bytes`, `2 bytes (33%)` of overhead.
* Large command: `84 bytes`, `42 bytes (50%)` of overhead. Overhead is high because the attributes only have `1-4 bytes` payloads with `4 bytes` of overhead
* Complex command: `65 bytes`, `46 bytes (70.8%)` of overhead, same reason as the large command with the addition that complex attributes are considered pure overhead.

### LCSF Small

All the previous measurements were made with the regular LCSF mode. This section will study the difference brought by using the small mode.

The most important change is message sizes, since that's the main thing differentiating the two modes:
* Simple command: `3 bytes`, `1 byte (33%)` of overhead.
* Large command: `61 bytes`, `21 bytes (34.4%)` of overhead.
* Complex command: `40 bytes`, `23 bytes (57.5%)` of overhead.

Since variable sizes aren't changed, heap and stack memory usage doesn't change at the exception of `LCSF_EncodeAtt_Rec` which uses now `32 bytes` of stack per call, most probably because of compiler optimization.

Program memory usage and processing time does improve a little as the messages are shorter but only on the transcoder side of things.

Decoding process time:
* Simple command: `36µs (-10%) @14MHz, 18µs (-5.9%) @28MHz`
* Large command: `178µs (-6.8%) @14MHz, 89µs (-9.2%) @28MHz`
* Complex command: `218µs (-9.5%) @14MHz, 111µs (-8.3%) @28MHz`

Encoding process time:
* Simple command: `52µs (-3.7%) @14MHz, 26µs (-7.1%) @28MHz`
* Large command: `192µs (-6.8%) @14MHz, 95µs (-8.7%) @28MHz`
* Complex command: `226µs (-7.4%) @14MHz, 114µs (-5%) @28MHz`

Program memory usage:
* LCSF stack: `2652 bytes (-5.8%)`

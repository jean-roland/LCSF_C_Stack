## Resource usage

This file aims at providing information regarding the LCSF_C_Stack resource consumption on an embedded target.

The lcsf stack was tested on a EFM32 leopard gecko dev board with a si446x rf transceiver. Resource usage will differ depending on your target, toolchain and application.

## Program memory

Program memory usage was evaluated by looking at binary sizes and elf symbol table with and without the lcsf stack, using arm-gcc with `-O2` optimization.

The test application included the stack and the protocol used in unit testing which has complex and large commands by design. The results were:

* LCSF stack: `2816 bytes`.
* Test protocol: `2130 bytes` from generated code, `1368 bytes` from user protocol code.

## Heap usage

The heap memory consumption was measured by looking at all the static variables and memory allocations of the LCSF stack and verified with heap usage statistics at run time.

LCSF Stack heap usage is mostly due to:
* Transcoder transmit buffer.
* Transcoder receive filo.
* Validator tx and rx filos.

All of which have configurable sizes in `LCSF_Config.h`. There is also the validator protocol array size which is a parameter of `LCSF_ValidatorInit`.

Protocol heap usage is mostly due to:
* Bridge transmit filo.
* Bridge receive command payload.
* Protocol main transmit command payload.

The filo size is a parameter of the bridge init function, the command payload size is equal to the size of the command with the largest payload. It will also vary if the user specific code has heavy use of heap memory.

There is also the static variables in each module of the LCSF stack and protocol that consumes a bit of heap memory.

For this test, the command payload size is `40 bytes`, a 12 items value was used for all filos and a 256 value for the buffer size. This represents `548 bytes` for the LCSF stack, `176 bytes` for the protocol.

The results were:
* LCSF stack heap usage: `632 bytes` (static + alloc),
* Protocol heap usage: `208 bytes` (static + alloc)

## Stack usage

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

With a simpler protocol, we can expect a maximum of `200-250 bytes` of stack usage, half if encoding and decoding are split in different callstacks.

## Processing time

Processing time was measured by measuring the value of a timer before and after processing a message, both for decoding and encoding. Tests were made with a simple command (no attributes), a large command (10 attributes) and a complex command (4 layers of sub-attributes).

Measurements were made with a processor clock speed at `14MHz` and `28MHz` and a timer at clock speed, which implies measurement uncertainty in the `~10ns` range, without accounting for clock inaccuracy. The following values are the minimum value of 100 measurements to remove variability (irq...):

Decoding:
* Simple command: `36.6µs @14MHz, 18.3µs @28MHz`
* Large command: `195.7µs @14MHz, 97.9µs @28MHz`
* Complex command: `240.1µs @14MHz, 120.0µs @28MHz`

Encoding:
* Simple command: `52.9µs @14MHz, 26.5µs @28MHz`
* Large command: `207.5µs @14MHz, 103.8µs @28MHz`
* Complex command: `245.0µs @14MHz, 122.5µs @28MHz`

Actual processing time will be longer, depending on user application code. This values only account for the lcsf stack + bridge processing time.

## Message Sizes

This section evaluates message sizes and protocol overhead. Measurements were made with the same 3 commands used in processing time section.

Overhead was calculated by assuming that everything but the following blocks are essential data:
* Protocol id
* Command id
* Data payloads

Results are:
* Simple command: `6 bytes`, `2 bytes (33%)` of overhead.
* Large command: `84 bytes`, `42 bytes (50%)` of overhead. Overhead is high because the attributes only have `1-4 bytes` payloads with `4 bytes` of overhead
* Complex command: `65 bytes`, `46 bytes (70.8%)` of overhead, same reason as the large command with the addition that complex attributes are considered pure overhead.

## LCSF Small

All the previous measurements were made with the regular LCSF mode. This section will study the difference brought by using the small mode.

The most important change is message sizes, since that's the main thing differentiating the two modes:
* Simple command: `3 bytes`, `1 byte (33%)` of overhead.
* Large command: `61 bytes`, `21 bytes (34.4%)` of overhead.
* Complex command: `40 bytes`, `23 bytes (57.5%)` of overhead.

Since variable sizes aren't changed, heap and stack memory usage doesn't change at the exception of `LCSF_EncodeAtt_Rec` which uses now `32 bytes` of stack per call, most probably because of compiler optimization.

Program memory usage and processing time does improve a little as the messages are shorter:

Decoding process time:
* Simple command: `34.7µs (-5.2%) @14MHz, 17.4µs (-4.9%) @28MHz`
* Large command: `178.7µs (-8.7%) @14MHz, 89.4µs (-8.7%) @28MHz`
* Complex command: `218.4µs (-9.0%) @14MHz, 109.2µs (-9.0%) @28MHz`

Encoding process time:
* Simple command: `51.2µs (-3.2%) @14MHz, 25.6µs (-3.4%) @28MHz`
* Large command: `194.3µs (-6.4%) @14MHz, 97.1µs (-6.5%) @28MHz`
* Complex command: `231.3µs (-5.6%) @14MHz, 115.6µs (-5.6%) @28MHz`

Program memory usage:
* LCSF stack: `2652 bytes (-5.8%)`

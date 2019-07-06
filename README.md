## Presentation

LCSF C Stack is a C implementation of the LCSF (Light Command Set Format), designed for, but not limited to, small embedded systems.

To use it in your project, include all the source files as well as the application code found in `custom_main`.

To interface it with your project:
* Call the function `LCSF_TranscoderReceive` with your module receiving data in lcsf format (eg: data coming from a network port, an UART or a CAN bus...)
* Give a callback to the function that will send the transcoder data.

Finally, to fit your application's needs, create a custom command set either by modifying the example command set or by using the LCSF Generator (WIP).

## About LCSF

LCSF is a specification to easily create and deploy custom command sets.

For more information, check the official LCSF documentation [here](https://jean-roland.github.io/LCSF_Doc/).

## How the stack works

The stack itself is composed of two main files:
* `LCSF_Transcoder.c`: This module is in charge of decoding and encoding message respecting the LCSF format.
* `LCSF_Validator.c`: This module is in charge of verifying if messages correspond to a known command set protocol.

They are accompanied by two support files:
* `MemAlloc.c`: A module that handles the static memory allocation.
* `Filo.c`: A module that creates memory pool that can be used as filo memory.

Then the actual protocols are fed into the stack at initialization time.

The following diagram helps understanding how it works:

![LCSF C Stack](./Doc/img/Stack.png)

## Note on recursivity

Since LCSF is based on nested structures, the stack use recursive functions. 

Recursivity is generally frowned upon in embedded applications, which is why the stack is made to limit the issue:
* The number of calls is directly linked to the number of sub-attribute layers in a protocol, that means the user has direct control.
* The stack is linear in its recursivity (one call will only lead to a maximum of one other call).
* The stack is limited by the depth of its filo when making new calls, it is guaranteed to not infinitely loop.
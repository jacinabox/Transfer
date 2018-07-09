The FRP/stream processing library that will get you laid(TM) and make FRP
comprehensible to C++ programmers.

Continuation passing style is leveraged to provide fanout of single inputs
to multiple outputs (but the use of CPS is standard by this point).

Some stream processing libraries suffer from space leaks, as the internal
representation of a switching construct grows without bound. This library
reconstructs the stream processor network with every input step, allowing
to produce a compact representation of the internals of that stream
processor network. That is important when a process network switches often.

The speed benefits of C++ are leveraged; this library is designed to work
without garbage collection.

------------------------------------------

The operators >> (right shift) and | (pipe) are overloaded as stream processor
(transfer) constructor wrappers, making for an acceptable syntax to construct/
stream processor (transfer) pipelines. Each basic combinator is its own class
overriding Transfer base class. The Transfer base class is parameterized on
input and output types, giving it the structure of a category. The classes
ComposeTransfer and IdentityTransfer reconstruct the composition and identity
operations of the category.

------------------------------------------

The file Transfer.cpp compiles on Visual Studio Express. It has an example
program  as described in the code.

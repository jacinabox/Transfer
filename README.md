The FRP/stream processing library that makes arrowized FRP comprehensible to C++ programmers.

Transfers support fanout of a single input to multiple outputs (or none).
The decision to allow an input to result in no outputs, can result in
non-productive stream processor definitions, but gives increased
expressive power; transfers never have to deal with empty inputs
from their upstream transfers.

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

------------------------------------------

Summary of combinators (assume that :-> is a type operator corresponding to
the transfer category). The basic combinators are as follows:

* map :: (t-\>u) -\> (t :-> u)

* filter :: (t-\>Bool) -\> (t :-> t)

* switch\_once :: (t -\> Maybe(t :-> u), t :-> u) -\> (t :-> u)

'Maybe' is the type constructor of a container, that may or may not contain
a single value. This combinator is based on the corresponding switching
combinator of Yampa. 'switch\_once' switches its internal transfer lazily.

* scanning :: (t,(t,u)-\>t) -\> (u :-> t)

This combinator is derivable from the other combinators.

* with\_remainder :: (t :-> u) -\> (t :-> (t :-> u, u))

'with\_remainder' is for making a copy of the present state of the internal
transfer. Such copy is used to resume the same transfer later. Resuming
also redoes any effects performed inside the transfer, from the point where
the transfer is suspended.

* identity :: t :-> t

* empty :: t :-> u

* feedback :: (t :-> t) -\> (t :-> t)

Feedback has semantics of the following stream processor f, where f2
is the stream processor associated with the argument transfer:

let f f2 (x:xs) =
	let xs' = f2 x in
		xs' ++ f f2(xs'++xs)

let f f2 [] = []

* lazy :: (() -\> (t :-> u)) -\> (t :-> u)

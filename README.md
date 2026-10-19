# Eidos IDL

Work-in-progress IDL for describing a minimally sufficient binary protocol. The format does not carry any metadata,
relies on the sender and the receiver having matching message descriptions and is intentionally lightweight to allow
the user to build their own framing rules.

It is implemented as a pure C99 library without any mandatory dependencies, **including `libc`**.

Native IP address utilities for node.js
=======================================

Installation
------------

    npm install advertine/node-ip-native

Usage
-----

```js
    ip = require('ip-native')

    ip.toBuffer(ip) -> Buffer
    ip.toBuffer(ip, buffer) -> buffer
    ip.toBuffer(ip, buffer, offset) -> buffer

    ip.toNumber(ip) -> number or string

    ip.toString(ip) -> string

```


Notes
-----

This module was tested on Linux (x64), OS X, FreeBSD and MS Windows (x64 and x86) with
node 0.8, 0.10, 0.11, 0.12 and io.js.

Licence
-------

MIT

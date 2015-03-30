Native IP address utilities for node.js
=======================================

[![Build Status][BS img]][Build Status]

Installation
------------

    npm install advertine/node-ip-native

Usage
-----

```js
    var ip = require('ip-native')

    ip.toBuffer('127.0.0.1') // -> <Buffer a7 00 00 01>
    var buffer = new Buffer(4);
    ip.toBuffer('10.11.0.1', buffer) // -> 4
    buffer // -> <Buffer 0a 0b 00 01>
    var buffer = new Buffer(20);
    ip.toBuffer('2a00::1234', buffer, 4) // -> 6
    buffer.slice(4) // -> <Buffer 2a 00 00 00 00 00 00 00 00 00 00 00 00 00 12 34>

    ip.toNumber(ip) -> number or string

    ip.toString(buffer) -> string
    ip.toString(buffer, offset) -> string
    ip.toString(buffer, offset, length) -> string

```


Notes
-----

This module was tested on Linux (x64), OS X, FreeBSD and MS Windows (x64 and x86) with
node 0.8, 0.10, 0.11, 0.12 and io.js.

Licence
-------

MIT

[Build Status]: https://travis-ci.org/advertine/node-ip-native
[BS img]: https://travis-ci.org/advertine/node-ip-native.svg

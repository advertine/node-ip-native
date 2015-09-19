var test = require("tap").test
  , ip = require('..')
  ;

test("should have functions", function(t) {
  t.type(ip.toBuffer, 'function');
  t.type(ip.toNumber, 'function');
  t.type(ip.toString, 'function');
  t.end();
});


test("should convert ip to buffer", function(t) {
  t.deepEqual(ip.toBuffer('0.0.0.0'), new Buffer([0,0,0,0]));
  t.deepEqual(ip.toBuffer('127.1.2.3'), new Buffer([127,1,2,3]));
  t.deepEqual(ip.toBuffer('255.255.255.255'), new Buffer([255,255,255,255]));
  t.deepEqual(ip.toBuffer('::ffff:8.255.254.0'), new Buffer([8,255,254,0]));
  t.deepEqual(ip.toBuffer('::ffff:ffff:ffff'), new Buffer([255,255,255,255]));
  t.deepEqual(ip.toBuffer('::'), new Buffer('00000000000000000000000000000000', 'hex'));
  t.deepEqual(ip.toBuffer('1234::abcd'), new Buffer('1234000000000000000000000000abcd', 'hex'));
  t.deepEqual(ip.toBuffer('1234:5678:9abc:def0:4321:8765:cba9:0fed'), new Buffer('123456789abcdef043218765cba90fed', 'hex'));
  t.deepEqual(ip.toBuffer('ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff'), new Buffer('ffffffffffffffffffffffffffffffff', 'hex'));
  t.deepEqual(ip.toBuffer('ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255'), new Buffer('ffffffffffffffffffffffffffffffff', 'hex'));
  t.end();
});

test("should not convert ip to buffer", function(t) {
  t.strictEqual(ip.toBuffer(''), null);
  t.strictEqual(ip.toBuffer('1.2.3.4.5'), null);
  t.strictEqual(ip.toBuffer(':::'), null);
  t.strictEqual(ip.toBuffer('foo'), null);
  t.strictEqual(ip.toBuffer(-1), null);
  t.strictEqual(ip.toBuffer([]), null);
  t.strictEqual(ip.toBuffer({}), null);
  t.strictEqual(ip.toBuffer(true), null);
  t.strictEqual(ip.toBuffer(false), null);
  t.strictEqual(ip.toBuffer(null), null);
  t.strictEqual(ip.toBuffer(void(0)), null);
  t.strictEqual(ip.toBuffer('ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.2555'), null);
  t.end();
});

test("should convert ip to argument buffer", function(t) {
  var buffer = new Buffer(20);
  buffer.fill(0);
  t.strictEqual(ip.toBuffer('0.0.0.0', buffer), 4);
  t.deepEqual(buffer.slice(0, 4), new Buffer([0,0,0,0]));
  t.strictEqual(ip.toBuffer('127.1.2.3', buffer, 4), 4);
  t.deepEqual(buffer.slice(0, 8), new Buffer([0,0,0,0,127,1,2,3]));
  t.strictEqual(ip.toBuffer('255.255.255.255', buffer, 8), 4);
  t.deepEqual(buffer.slice(0, 12), new Buffer([0,0,0,0,127,1,2,3,255,255,255,255]));
  t.strictEqual(ip.toBuffer('::ffff:8.255.254.0', buffer, 12), 4);
  t.deepEqual(buffer.slice(0, 16), new Buffer([0,0,0,0,127,1,2,3,255,255,255,255,8,255,254,0]));
  t.strictEqual(ip.toBuffer('::ffff:ffff:ffff', buffer, -22), 4);
  t.deepEqual(buffer.slice(0, 16), new Buffer([255,255,0,0,127,1,2,3,255,255,255,255,8,255,254,0]));

  t.strictEqual(ip.toBuffer('::', buffer), 6);
  t.deepEqual(buffer, new Buffer('0000000000000000000000000000000000000000', 'hex'));
  t.strictEqual(ip.toBuffer('1234::abcd', buffer), 6);
  t.deepEqual(buffer, new Buffer('1234000000000000000000000000abcd00000000', 'hex'));
  t.strictEqual(ip.toBuffer('1234:5678:9abc:def0:4321:8765:cba9:0fed', buffer, 4), 6);
  t.deepEqual(buffer, new Buffer('12340000123456789abcdef043218765cba90fed', 'hex'));
  t.end();
});

test("should convert ip to number", function(t) {
  t.strictEqual(ip.toNumber('0.0.0.0'), 0);
  t.strictEqual(ip.toNumber('127.1.2.3'), 0x7f010203);
  t.strictEqual(ip.toNumber('255.255.255.255'), 0xffffffff);
  t.strictEqual(ip.toNumber('255.255.255.254'), 0xfffffffe);
  t.strictEqual(ip.toNumber('::ffff:8.255.254.0'), 0x08fffe00);
  t.strictEqual(ip.toNumber('::ffff:ffff:ffff'), 0xffffffff);
  t.strictEqual(ip.toNumber('::'), '0');
  t.strictEqual(ip.toNumber('::0123:abcd'), String(0x0123abcd));
  t.strictEqual(ip.toNumber('ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255'), '340282366920938463463374607431768211455');
  t.strictEqual(ip.toNumber('ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff'), '340282366920938463463374607431768211455');
  t.strictEqual(ip.toNumber('ffff:ffff:ffff:ffff:ffff:ffff:ffff:fffe'), '340282366920938463463374607431768211454');
  t.end();
});

test("should not convert ip to number", function(t) {
  t.strictEqual(ip.toNumber(''), null);
  t.strictEqual(ip.toNumber('1.2.3.4.5'), null);
  t.strictEqual(ip.toNumber(':::'), null);
  t.strictEqual(ip.toNumber('foo'), null);
  t.strictEqual(ip.toNumber(-1), null);
  t.strictEqual(ip.toNumber([]), null);
  t.strictEqual(ip.toNumber({}), null);
  t.strictEqual(ip.toNumber(true), null);
  t.strictEqual(ip.toNumber(false), null);
  t.strictEqual(ip.toNumber(null), null);
  t.strictEqual(ip.toNumber(void(0)), null);
  t.end();
});

test("should convert buffer to ip string", function(t) {
  t.strictEqual(ip.toString(new Buffer([0, 0, 0, 0])), '0.0.0.0');
  t.strictEqual(ip.toString(new Buffer([0, 0, 127,1,2,3]), 2), '127.1.2.3');
  t.strictEqual(ip.toString(new Buffer([0, 0, 255,255,255,255, 0, 0]), 2, 4), '255.255.255.255');
  t.strictEqual(ip.toString(new Buffer([8,255,254,0])), '8.255.254.0');
  t.strictEqual(ip.toString(new Buffer([255,255,255,255])), '255.255.255.255');
  t.strictEqual(ip.toString(new Buffer('00000000000000000000000000000000', 'hex')), '::');
  t.strictEqual(ip.toString(new Buffer('000000000000000000000000abcd1234', 'hex')), '::171.205.18.52');
  t.strictEqual(ip.toString(new Buffer('00000000000000000000ffffabcd1234', 'hex')), '::ffff:171.205.18.52');
  t.strictEqual(ip.toString(new Buffer('1234000000000000000000000000abcd', 'hex')), '1234::abcd');
  t.strictEqual(ip.toString(new Buffer('123456789abcdef043218765cba90fed', 'hex')), '1234:5678:9abc:def0:4321:8765:cba9:fed');
  t.end()
});

test("should not convert ip to string", function(t) {
  t.strictEqual(ip.toString(new Buffer(0)), null);
  t.strictEqual(ip.toString(new Buffer(5)), null);
  t.strictEqual(ip.toString(new Buffer(1)), null);
  t.strictEqual(ip.toString(new Buffer(15)), null);
  t.strictEqual(ip.toString(new Buffer(16), 1), null);
  t.strictEqual(ip.toString(new Buffer(4), 1, 4), null);
  t.strictEqual(ip.toString(new Buffer(6), 1, 16), null);
  t.end();
});

// Copyright (c) 2011-2013 LG Electronics, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

var assert = require('assert');
var Buffer = require('buffer').Buffer;

assert.equal(14, Buffer.byteLength('Il était tué'));
assert.equal(14, Buffer.byteLength('Il était tué', 'utf8'));
assert.equal(24, Buffer.byteLength('Il était tué', 'ucs2'));
assert.equal(12, Buffer.byteLength('Il était tué', 'ascii'));
assert.equal(12, Buffer.byteLength('Il était tué', 'binary'));

var long_string = 'long string';

for (var i = 0; i < 1024*1024; i++) {
    long_string += ' longer ';
}

var start = process.uptime();

var len1 = Buffer.byteLength(long_string, 'binary');
var len2 = Buffer.byteLength(long_string, 'ascii');
var len3 = Buffer.byteLength(long_string, 'utf8');

var end = process.uptime();

var time = (end - start) * 1000;
console.log('time: ' + time);
console.log('memUsage: ' + JSON.stringify(process.memoryUsage()));
console.log('len1: ' + len2 + ' len2: ' + len2 + ' len3: ' + len3);

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

#ifdef RUNTIME_SAMPLE
  #error Macro name conflict
#endif

#ifdef RUNTIME_SAMPLE_DEFINITION
// variable definition
#define RUNTIME_SAMPLE(name) Sample Runtime::k##name = { 0, 0, 0 };
#else
  // variable declaration & inline function definition
  #define RUNTIME_SAMPLE(name)                            \
      private:                                            \
        static Sample k##name;                            \
      public:                                             \
        static Sample name() { return k##name; }          \
        static void mark_##name()                         \
        {                                                 \
          (k##name).uptime = Platform::GetUptime();       \
          Platform::GetMemory(&((k##name).rss),           \
                        &((k##name).vsize));              \
      }
#endif

// when does main() start
RUNTIME_SAMPLE(enterMain)

// the cost up to whatever main did before initializing v8
RUNTIME_SAMPLE(beforeV8Init)

// the cost up to initializing v8
RUNTIME_SAMPLE(afterV8Init)

// the cost up to initializing nodejs before the actual node.js script is loaded & run
RUNTIME_SAMPLE(beforeNodejsInit)

// the cost up-to the point we call into the javascript
RUNTIME_SAMPLE(afterNodejsInit)

// the cost up to the time we entered the event loop
RUNTIME_SAMPLE(enterLoop)

// the cost up to the last time we entered DoPoll
RUNTIME_SAMPLE(enterPoll);

RUNTIME_SAMPLE(enterDonePoll);

// the cost up to the last time an async I/O operation finished
RUNTIME_SAMPLE(ioFinished);

// the cost up to the last Tick we got
RUNTIME_SAMPLE(enterTick)

// the cost up to after node.js finished execution (presumably this includes
// the costs of running the specified script)
RUNTIME_SAMPLE(afterFinished)

#undef RUNTIME_SAMPLE



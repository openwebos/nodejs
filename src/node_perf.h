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

#ifndef NODE_PERF_H
#define NODE_PERF_H

#include <string>
#include <stdio.h>
#include "platform.h"

namespace node {
  namespace profiler {
    // controlled through environment variables NODE_PROFILE
    extern bool kProfileNode;

    struct Sample {
      int64_t uptime;
      size_t rss;
      size_t vsize;
    };

    class Runtime {
#include "samples.inl"
    };
#define TRACE_PERF(name) ::node::profiler::Runtime::mark_##name()
#define SAMPLE(name) ::node::profiler::Runtime::##name()

    class Memory {
    public:
      Memory()
      {
        mark();
      }

      void mark()
      {
        if (!kProfileNode)
          return;
        mark(m_currentRss);
      }

      void delta(const std::string& msg, bool reset = true);

      size_t delta() const
      {
        if (!kProfileNode)
          return 0;

        size_t current;
        mark(current);
        return current - m_currentRss;
      }

      struct Printable {
        std::string units;
        double amount;
      };

      static Printable memoryUsage(size_t bytes)
      {
        Printable result;
        result.units = adjust(bytes, &result.amount);
        return result;
      }

    private:
      static std::string adjust(size_t bytes, double *adjustedPtr = NULL);

      static void mark(size_t &rss)
      {
        if (0 != Platform::GetMemory(&rss, NULL))
          rss = 0;
      }

      size_t m_currentRss;
    };

    class CPU {
    public:
      CPU()
      {
        mark();
      }

      void mark()
      {
        if (!kProfileNode)
          return;
        mark(m_ts);
      }

      void elapsed(const std::string msg, bool reset = true)
      {
        if (!kProfileNode)
          return;

        int64_t dtNs = elapsedNs();
        double dtMs = dtNs / 1000000.0;

        fprintf(stderr, "Took %.3lfms to %s\n", dtMs, msg.c_str());
        if (reset)
          mark();
      }

      int64_t elapsedNs() const
      {
        int64_t now;
        mark(now);

        return now - m_ts;
      }

    private:
      static void mark(int64_t &ts)
      {
        ts = Platform::GetUptime(true);
      }

      int64_t m_ts;
    };

    // combines memory & cpu into 1 line output
    class CPUMemory {
    public:
      CPUMemory()
      {
        // the individual instanses already do a mark in their constructors
      }

      void mark()
      {
        if (!kProfileNode)
          return;

        m_mem.mark();
        m_cpu.mark();
      }

      void dump(const std::string &msg, bool reset = true)
      {
        if (!kProfileNode)
          return;

        int64_t dtNs = m_cpu.elapsedNs();
        size_t dMemory = m_mem.delta();

        double dtMs = dtNs / 1000000.0;

        Memory::Printable printable = Memory::memoryUsage(dMemory);

        fprintf(stderr, "Took %.3lfms and %.1lf%s to %s\n", dtMs, printable.amount, printable.units.c_str(), msg.c_str());

        if (reset)
          mark();
      }

    private:
      Memory m_mem;
      CPU m_cpu;
    };
  }
}

#define PROFILER_NOOP do { } while(0)

#define PROFILE_NODEJS 1
#if PROFILE_NODEJS
#define PROFILER_CREATE(type, varname)        ::node::profiler::type varname
#define PROFILER_CMCREATE(varname)            PROFILER_CREATE(CPUMemory, varname)
#define PROFILER_CCREATE(varname)             PROFILER_CREATE(CPU, varname)
#define PROFILER_MCREATE(varname)             PROFILER_CREATE(Memory, varname)
#define PROFILER_STAT(varname, funcname, msg) varname.funcname(msg)
#define PROFILER_CMSTAT(varname, msg)         PROFILER_STAT(varname, dump, msg)
#define PROFILER_CSTAT(varname, msg)          PROFILER_STAT(varname, elapsed, msg)
#define PROFILER_MSTAT(varname, msg)          PROFILER_STAT(varname, delta, msg)
#else
#define PROFILER_CREATE(type, varname)        PROFILER_NOOP
#define PROFILER_CMCREATE(varname)            PROFILER_NOOP
#define PROFILER_CCREATE(varname)             PROFILER_NOOP
#define PROFILER_MCREATE(varname)             PROFILER_NOOP
#define PROFILER_STAT(varname, funcname, msg) PROFILER_NOOP
#define PROFILER_CMSTAT(varname, msg)         PROFILER_NOOP
#define PROFILER_CSTAT(varname, msg)          PROFILER_NOOP
#define PROFILER_MSTAT(varname, msg)          PROFILER_NOOP
#endif

// don't allow PROFILE_NODEJS to be used.  Instead use the functions exposed here
// (or add functions) to accomplish compile-time selection
#undef PROFILE_NODEJS

#endif // NODE_PERF_H

// Copyright (c) 2010-2013 LG Electronics, Inc.
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

#include "node_perf.h"
#include <string.h>
#include <stdlib.h>

namespace node {
    namespace profiler {
#define RUNTIME_SAMPLE_DEFINITION
#include "samples.inl"
#undef RUNTIME_SAMPLE_DEFINITION

        static bool isProfilingEnabled()
        {
          const char *setting = getenv("NODE_PROFILE");
          return setting != NULL && (
            strcmp(setting, "1") == 0 ||
            strcasecmp(setting, "true") == 0 ||
            strcasecmp(setting, "y") == 0 ||
            strcasecmp(setting, "yes") == 0
          );
        }

        bool kProfileNode = isProfilingEnabled();

        void Memory::delta(const std::string& msg, bool reset)
        {
            if (!kProfileNode)
              return;

            size_t dMemory = delta();
            double memoryUsed;
            std::string units = adjust(dMemory, &memoryUsed);
            fprintf(stderr, "%s took up %.3lf %s\n", msg.c_str(), memoryUsed, units.c_str());

            if (reset)
                mark();
        }

        std::string Memory::adjust(size_t mb, double *adjustedPtr)
        {
            char unitStr[3];
            const char *unit = " kMG";
            char *str;
            double adjusted = mb;
            while (adjusted > 1024 && *unit) {
                unit++;
                adjusted /= 1024;
            }
            if (!*unit) {
                unit--;
                adjusted *= 1024;
            }

            if (adjustedPtr) *adjustedPtr = adjusted;

            if (*unit != ' ') {
                unitStr[0] = *unit;
                str = unitStr;
            } else {
                str = unitStr + 1;
            }
            unitStr[1] = 'B';
            unitStr[2] = '\0';

            return str;
        }
    }
}

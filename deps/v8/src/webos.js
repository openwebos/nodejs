// Copyright (c) 2012-2013 LG Electronics, Inc.
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


// Purpose: [for internal use only] Recursively calls ObjectFreeze on every
// property in the object tree i.e. freeze the object tree, not just the root
// level object.
function WebOsFreezeObject(obj) {
  // The following implementation is imported from ObjectFreeze() and optimized
  // here to do recursive freezing (without the recursion).

  // Note: Currently, to deal with potential internal circular references, we
  // have the IsExtensible() check.  If the object is not extensible, then it's
  // already frozen and we need not process it again.  This prevents us from
  // getting into infinite loops while processing those circular references.
  //    This generic approach is not correct in the general case where someone
  // can seal an object (i.e. not frozen) and insert it into a tree that we're
  // later told to freeze.  However, since this API is for internal use only,
  // we know that we won't ever encounter that.

  if (!IS_SPEC_OBJECT(obj)) {
    return obj;
  }

  // Pre-populate the queue with the root object:
  var j = 1;
  var queue = [ obj ];

  // Process the queue until there's nothing left to process:
  for (var i = 0; i < j; i++) {
    var curr = queue[i];
    var extensible = %IsExtensible(curr);
    if (!extensible) continue; // Already frozen.  Move on to the next.

    // Add all the properties of the current object to the queue to be
    // processed next:
    var names = ObjectGetOwnPropertyNames(curr);
    for (var k = 0; k < names.length; k++) {
      name = names[k];

      // Freeze the properties i.e. make them read only and not deletable:
      %FreezeProperty(curr, name);

      // Add the property object to the queue if appropriate:
      child = curr[name];
      if (IS_SPEC_OBJECT(child)) {
        queue[j++] = child;
      }
    }

    // Prevent extensions on the current object:
    // Inlining ObjectPreventExtension(curr) here:
    // Note: The IS_SPEC_OBJECT check is already ensured above.
    %PreventExtensions(curr);
  }

  return obj;  
}


// Purpose: Loads the specified resource from the specified url.
function PalmGetResource(fileurl, args) {
  // Trim of the leading "file://" in the file url:
  if (fileurl.indexOf("file:\/\/") === 0) {
    fileurl = fileurl.substr(7);
  }

  var result;
  var is_const;
  var is_json;

  // Check if the args is a string of attributes:
  if (IS_STRING(args)) {
    var i, arg;
    args = args.split(/\s+/g);

    for (i = 0; i < args.length; i++) {
      arg =  args[i];
      if (arg === 'const') {
        is_const = true;
      } else if (arg === 'json') {
        is_json = true;
      }
    }
  }

  result = %WebOsFileLoadIntoString(fileurl);

  // Parse it into a json file if requested:
  if (is_json) {
    result = JSONParse(result);

    // Only freeze the object at the end:
    if (is_const) {
      result = WebOsFreezeObject(result);
    }
  }

  return result;
}

function SetupWebOsGlobals() {
  // Setup non-enumerable function on the global object.
  InstallFunctions(global, DONT_ENUM, $Array(
    // Insert future native webOS functions here as comma separated
    // name, function pairs.  Note: the last entry should not have a
    // trailing comma.
    "palmGetResource", PalmGetResource
  ));
}


SetupWebOsGlobals();

#!/usr/bin/env python2
import pyrb

ruby_script_array="""
    def make_an_array()
        array = []
        3.times { |i| array.push(i) }
        array.push(3.14159265)
        return array
    end

    make_an_array()
"""

ruby_script_exception="""
    raise "This Exception was raised in Ruby"
"""


print("\n- Executing this Ruby code:")
print(ruby_script_array)

list_from_ruby = pyrb.eval(ruby_script_array)

print("\n- The value returned by Ruby to Python is (Ruby returns the value of the last expression by default):")
print("\n    {}".format(list_from_ruby))
print("\n\n- Executing this Ruby code:")
print(ruby_script_exception)

try:
    pyrb.eval(ruby_script_exception)
except Exception as exc:
    print("\n- Caught the exception raised from Ruby in a Python try/catch block!  Re-raising for traceback...\n")
    raise exc

#!/usr/bin/env python

import pyrb


print("This resulted from Python code; assigning Ruby heredoc to a var and eval'ing it...")
print


ruby_script="""
puts "This is pure Ruby from within the Python environment."
puts
puts "Raising a Ruby exception..."
raise "Testing_Ruby_Exception_to_Python_Exception"
"""


pyrb.eval(ruby_script)

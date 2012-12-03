Pyruby module
=============

- A Python module that provides native execution of Ruby code
  	from directly within the Python interpreter's environment.
 
  Note: return value of pyrb.eval is a Python-ized Ruby object
 	(Ruby arrays become Python lists, primitive exception
    conversion is done, nil becomes None, etc.)
 
 
Usage:
-----

import pyrb



pyrb.eval_s("Any valid Ruby code can be passed as a Python string.")
 
pyrb.eval_f("RubyScriptFile.rb")

pyrb.eval("If this filename exists, same as eval_f; else same as eval_s")

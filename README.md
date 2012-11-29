Pyruby module
=============

 * 	A Python module that provides native execution of Ruby code
  	from directly within the Python interpreter's environment.
 
 * 	Note: return value of pyrb.eval is a Python-ized Ruby object
  	(Ruby arrays become Python lists, nil becomes None, etc.)
 
 
 * Usage:
 
   import pyrb
 
   # Execute a string as Ruby code
   pyrb.eval_s("Any valid Ruby code can be passed as a Python string.")
 
  # Execute a file as a Ruby script
  pyrb.eval_f("RubyScriptFile.rb")

  # Just evaluate the parameter based on context (existing file or string code)
  pyrb.eval("If this filename exists, same as eval_f; else same as eval_s")



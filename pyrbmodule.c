#include <Python.h>
#include <ruby.h>


#define RBSTR2PYSTR(rbstr) Py_BuildValue("s", RSTRING_PTR(rbstr))

#define RB_GET_ERRSTR() rb_funcall(rb_errinfo(), id_to_s, 0)
#define RB_GET_ERRINFO() rb_funcall(rb_errinfo(), id_inspect, 0)

static PyObject *pyrb_eval_s(PyObject *, PyObject *);
static PyObject *pyrb_eval(PyObject *, PyObject *);

static ID id_to_s;
static ID id_inspect;
static int rb_needs_init = 1;



static PyMethodDef pyrb_methods[] = {
	{ "eval_s", pyrb_eval_s, METH_VARARGS,
	  "Evaluate a Python string as Ruby code." },
	{ "eval", pyrb_eval, METH_VARARGS,
	  "Evaluate a Python object using the Ruby interpreter." },

//	{ NULL, NULL, 0, NULL },
};


int main(int argc, char **argv, char **env)
{
	printf("Success! ... but for how long? Dum-dum-DUM!\n");
	return 0;
}


static PyObject *pyrb_eval_cstr(PyObject *self, char *arg)
{
	const char *rb_code = (const char *)arg;
	int state = 0;
	VALUE rb_ret;

	rb_ret = rb_funcall(rb_eval_string_protect(rb_code, &state), id_inspect, 0);
	if (state)
		return RBSTR2PYSTR( RB_GET_ERRINFO() );
	else
		return RBSTR2PYSTR(rb_ret);
	//	Py_BuildValue("s", RBSTR2PYSTR(rb_ret));
}


static PyObject *pyrb_eval_s(PyObject *self, PyObject *args)
{
	const char *rb_code;

	if (! PyArg_ParseTuple(args, "s", &rb_code) )
		return NULL;
	else
		return pyrb_eval_cstr(self, (char *)rb_code);
}


static PyObject *pyrb_eval_f(PyObject *self, PyObject *args)
{
	char *rb_code;
	VALUE rb_f_contents;
	
	if (! PyArg_ParseTuple(args, "s", &rb_code) )
		return NULL;

	rb_f_contents = rb_funcall(rb_cFile, rb_intern("read"), 1, rb_str_new2(rb_code));
	rb_code = RSTRING_PTR(rb_funcall(rb_f_contents, id_to_s, 0));
	return pyrb_eval_cstr(self, rb_code);
}


/*
 * Runs contents of the passed string if a file by that name exists,
 * otherwise runs the passed string as Ruby code.
 */
static PyObject *pyrb_eval(PyObject *self, PyObject *args)
{
	char *str;

	if (! PyArg_ParseTuple(args, "s", &str) )
		return NULL;

	// Check if this is an existing filename & run file contents if so
	if (RTEST( rb_funcall(rb_cFile, rb_intern("exists?"), 1, rb_str_new2(str)) ))
		return pyrb_eval_f(self, args);
	else
		return pyrb_eval_s(self, args);
}


PyMODINIT_FUNC initpyrb()
{
	ruby_init();
	ruby_init_loadpath();
	ruby_set_argv(0, NULL);
	rb_needs_init = 0;

	id_to_s = rb_intern("to_s");
	id_inspect = rb_intern("inspect");

	Py_INCREF(Py_None);
	(void)Py_InitModule("pyrb", pyrb_methods);
}


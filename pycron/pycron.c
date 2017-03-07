#include "Python.h"
#include "cron/cronexpr.h"

/* add interface next_cronexpr_time for python module */
time_t next_cronexpr_time(const char* expr, time_t baseTime)
{
	cronexpr_t * cron = cronexpr_create(expr);
	if(cron)
	{
		time_t next = cronexpr_after(cron, baseTime);
		cronexpr_destroy(cron);
		return next;
	}
	return 0;
}

/* add interface last_cronexpr_time for python module */
time_t last_cronexpr_time(const char* expr, time_t baseTime)
{
	cronexpr_t * cron = cronexpr_create(expr);
	if(cron)
	{
		time_t last = cronexpr_before(cron, baseTime);
		cronexpr_destroy(cron);
		return last;
	}
	return 0;
}

/* python wrapper start */
static PyObject* next_cronexpr_time_wrapper(PyObject *self, PyObject *args)
{
	char* expr;
	time_t base_time;

	PyArg_ParseTuple(args, "sl" , &expr, &base_time);
	time_t time = next_cronexpr_time(expr, base_time);
	PyObject* resultObject = Py_BuildValue("l", time);
	return resultObject;
}

static PyObject* last_cronexpr_time_wrapper(PyObject *self, PyObject *args)
{
	char* expr;
	time_t base_time;

	PyArg_ParseTuple(args, "sl" , &expr, &base_time);
	time_t time = last_cronexpr_time(expr, base_time);
	PyObject* resultObject = Py_BuildValue("l", time);
	return resultObject;
}

static PyMethodDef cron_methods[] =
{
	{"next_cronexpr_time" , next_cronexpr_time_wrapper, METH_VARARGS, "next_cronexpr_time(expr, baseTime)"},
	{"last_cronexpr_time" , last_cronexpr_time_wrapper, METH_VARARGS, "last_cronexpr_time(expr, baseTime)"},
	{NULL , NULL}
};

void initpycron(void)
{
	Py_InitModule("pycron", cron_methods);
}


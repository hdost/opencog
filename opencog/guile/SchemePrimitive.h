/*
 * SchemePrimitive.h
 *
 * Allow C++ code to be invoked from scheme -- 
 * by creating a new scheme primitive function.
 *
 * Copyright (C) 2009 Linas Vepstas
 */

#ifdef HAVE_GUILE

#ifndef _OPENCOG_SCHEME_PRIMITIVE_H
#define _OPENCOG_SCHEME_PRIMITIVE_H

#include <opencog/atomspace/Handle.h>
#include <opencog/guile/SchemeSmob.h>
#include <libguile.h>

namespace opencog {

class PrimitiveEnviron
{
	friend class SchemeEval;
	private:
		static bool is_inited;
		static void init(void);

		static SCM do_call(SCM, SCM);
		static PrimitiveEnviron *verify_pe(SCM, const char *);

	protected:
		void do_register(const char *, int);
		virtual SCM invoke (SCM) = 0;
	public:
		virtual ~PrimitiveEnviron();
		virtual const char *get_name(void) = 0;
};

template<class T>
class SchemePrimitive : public PrimitiveEnviron
{
	private:
		Handle (T::*method)(Handle);
		T* that;
		const char *scheme_name;
		enum 
		{
			H_H,
		} signature;

		virtual SCM invoke (SCM args)
		{
			SCM rc = SCM_EOL;
			switch (signature)
			{
				case H_H:
				{
					Handle h = SchemeSmob::verify_handle(scm_car(args), scheme_name);
					Handle rh = (that->*method)(h);
					rc = SchemeSmob::handle_to_scm(rh);
					break;
				}
				default:
					printf ("Error! Unsupported signature: %d\n", signature);
			}
			return rc;
		}
	public:
		SchemePrimitive(const char *name, Handle (T::*cb)(Handle), T *data)
		{
			that = data;
			method = cb;
			scheme_name = name;
			signature = H_H;
			do_register(name, 1);
		}
		virtual const char *get_name(void) { return scheme_name; }
};

template<class T>
inline void declare(const char *name, Handle (T::*cb)(Handle), T *data)
{
	// Note: this is freed automatically by scheme garbage collection
	// when it is no longer needed. 
	new SchemePrimitive<T>(name, cb, data);
}

};

#endif // _OPENCOG_SCHEME_PRIMITIVE_H

#endif // HAVE_GUILE

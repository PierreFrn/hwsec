// MASTER-ONLY: DO NOT MODIFY THIS FILE

/*
 * Copyright (C) Telecom Paris
 * 
 * This file must be used under the terms of the CeCILL. This source
 * file is licensed as described in the file COPYING, which you should
 * have received as part of this distribution. The terms are also
 * available at:
 * http://www.cecill.info/licences/Licence_CeCILL_V1.1-US.txt
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

void
myError (const char *file, const int line, const char *fnct, const char *frm,
     ...)
{
  va_list ap;

  va_start (ap, frm);
  fprintf (stderr, "*** error in file %s, line %d, function %s:\n", file,
	   line, fnct);
  vfprintf (stderr, frm, ap);
  fprintf (stderr, "\n");
}

void
warning (const char *file, const int line, const char *fnct, const char *frm,
	 ...)
{
  va_list ap;

  va_start (ap, frm);
  fprintf (stderr, "*** warning in file %s, line %d, function %s:\n*** ",
	   file, line, fnct);
  vfprintf (stderr, frm, ap);
  fprintf (stderr, "\n");
}

void *
xmalloc (const char *file, const int line, const char *fnct,
	 const size_t size)
{
  void *ptr;

  ptr = malloc (size);
  if (ptr == NULL)
    {
      myError (file, line, fnct, "memory allocation failed");
      exit (-1);
    }
  return ptr;
}

void *
xcalloc (const char *file, const int line, const char *fnct,
	 const size_t nmemb, const size_t size)
{
  void *ptr;

  ptr = calloc (nmemb, size);
  if (ptr == NULL)
    {
      myError (file, line, fnct, "memory allocation failed");
      exit (-1);
    }
  return ptr;
}

void *
xrealloc (const char *file, const int line, const char *fnct, void *ptr,
	  const size_t size)
{
  ptr = realloc (ptr, size);
  if (ptr == NULL)
    {
      myError (file, line, fnct, "memory allocation failed");
      exit (-1);
    }
  return ptr;
}

FILE *
xfopen (const char *file, const int line, const char *fnct, const char *name,
	const char *mode)
{
  FILE *fp;

  fp = fopen (name, mode);
  if (fp == NULL)
    {
      myError (file, line, fnct, "could not open file %s in mode %s", name,
	     mode);
      exit (-1);
    }
  return fp;
}

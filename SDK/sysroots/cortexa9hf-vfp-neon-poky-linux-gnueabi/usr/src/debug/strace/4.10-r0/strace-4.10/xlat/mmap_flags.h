/* Generated by ./xlat/gen.sh from ./xlat/mmap_flags.in; do not edit. */

static const struct xlat mmap_flags[] = {
#if defined(MAP_SHARED) || (defined(HAVE_DECL_MAP_SHARED) && HAVE_DECL_MAP_SHARED)
 XLAT(MAP_SHARED),
#endif
#if defined(MAP_PRIVATE) || (defined(HAVE_DECL_MAP_PRIVATE) && HAVE_DECL_MAP_PRIVATE)
 XLAT(MAP_PRIVATE),
#endif
#if defined(MAP_FIXED) || (defined(HAVE_DECL_MAP_FIXED) && HAVE_DECL_MAP_FIXED)
 XLAT(MAP_FIXED),
#endif
#if defined(MAP_ANONYMOUS) || (defined(HAVE_DECL_MAP_ANONYMOUS) && HAVE_DECL_MAP_ANONYMOUS)
 XLAT(MAP_ANONYMOUS),
#endif
#if defined(MAP_32BIT) || (defined(HAVE_DECL_MAP_32BIT) && HAVE_DECL_MAP_32BIT)
 XLAT(MAP_32BIT),
#endif
#if defined(MAP_RENAME) || (defined(HAVE_DECL_MAP_RENAME) && HAVE_DECL_MAP_RENAME)
 XLAT(MAP_RENAME),
#endif
#if defined(MAP_NORESERVE) || (defined(HAVE_DECL_MAP_NORESERVE) && HAVE_DECL_MAP_NORESERVE)
 XLAT(MAP_NORESERVE),
#endif
#if defined(MAP_POPULATE) || (defined(HAVE_DECL_MAP_POPULATE) && HAVE_DECL_MAP_POPULATE)
 XLAT(MAP_POPULATE),
#endif
#if defined(MAP_NONBLOCK) || (defined(HAVE_DECL_MAP_NONBLOCK) && HAVE_DECL_MAP_NONBLOCK)
 XLAT(MAP_NONBLOCK),
#endif
/*
* XXX - this was introduced in SunOS 4.x to distinguish between
* the old pre-4.x "mmap()", which:
*
*	only let you map devices with an "mmap" routine (e.g.,
*	frame buffers) in;
*
*	required you to specify the mapping address;
*
*	returned 0 on success and -1 on failure;
*
* memory and which, and the 4.x "mmap()" which:
*
*	can map plain files;
*
*	can be asked to pick where to map the file;
*
*	returns the address where it mapped the file on success
*	and -1 on failure.
*
* It's not actually used in source code that calls "mmap()"; the
* "mmap()" routine adds it for you.
*
* It'd be nice to come up with some way of eliminating it from
* the flags, e.g. reporting calls *without* it as "old_mmap()"
* and calls with it as "mmap()".
*/
#if defined(_MAP_NEW) || (defined(HAVE_DECL__MAP_NEW) && HAVE_DECL__MAP_NEW)
 XLAT(_MAP_NEW),
#endif
#if defined(MAP_GROWSDOWN) || (defined(HAVE_DECL_MAP_GROWSDOWN) && HAVE_DECL_MAP_GROWSDOWN)
 XLAT(MAP_GROWSDOWN),
#endif
#if defined(MAP_DENYWRITE) || (defined(HAVE_DECL_MAP_DENYWRITE) && HAVE_DECL_MAP_DENYWRITE)
 XLAT(MAP_DENYWRITE),
#endif
#if defined(MAP_EXECUTABLE) || (defined(HAVE_DECL_MAP_EXECUTABLE) && HAVE_DECL_MAP_EXECUTABLE)
 XLAT(MAP_EXECUTABLE),
#endif
#if defined(MAP_INHERIT) || (defined(HAVE_DECL_MAP_INHERIT) && HAVE_DECL_MAP_INHERIT)
 XLAT(MAP_INHERIT),
#endif
#if defined(MAP_FILE) || (defined(HAVE_DECL_MAP_FILE) && HAVE_DECL_MAP_FILE)
 XLAT(MAP_FILE),
#endif
#if defined(MAP_LOCKED) || (defined(HAVE_DECL_MAP_LOCKED) && HAVE_DECL_MAP_LOCKED)
 XLAT(MAP_LOCKED),
#endif
/* FreeBSD ones */
#if defined(MAP_ANON) && (!defined(MAP_ANONYMOUS) || MAP_ANON != MAP_ANONYMOUS)
#if defined(MAP_ANON) || (defined(HAVE_DECL_MAP_ANON) && HAVE_DECL_MAP_ANON)
 XLAT(MAP_ANON),
#endif
#endif
#if defined(MAP_HASSEMAPHORE) || (defined(HAVE_DECL_MAP_HASSEMAPHORE) && HAVE_DECL_MAP_HASSEMAPHORE)
 XLAT(MAP_HASSEMAPHORE),
#endif
#if defined(MAP_STACK) || (defined(HAVE_DECL_MAP_STACK) && HAVE_DECL_MAP_STACK)
 XLAT(MAP_STACK),
#endif
#if defined(MAP_HUGETLB) || (defined(HAVE_DECL_MAP_HUGETLB) && HAVE_DECL_MAP_HUGETLB)
 XLAT(MAP_HUGETLB),
#endif
#if defined MAP_UNINITIALIZED && MAP_UNINITIALIZED > 0
#if defined(MAP_UNINITIALIZED) || (defined(HAVE_DECL_MAP_UNINITIALIZED) && HAVE_DECL_MAP_UNINITIALIZED)
 XLAT(MAP_UNINITIALIZED),
#endif
#endif
#if defined(MAP_NOSYNC) || (defined(HAVE_DECL_MAP_NOSYNC) && HAVE_DECL_MAP_NOSYNC)
 XLAT(MAP_NOSYNC),
#endif
#if defined(MAP_NOCORE) || (defined(HAVE_DECL_MAP_NOCORE) && HAVE_DECL_MAP_NOCORE)
 XLAT(MAP_NOCORE),
#endif
 XLAT_END
};

/* Generated by ./xlat/gen.sh from ./xlat/wait4_options.in; do not edit. */

static const struct xlat wait4_options[] = {
#if defined(WNOHANG) || (defined(HAVE_DECL_WNOHANG) && HAVE_DECL_WNOHANG)
 XLAT(WNOHANG),
#endif
#ifndef WSTOPPED
#if defined(WUNTRACED) || (defined(HAVE_DECL_WUNTRACED) && HAVE_DECL_WUNTRACED)
 XLAT(WUNTRACED),
#endif
#endif
#if defined(WEXITED) || (defined(HAVE_DECL_WEXITED) && HAVE_DECL_WEXITED)
 XLAT(WEXITED),
#endif
#if defined(WTRAPPED) || (defined(HAVE_DECL_WTRAPPED) && HAVE_DECL_WTRAPPED)
 XLAT(WTRAPPED),
#endif
#if defined(WSTOPPED) || (defined(HAVE_DECL_WSTOPPED) && HAVE_DECL_WSTOPPED)
 XLAT(WSTOPPED),
#endif
#if defined(WCONTINUED) || (defined(HAVE_DECL_WCONTINUED) && HAVE_DECL_WCONTINUED)
 XLAT(WCONTINUED),
#endif
#if defined(WNOWAIT) || (defined(HAVE_DECL_WNOWAIT) && HAVE_DECL_WNOWAIT)
 XLAT(WNOWAIT),
#endif
#if defined(__WCLONE) || (defined(HAVE_DECL___WCLONE) && HAVE_DECL___WCLONE)
 XLAT(__WCLONE),
#endif
#if defined(__WALL) || (defined(HAVE_DECL___WALL) && HAVE_DECL___WALL)
 XLAT(__WALL),
#endif
#if defined(__WNOTHREAD) || (defined(HAVE_DECL___WNOTHREAD) && HAVE_DECL___WNOTHREAD)
 XLAT(__WNOTHREAD),
#endif
 XLAT_END
};

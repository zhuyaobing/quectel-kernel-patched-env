/* Generated by ./xlat/gen.sh from ./xlat/bpf_rval.in; do not edit. */

static const struct xlat bpf_rval[] = {
#if defined(BPF_K) || (defined(HAVE_DECL_BPF_K) && HAVE_DECL_BPF_K)
 XLAT(BPF_K),
#endif
#if defined(BPF_X) || (defined(HAVE_DECL_BPF_X) && HAVE_DECL_BPF_X)
 XLAT(BPF_X),
#endif
#if defined(BPF_A) || (defined(HAVE_DECL_BPF_A) && HAVE_DECL_BPF_A)
 XLAT(BPF_A),
#endif
 XLAT_END
};

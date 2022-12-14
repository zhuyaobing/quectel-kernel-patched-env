/*
 * Copyright 2012 Bjorn Fahller <bjorn@fahller.se>
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <crpcut.hpp>

namespace crpcut {

  regex::regex(const regex& r)
    : p_(r.p_)
  {
  }

  std::ostream& operator<<(std::ostream &os, const regex &r)
  {
    return os << *r.p_;
  }

  void regex::type::init(const char *s, int flags)
  {
    int i = wrapped::regcomp(&r_, s, flags | REG_NOSUB);
    if (i != 0)
      {
        size_t n = wrapped::regerror(i, &r_, 0, 0);
        errmsg_ = new char[n];
        wrapped::regerror(i, &r_, errmsg_, n);
      }
  }

  bool regex::type::match(const char *s)
  {
    if (errmsg_) return false;
    int i = wrapped::regexec(&r_, s, 0, 0, 0);
    return i == 0;
  }

  regex::type::~type()
  {
    wrapped::regfree(&r_);
    delete[] errmsg_;
  }

  std::ostream& operator<<(std::ostream &os, const regex::type &obj)
  {
    if (obj.errmsg_)
      {
        return os << obj.errmsg_;
      }
    return os << "regex(\"" << obj.re_str_ << "\")";
  }
}

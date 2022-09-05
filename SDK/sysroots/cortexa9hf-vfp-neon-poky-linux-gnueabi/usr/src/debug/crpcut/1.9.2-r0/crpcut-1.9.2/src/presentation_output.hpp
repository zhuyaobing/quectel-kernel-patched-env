/*
 * Copyright 2011-2012 Bjorn Fahller <bjorn@fahller.se>
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

#ifndef PRESENTATION_OUTPUT_HPP
#define PRESENTATION_OUTPUT_HPP

#include "io.hpp"
#include <cstddef>
namespace crpcut {
  template <typename T>
  class poll;

  namespace output {
    class buffer;
  }

  class posix_write;
  class presentation_output : public io
  {
  public:
    presentation_output(output::buffer &buffer,
                        poll<io>       &poller,
                        int             fd,
                        posix_write    &w = default_write());
    virtual ~presentation_output();
    virtual bool read();
    virtual bool write();
    virtual void exception() { enable(false); }
    virtual void enable(bool val);
    bool         enabled() const;
  private:
    static posix_write& default_write();
    output::buffer &buffer_;
    poll<io>       &poller_;
    int             fd_;
    std::size_t     pos_;
    bool            is_enabled_;
    posix_write    &write_;
  };

}
#endif // PRESENTATION_OUTPUT_HPP

/* Copyright (C) 1993-2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.

   As a special exception, if you link the code in this file with
   files compiled with a GNU compiler to produce an executable,
   that does not cause the resulting executable to be covered by
   the GNU Lesser General Public License.  This exception does not
   however invalidate any other reasons why the executable file
   might be covered by the GNU Lesser General Public License.
   This exception applies to code released by its copyright holders
   in files containing the exception.  */

#include <libioP.h>

_IO_off64_t
_IO_seekpos_unlocked (fp, pos, mode)
     _IO_FILE *fp;
     _IO_off64_t pos;
     int mode;
{
  /* If we have a backup buffer, get rid of it, since the __seekoff
     callback may not know to do the right thing about it.
     This may be over-kill, but it'll do for now. TODO */
  if (! _IO_is_wide (fp))
    {
      if (_IO_have_backup (fp))
	_IO_free_backup_area (fp);
    }
  else
    {
      if (_IO_have_wbackup (fp))
	_IO_free_wbackup_area (fp);
    }

  return _IO_SEEKOFF (fp, pos, 0, mode);
}


_IO_off64_t
_IO_seekpos (fp, pos, mode)
     _IO_FILE *fp;
     _IO_off64_t pos;
     int mode;
{
  _IO_off64_t retval;

  _IO_acquire_lock (fp);
  retval = _IO_seekpos_unlocked (fp, pos, mode);
  _IO_release_lock (fp);
  return retval;
}

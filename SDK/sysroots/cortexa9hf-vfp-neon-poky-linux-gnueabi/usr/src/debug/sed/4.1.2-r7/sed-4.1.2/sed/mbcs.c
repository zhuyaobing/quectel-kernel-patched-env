/*  GNU SED, a batch stream editor.
    Copyright (C) 2003 Free Software Foundation, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#include "sed.h"
#include <stdlib.h>

int mb_cur_max;

#ifdef HAVE_MBRTOWC
/* Add a byte to the multibyte character represented by the state
   CUR_STAT, and answer its length if a character is completed,
   or -2 if it is yet to be completed.  */
int brlen (ch, cur_stat)
     int ch;
     mbstate_t *cur_stat;
{
  char c = ch;

  /* If we use the generic brlen, then MBRLEN == mbrlen.  */
  int result = mbrtowc(NULL, &c, 1, cur_stat);

  /* An invalid sequence is treated like a singlebyte character. */
  if (result == -1)
    {
      memset (cur_stat, 0, sizeof (mbstate_t));
      return 1;
    }

  return result;
}
#endif

void
initialize_mbcs ()
{
#ifdef HAVE_MBRTOWC
  mb_cur_max = MB_CUR_MAX;
#else
  mb_cur_max = 1;
#endif
}


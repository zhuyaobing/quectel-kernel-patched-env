/***
  This file is part of systemd.

  Copyright 2012 Lennart Poettering

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include <alloca.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "alloc-util.h"
#include "calendarspec.h"
#include "fileio.h"
#include "macro.h"
#include "parse-util.h"
#include "string-util.h"

/* Longest valid date/time range is 1970..2199 */
#define MAX_RANGE_LEN   230
#define BITS_WEEKDAYS   127

static void free_chain(CalendarComponent *c) {
        CalendarComponent *n;

        while (c) {
                n = c->next;
                free(c);
                c = n;
        }
}

void calendar_spec_free(CalendarSpec *c) {

        if (!c)
                return;

        free_chain(c->year);
        free_chain(c->month);
        free_chain(c->day);
        free_chain(c->hour);
        free_chain(c->minute);
        free_chain(c->microsecond);

        free(c);
}

static int component_compare(const void *_a, const void *_b) {
        CalendarComponent * const *a = _a, * const *b = _b;

        if ((*a)->value < (*b)->value)
                return -1;
        if ((*a)->value > (*b)->value)
                return 1;

        if ((*a)->repeat < (*b)->repeat)
                return -1;
        if ((*a)->repeat > (*b)->repeat)
                return 1;

        return 0;
}

static void sort_chain(CalendarComponent **c) {
        unsigned n = 0, k;
        CalendarComponent **b, *i, **j, *next;

        assert(c);

        for (i = *c; i; i = i->next)
                n++;

        if (n <= 1)
                return;

        j = b = alloca(sizeof(CalendarComponent*) * n);
        for (i = *c; i; i = i->next)
                *(j++) = i;

        qsort(b, n, sizeof(CalendarComponent*), component_compare);

        b[n-1]->next = NULL;
        next = b[n-1];

        /* Drop non-unique entries */
        for (k = n-1; k > 0; k--) {
                if (b[k-1]->value == next->value &&
                    b[k-1]->repeat == next->repeat) {
                        free(b[k-1]);
                        continue;
                }

                b[k-1]->next = next;
                next = b[k-1];
        }

        *c = next;
}

static void fix_year(CalendarComponent *c) {
        /* Turns 12 ??? 2012, 89 ??? 1989 */

        while (c) {
                CalendarComponent *n = c->next;

                if (c->value >= 0 && c->value < 70)
                        c->value += 2000;

                if (c->value >= 70 && c->value < 100)
                        c->value += 1900;

                c = n;
        }
}

int calendar_spec_normalize(CalendarSpec *c) {
        assert(c);

        if (c->weekdays_bits <= 0 || c->weekdays_bits >= BITS_WEEKDAYS)
                c->weekdays_bits = -1;

        fix_year(c->year);

        sort_chain(&c->year);
        sort_chain(&c->month);
        sort_chain(&c->day);
        sort_chain(&c->hour);
        sort_chain(&c->minute);
        sort_chain(&c->microsecond);

        return 0;
}

_pure_ static bool chain_valid(CalendarComponent *c, int from, int to) {
        if (!c)
                return true;

        if (c->value < from || c->value > to)
                return false;

        if (c->value + c->repeat > to)
                return false;

        if (c->next)
                return chain_valid(c->next, from, to);

        return true;
}

_pure_ bool calendar_spec_valid(CalendarSpec *c) {
        assert(c);

        if (c->weekdays_bits > BITS_WEEKDAYS)
                return false;

        if (!chain_valid(c->year, 1970, 2199))
                return false;

        if (!chain_valid(c->month, 1, 12))
                return false;

        if (!chain_valid(c->day, 1, 31))
                return false;

        if (!chain_valid(c->hour, 0, 23))
                return false;

        if (!chain_valid(c->minute, 0, 59))
                return false;

        if (!chain_valid(c->microsecond, 0, 60*USEC_PER_SEC-1))
                return false;

        return true;
}

static void format_weekdays(FILE *f, const CalendarSpec *c) {
        static const char *const days[] = {
                "Mon",
                "Tue",
                "Wed",
                "Thu",
                "Fri",
                "Sat",
                "Sun"
        };

        int l, x;
        bool need_comma = false;

        assert(f);
        assert(c);
        assert(c->weekdays_bits > 0 && c->weekdays_bits <= BITS_WEEKDAYS);

        for (x = 0, l = -1; x < (int) ELEMENTSOF(days); x++) {

                if (c->weekdays_bits & (1 << x)) {

                        if (l < 0) {
                                if (need_comma)
                                        fputc(',', f);
                                else
                                        need_comma = true;

                                fputs(days[x], f);
                                l = x;
                        }

                } else if (l >= 0) {

                        if (x > l + 1) {
                                fputs(x > l + 2 ? ".." : ",", f);
                                fputs(days[x-1], f);
                        }

                        l = -1;
                }
        }

        if (l >= 0 && x > l + 1) {
                fputs(x > l + 2 ? ".." : ",", f);
                fputs(days[x-1], f);
        }
}

static void format_chain(FILE *f, int space, const CalendarComponent *c, bool usec) {
        assert(f);

        if (!c) {
                fputc('*', f);
                return;
        }

        assert(c->value >= 0);
        if (!usec)
                fprintf(f, "%0*i", space, c->value);
        else if (c->value % USEC_PER_SEC == 0)
                fprintf(f, "%0*i", space, (int) (c->value / USEC_PER_SEC));
        else
                fprintf(f, "%0*i.%06i", space, (int) (c->value / USEC_PER_SEC), (int) (c->value % USEC_PER_SEC));

        if (c->repeat > 0) {
                if (!usec)
                        fprintf(f, "/%i", c->repeat);
                else if (c->repeat % USEC_PER_SEC == 0)
                        fprintf(f, "/%i", (int) (c->repeat / USEC_PER_SEC));
                else
                        fprintf(f, "/%i.%06i", (int) (c->repeat / USEC_PER_SEC), (int) (c->repeat % USEC_PER_SEC));
        }

        if (c->next) {
                fputc(',', f);
                format_chain(f, space, c->next, usec);
        }
}

int calendar_spec_to_string(const CalendarSpec *c, char **p) {
        char *buf = NULL;
        size_t sz = 0;
        FILE *f;
        int r;

        assert(c);
        assert(p);

        f = open_memstream(&buf, &sz);
        if (!f)
                return -ENOMEM;

        if (c->weekdays_bits > 0 && c->weekdays_bits <= BITS_WEEKDAYS) {
                format_weekdays(f, c);
                fputc(' ', f);
        }

        format_chain(f, 4, c->year, false);
        fputc('-', f);
        format_chain(f, 2, c->month, false);
        fputc('-', f);
        format_chain(f, 2, c->day, false);
        fputc(' ', f);
        format_chain(f, 2, c->hour, false);
        fputc(':', f);
        format_chain(f, 2, c->minute, false);
        fputc(':', f);
        format_chain(f, 2, c->microsecond, true);

        if (c->utc)
                fputs(" UTC", f);
        else if (IN_SET(c->dst, 0, 1)) {

                /* If daylight saving is explicitly on or off, let's show the used timezone. */

                tzset();

                if (!isempty(tzname[c->dst])) {
                        fputc(' ', f);
                        fputs(tzname[c->dst], f);
                }
        }

        r = fflush_and_check(f);
        if (r < 0) {
                free(buf);
                fclose(f);
                return r;
        }

        fclose(f);

        *p = buf;
        return 0;
}

static int parse_weekdays(const char **p, CalendarSpec *c) {
        static const struct {
                const char *name;
                const int nr;
        } day_nr[] = {
                { "Monday",    0 },
                { "Mon",       0 },
                { "Tuesday",   1 },
                { "Tue",       1 },
                { "Wednesday", 2 },
                { "Wed",       2 },
                { "Thursday",  3 },
                { "Thu",       3 },
                { "Friday",    4 },
                { "Fri",       4 },
                { "Saturday",  5 },
                { "Sat",       5 },
                { "Sunday",    6 },
                { "Sun",       6 }
        };

        int l = -1;
        bool first = true;

        assert(p);
        assert(*p);
        assert(c);

        for (;;) {
                unsigned i;

                if (!first && **p == ' ')
                        return 0;

                for (i = 0; i < ELEMENTSOF(day_nr); i++) {
                        size_t skip;

                        if (!startswith_no_case(*p, day_nr[i].name))
                                continue;

                        skip = strlen(day_nr[i].name);

                        if ((*p)[skip] != '-' &&
                            (*p)[skip] != '.' &&
                            (*p)[skip] != ',' &&
                            (*p)[skip] != ' ' &&
                            (*p)[skip] != 0)
                                return -EINVAL;

                        c->weekdays_bits |= 1 << day_nr[i].nr;

                        if (l >= 0) {
                                int j;

                                if (l > day_nr[i].nr)
                                        return -EINVAL;

                                for (j = l + 1; j < day_nr[i].nr; j++)
                                        c->weekdays_bits |= 1 << j;
                        }

                        *p += skip;
                        break;
                }

                /* Couldn't find this prefix, so let's assume the
                   weekday was not specified and let's continue with
                   the date */
                if (i >= ELEMENTSOF(day_nr))
                        return first ? 0 : -EINVAL;

                /* We reached the end of the string */
                if (**p == 0)
                        return 0;

                /* We reached the end of the weekday spec part */
                if (**p == ' ') {
                        *p += strspn(*p, " ");
                        return 0;
                }

                if (**p == '.') {
                        if (l >= 0)
                                return -EINVAL;

                        if ((*p)[1] != '.')
                                return -EINVAL;

                        l = day_nr[i].nr;
                        *p += 1;

                /* Support ranges with "-" for backwards compatibility */
                } else if (**p == '-') {
                        if (l >= 0)
                                return -EINVAL;

                        l = day_nr[i].nr;
                } else
                        l = -1;

                *p += 1;
                first = false;
        }
}

static int parse_component_decimal(const char **p, bool usec, unsigned long *res) {
        unsigned long value;
        const char *e = NULL;
        char *ee = NULL;
        int r;

        errno = 0;
        value = strtoul(*p, &ee, 10);
        if (errno > 0)
                return -errno;
        if (ee == *p)
                return -EINVAL;
        if ((unsigned long) (int) value != value)
                return -ERANGE;
        e = ee;

        if (usec) {
                if (value * USEC_PER_SEC / USEC_PER_SEC != value)
                        return -ERANGE;

                value *= USEC_PER_SEC;
                if (*e == '.') {
                        unsigned add;

                        e++;
                        r = parse_fractional_part_u(&e, 6, &add);
                        if (r < 0)
                                return r;

                        if (add + value < value)
                                return -ERANGE;
                        value += add;
                }
        }

        *p = e;
        *res = value;

        return 0;
}

static int const_chain(int value, CalendarComponent **c) {
        CalendarComponent *cc = NULL;

        assert(c);

        cc = new0(CalendarComponent, 1);
        if (!cc)
                return -ENOMEM;

        cc->value = value;
        cc->repeat = 0;
        cc->next = *c;

        *c = cc;

        return 0;
}

static int prepend_component(const char **p, bool usec, CalendarComponent **c) {
        unsigned long i, value, range_end, range_inc, repeat = 0;
        CalendarComponent *cc;
        int r;
        const char *e;

        assert(p);
        assert(c);

        e = *p;

        r = parse_component_decimal(&e, usec, &value);
        if (r < 0)
                return r;

        if (*e == '/') {
                e++;
                r = parse_component_decimal(&e, usec, &repeat);
                if (r < 0)
                        return r;

                if (repeat == 0)
                        return -ERANGE;
        } else if (e[0] == '.' && e[1] == '.') {
                e += 2;
                r = parse_component_decimal(&e, usec, &range_end);
                if (r < 0)
                        return r;

                if (value >= range_end)
                        return -EINVAL;

                range_inc = usec ? USEC_PER_SEC : 1;

                /* Don't allow impossibly large ranges... */
                if (range_end - value >= MAX_RANGE_LEN * range_inc)
                        return -EINVAL;

                /* ...or ranges with only a single element */
                if (range_end - value < range_inc)
                        return -EINVAL;

                for (i = value; i <= range_end; i += range_inc) {
                        r = const_chain(i, c);
                        if (r < 0)
                                return r;
                }
        }

        if (*e != 0 && *e != ' ' && *e != ',' && *e != '-' && *e != ':')
                return -EINVAL;

        cc = new0(CalendarComponent, 1);
        if (!cc)
                return -ENOMEM;

        cc->value = value;
        cc->repeat = repeat;
        cc->next = *c;

        *p = e;
        *c = cc;

        if (*e ==',') {
                *p += 1;
                return prepend_component(p, usec, c);
        }

        return 0;
}

static int parse_chain(const char **p, bool usec, CalendarComponent **c) {
        const char *t;
        CalendarComponent *cc = NULL;
        int r;

        assert(p);
        assert(c);

        t = *p;

        if (t[0] == '*') {
                if (usec) {
                        r = const_chain(0, c);
                        if (r < 0)
                                return r;
                        (*c)->repeat = USEC_PER_SEC;
                } else
                        *c = NULL;

                *p = t + 1;
                return 0;
        }

        r = prepend_component(&t, usec, &cc);
        if (r < 0) {
                free_chain(cc);
                return r;
        }

        *p = t;
        *c = cc;
        return 0;
}

static int parse_date(const char **p, CalendarSpec *c) {
        const char *t;
        int r;
        CalendarComponent *first, *second, *third;

        assert(p);
        assert(*p);
        assert(c);

        t = *p;

        if (*t == 0)
                return 0;

        r = parse_chain(&t, false, &first);
        if (r < 0)
                return r;

        /* Already the end? A ':' as separator? In that case this was a time, not a date */
        if (*t == 0 || *t == ':') {
                free_chain(first);
                return 0;
        }

        if (*t != '-') {
                free_chain(first);
                return -EINVAL;
        }

        t++;
        r = parse_chain(&t, false, &second);
        if (r < 0) {
                free_chain(first);
                return r;
        }

        /* Got two parts, hence it's month and day */
        if (*t == ' ' || *t == 0) {
                *p = t + strspn(t, " ");
                c->month = first;
                c->day = second;
                return 0;
        }

        if (*t != '-') {
                free_chain(first);
                free_chain(second);
                return -EINVAL;
        }

        t++;
        r = parse_chain(&t, false, &third);
        if (r < 0) {
                free_chain(first);
                free_chain(second);
                return r;
        }

        /* Got tree parts, hence it is year, month and day */
        if (*t == ' ' || *t == 0) {
                *p = t + strspn(t, " ");
                c->year = first;
                c->month = second;
                c->day = third;
                return 0;
        }

        free_chain(first);
        free_chain(second);
        free_chain(third);
        return -EINVAL;
}

static int parse_calendar_time(const char **p, CalendarSpec *c) {
        CalendarComponent *h = NULL, *m = NULL, *s = NULL;
        const char *t;
        int r;

        assert(p);
        assert(*p);
        assert(c);

        t = *p;

        if (*t == 0) {
                /* If no time is specified at all, but a date of some
                 * kind, then this means 00:00:00 */
                if (c->day || c->weekdays_bits > 0)
                        goto null_hour;

                goto finish;
        }

        r = parse_chain(&t, false, &h);
        if (r < 0)
                goto fail;

        if (*t != ':') {
                r = -EINVAL;
                goto fail;
        }

        t++;
        r = parse_chain(&t, false, &m);
        if (r < 0)
                goto fail;

        /* Already at the end? Then it's hours and minutes, and seconds are 0 */
        if (*t == 0) {
                if (m != NULL)
                        goto null_second;

                goto finish;
        }

        if (*t != ':') {
                r = -EINVAL;
                goto fail;
        }

        t++;
        r = parse_chain(&t, true, &s);
        if (r < 0)
                goto fail;

        /* At the end? Then it's hours, minutes and seconds */
        if (*t == 0)
                goto finish;

        r = -EINVAL;
        goto fail;

null_hour:
        r = const_chain(0, &h);
        if (r < 0)
                goto fail;

        r = const_chain(0, &m);
        if (r < 0)
                goto fail;

null_second:
        r = const_chain(0, &s);
        if (r < 0)
                goto fail;

finish:
        *p = t;
        c->hour = h;
        c->minute = m;
        c->microsecond = s;

        return 0;

fail:
        free_chain(h);
        free_chain(m);
        free_chain(s);
        return r;
}

int calendar_spec_from_string(const char *p, CalendarSpec **spec) {
        const char *utc;
        CalendarSpec *c;
        int r;

        assert(p);
        assert(spec);

        if (isempty(p))
                return -EINVAL;

        c = new0(CalendarSpec, 1);
        if (!c)
                return -ENOMEM;
        c->dst = -1;

        utc = endswith_no_case(p, " UTC");
        if (utc) {
                c->utc = true;
                p = strndupa(p, utc - p);
        } else {
                const char *e = NULL;
                int j;

                tzset();

                /* Check if the local timezone was specified? */
                for (j = 0; j <= 1; j++) {
                        if (isempty(tzname[j]))
                                continue;

                        e = endswith_no_case(p, tzname[j]);
                        if(!e)
                                continue;
                        if (e == p)
                                continue;
                        if (e[-1] != ' ')
                                continue;

                        break;
                }

                /* Found one of the two timezones specified? */
                if (IN_SET(j, 0, 1)) {
                        p = strndupa(p, e - p - 1);
                        c->dst = j;
                }
        }

        if (strcaseeq(p, "minutely")) {
                r = const_chain(0, &c->microsecond);
                if (r < 0)
                        goto fail;

        } else if (strcaseeq(p, "hourly")) {
                r = const_chain(0, &c->minute);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->microsecond);
                if (r < 0)
                        goto fail;

        } else if (strcaseeq(p, "daily")) {
                r = const_chain(0, &c->hour);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->minute);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->microsecond);
                if (r < 0)
                        goto fail;

        } else if (strcaseeq(p, "monthly")) {
                r = const_chain(1, &c->day);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->hour);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->minute);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->microsecond);
                if (r < 0)
                        goto fail;

        } else if (strcaseeq(p, "annually") ||
                   strcaseeq(p, "yearly") ||
                   strcaseeq(p, "anually") /* backwards compatibility */ ) {

                r = const_chain(1, &c->month);
                if (r < 0)
                        goto fail;
                r = const_chain(1, &c->day);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->hour);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->minute);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->microsecond);
                if (r < 0)
                        goto fail;

        } else if (strcaseeq(p, "weekly")) {

                c->weekdays_bits = 1;

                r = const_chain(0, &c->hour);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->minute);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->microsecond);
                if (r < 0)
                        goto fail;

        } else if (strcaseeq(p, "quarterly")) {

                r = const_chain(1, &c->month);
                if (r < 0)
                        goto fail;
                r = const_chain(4, &c->month);
                if (r < 0)
                        goto fail;
                r = const_chain(7, &c->month);
                if (r < 0)
                        goto fail;
                r = const_chain(10, &c->month);
                if (r < 0)
                        goto fail;
                r = const_chain(1, &c->day);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->hour);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->minute);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->microsecond);
                if (r < 0)
                        goto fail;

        } else if (strcaseeq(p, "biannually") ||
                   strcaseeq(p, "bi-annually") ||
                   strcaseeq(p, "semiannually") ||
                   strcaseeq(p, "semi-annually")) {

                r = const_chain(1, &c->month);
                if (r < 0)
                        goto fail;
                r = const_chain(7, &c->month);
                if (r < 0)
                        goto fail;
                r = const_chain(1, &c->day);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->hour);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->minute);
                if (r < 0)
                        goto fail;
                r = const_chain(0, &c->microsecond);
                if (r < 0)
                        goto fail;

        } else {
                r = parse_weekdays(&p, c);
                if (r < 0)
                        goto fail;

                r = parse_date(&p, c);
                if (r < 0)
                        goto fail;

                r = parse_calendar_time(&p, c);
                if (r < 0)
                        goto fail;

                if (*p != 0) {
                        r = -EINVAL;
                        goto fail;
                }
        }

        r = calendar_spec_normalize(c);
        if (r < 0)
                goto fail;

        if (!calendar_spec_valid(c)) {
                r = -EINVAL;
                goto fail;
        }

        *spec = c;
        return 0;

fail:
        calendar_spec_free(c);
        return r;
}

static int find_matching_component(const CalendarComponent *c, int *val) {
        const CalendarComponent *n;
        int d = -1;
        bool d_set = false;
        int r;

        assert(val);

        if (!c)
                return 0;

        while (c) {
                n = c->next;

                if (c->value >= *val) {

                        if (!d_set || c->value < d) {
                                d = c->value;
                                d_set = true;
                        }

                } else if (c->repeat > 0) {
                        int k;

                        k = c->value + c->repeat * ((*val - c->value + c->repeat -1) / c->repeat);

                        if (!d_set || k < d) {
                                d = k;
                                d_set = true;
                        }
                }

                c = n;
        }

        if (!d_set)
                return -ENOENT;

        r = *val != d;
        *val = d;
        return r;
}

static bool tm_out_of_bounds(const struct tm *tm, bool utc) {
        struct tm t;
        assert(tm);

        t = *tm;

        if (mktime_or_timegm(&t, utc) == (time_t) -1)
                return true;

        /* Did any normalization take place? If so, it was out of bounds before */
        return
                t.tm_year != tm->tm_year ||
                t.tm_mon != tm->tm_mon ||
                t.tm_mday != tm->tm_mday ||
                t.tm_hour != tm->tm_hour ||
                t.tm_min != tm->tm_min ||
                t.tm_sec != tm->tm_sec;
}

static bool matches_weekday(int weekdays_bits, const struct tm *tm, bool utc) {
        struct tm t;
        int k;

        if (weekdays_bits < 0 || weekdays_bits >= BITS_WEEKDAYS)
                return true;

        t = *tm;
        if (mktime_or_timegm(&t, utc) == (time_t) -1)
                return false;

        k = t.tm_wday == 0 ? 6 : t.tm_wday - 1;
        return (weekdays_bits & (1 << k));
}

static int find_next(const CalendarSpec *spec, struct tm *tm, usec_t *usec) {
        struct tm c;
        int tm_usec;
        int r;

        assert(spec);
        assert(tm);

        c = *tm;
        tm_usec = *usec;

        for (;;) {
                /* Normalize the current date */
                (void) mktime_or_timegm(&c, spec->utc);
                c.tm_isdst = spec->dst;

                c.tm_year += 1900;
                r = find_matching_component(spec->year, &c.tm_year);
                c.tm_year -= 1900;

                if (r > 0) {
                        c.tm_mon = 0;
                        c.tm_mday = 1;
                        c.tm_hour = c.tm_min = c.tm_sec = tm_usec = 0;
                }
                if (r < 0)
                        return r;
                if (tm_out_of_bounds(&c, spec->utc))
                        return -ENOENT;

                c.tm_mon += 1;
                r = find_matching_component(spec->month, &c.tm_mon);
                c.tm_mon -= 1;

                if (r > 0) {
                        c.tm_mday = 1;
                        c.tm_hour = c.tm_min = c.tm_sec = tm_usec = 0;
                }
                if (r < 0 || tm_out_of_bounds(&c, spec->utc)) {
                        c.tm_year++;
                        c.tm_mon = 0;
                        c.tm_mday = 1;
                        c.tm_hour = c.tm_min = c.tm_sec = tm_usec = 0;
                        continue;
                }

                r = find_matching_component(spec->day, &c.tm_mday);
                if (r > 0)
                        c.tm_hour = c.tm_min = c.tm_sec = tm_usec = 0;
                if (r < 0 || tm_out_of_bounds(&c, spec->utc)) {
                        c.tm_mon++;
                        c.tm_mday = 1;
                        c.tm_hour = c.tm_min = c.tm_sec = tm_usec = 0;
                        continue;
                }

                if (!matches_weekday(spec->weekdays_bits, &c, spec->utc)) {
                        c.tm_mday++;
                        c.tm_hour = c.tm_min = c.tm_sec = tm_usec = 0;
                        continue;
                }

                r = find_matching_component(spec->hour, &c.tm_hour);
                if (r > 0)
                        c.tm_min = c.tm_sec = tm_usec = 0;
                if (r < 0 || tm_out_of_bounds(&c, spec->utc)) {
                        c.tm_mday++;
                        c.tm_hour = c.tm_min = c.tm_sec = tm_usec = 0;
                        continue;
                }

                r = find_matching_component(spec->minute, &c.tm_min);
                if (r > 0)
                        c.tm_sec = tm_usec = 0;
                if (r < 0 || tm_out_of_bounds(&c, spec->utc)) {
                        c.tm_hour++;
                        c.tm_min = c.tm_sec = tm_usec = 0;
                        continue;
                }

                c.tm_sec = c.tm_sec * USEC_PER_SEC + tm_usec;
                r = find_matching_component(spec->microsecond, &c.tm_sec);
                tm_usec = c.tm_sec % USEC_PER_SEC;
                c.tm_sec /= USEC_PER_SEC;

                if (r < 0 || tm_out_of_bounds(&c, spec->utc)) {
                        c.tm_min++;
                        c.tm_sec = tm_usec = 0;
                        continue;
                }

                *tm = c;
                *usec = tm_usec;
                return 0;
        }
}

int calendar_spec_next_usec(const CalendarSpec *spec, usec_t usec, usec_t *next) {
        struct tm tm;
        time_t t;
        int r;
        usec_t tm_usec;

        assert(spec);
        assert(next);

        usec++;
        t = (time_t) (usec / USEC_PER_SEC);
        assert_se(localtime_or_gmtime_r(&t, &tm, spec->utc));
        tm_usec = usec % USEC_PER_SEC;

        r = find_next(spec, &tm, &tm_usec);
        if (r < 0)
                return r;

        t = mktime_or_timegm(&tm, spec->utc);
        if (t == (time_t) -1)
                return -EINVAL;

        *next = (usec_t) t * USEC_PER_SEC + tm_usec;
        return 0;
}

/***
  This file is part of systemd.

  Copyright 2011 Lennart Poettering

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

#include <errno.h>
#include <string.h>
#include <sys/mount.h>
#include <unistd.h>

#include "alloc-util.h"
#include "bus-common-errors.h"
#include "bus-error.h"
#include "bus-util.h"
#include "cgroup-util.h"
#include "clean-ipc.h"
#include "conf-parser.h"
#include "escape.h"
#include "fd-util.h"
#include "fileio.h"
#include "formats-util.h"
#include "fs-util.h"
#include "hashmap.h"
#include "label.h"
#include "logind-user.h"
#include "mkdir.h"
#include "mount-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "rm-rf.h"
#include "smack-util.h"
#include "special.h"
#include "stdio-util.h"
#include "string-table.h"
#include "unit-name.h"
#include "user-util.h"
#include "util.h"

int user_new(User **out, Manager *m, uid_t uid, gid_t gid, const char *name) {
        _cleanup_(user_freep) User *u = NULL;
        char lu[DECIMAL_STR_MAX(uid_t) + 1];
        int r;

        assert(out);
        assert(m);
        assert(name);

        u = new0(User, 1);
        if (!u)
                return -ENOMEM;

        u->manager = m;
        u->uid = uid;
        u->gid = gid;
        xsprintf(lu, UID_FMT, uid);

        u->name = strdup(name);
        if (!u->name)
                return -ENOMEM;

        if (asprintf(&u->state_file, "/run/systemd/users/"UID_FMT, uid) < 0)
                return -ENOMEM;

        if (asprintf(&u->runtime_path, "/run/user/"UID_FMT, uid) < 0)
                return -ENOMEM;

        r = slice_build_subslice(SPECIAL_USER_SLICE, lu, &u->slice);
        if (r < 0)
                return r;

        r = unit_name_build("user", lu, ".service", &u->service);
        if (r < 0)
                return r;

        r = hashmap_put(m->users, UID_TO_PTR(uid), u);
        if (r < 0)
                return r;

        r = hashmap_put(m->user_units, u->slice, u);
        if (r < 0)
                return r;

        r = hashmap_put(m->user_units, u->service, u);
        if (r < 0)
                return r;

        *out = u;
        u = NULL;
        return 0;
}

User *user_free(User *u) {
        if (!u)
                return NULL;

        if (u->in_gc_queue)
                LIST_REMOVE(gc_queue, u->manager->user_gc_queue, u);

        while (u->sessions)
                session_free(u->sessions);

        if (u->service)
                hashmap_remove_value(u->manager->user_units, u->service, u);

        if (u->slice)
                hashmap_remove_value(u->manager->user_units, u->slice, u);

        hashmap_remove_value(u->manager->users, UID_TO_PTR(u->uid), u);

        u->slice_job = mfree(u->slice_job);
        u->service_job = mfree(u->service_job);

        u->service = mfree(u->service);
        u->slice = mfree(u->slice);
        u->runtime_path = mfree(u->runtime_path);
        u->state_file = mfree(u->state_file);
        u->name = mfree(u->name);

        return mfree(u);
}

static int user_save_internal(User *u) {
        _cleanup_free_ char *temp_path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        int r;

        assert(u);
        assert(u->state_file);

        r = mkdir_safe_label("/run/systemd/users", 0755, 0, 0);
        if (r < 0)
                goto fail;

        r = fopen_temporary(u->state_file, &f, &temp_path);
        if (r < 0)
                goto fail;

        fchmod(fileno(f), 0644);

        fprintf(f,
                "# This is private data. Do not parse.\n"
                "NAME=%s\n"
                "STATE=%s\n",
                u->name,
                user_state_to_string(user_get_state(u)));

        /* LEGACY: no-one reads RUNTIME= anymore, drop it at some point */
        if (u->runtime_path)
                fprintf(f, "RUNTIME=%s\n", u->runtime_path);

        if (u->service_job)
                fprintf(f, "SERVICE_JOB=%s\n", u->service_job);

        if (u->slice_job)
                fprintf(f, "SLICE_JOB=%s\n", u->slice_job);

        if (u->display)
                fprintf(f, "DISPLAY=%s\n", u->display->id);

        if (dual_timestamp_is_set(&u->timestamp))
                fprintf(f,
                        "REALTIME="USEC_FMT"\n"
                        "MONOTONIC="USEC_FMT"\n",
                        u->timestamp.realtime,
                        u->timestamp.monotonic);

        if (u->sessions) {
                Session *i;
                bool first;

                fputs("SESSIONS=", f);
                first = true;
                LIST_FOREACH(sessions_by_user, i, u->sessions) {
                        if (first)
                                first = false;
                        else
                                fputc(' ', f);

                        fputs(i->id, f);
                }

                fputs("\nSEATS=", f);
                first = true;
                LIST_FOREACH(sessions_by_user, i, u->sessions) {
                        if (!i->seat)
                                continue;

                        if (first)
                                first = false;
                        else
                                fputc(' ', f);

                        fputs(i->seat->id, f);
                }

                fputs("\nACTIVE_SESSIONS=", f);
                first = true;
                LIST_FOREACH(sessions_by_user, i, u->sessions) {
                        if (!session_is_active(i))
                                continue;

                        if (first)
                                first = false;
                        else
                                fputc(' ', f);

                        fputs(i->id, f);
                }

                fputs("\nONLINE_SESSIONS=", f);
                first = true;
                LIST_FOREACH(sessions_by_user, i, u->sessions) {
                        if (session_get_state(i) == SESSION_CLOSING)
                                continue;

                        if (first)
                                first = false;
                        else
                                fputc(' ', f);

                        fputs(i->id, f);
                }

                fputs("\nACTIVE_SEATS=", f);
                first = true;
                LIST_FOREACH(sessions_by_user, i, u->sessions) {
                        if (!session_is_active(i) || !i->seat)
                                continue;

                        if (first)
                                first = false;
                        else
                                fputc(' ', f);

                        fputs(i->seat->id, f);
                }

                fputs("\nONLINE_SEATS=", f);
                first = true;
                LIST_FOREACH(sessions_by_user, i, u->sessions) {
                        if (session_get_state(i) == SESSION_CLOSING || !i->seat)
                                continue;

                        if (first)
                                first = false;
                        else
                                fputc(' ', f);

                        fputs(i->seat->id, f);
                }
                fputc('\n', f);
        }

        r = fflush_and_check(f);
        if (r < 0)
                goto fail;

        if (rename(temp_path, u->state_file) < 0) {
                r = -errno;
                goto fail;
        }

        return 0;

fail:
        (void) unlink(u->state_file);

        if (temp_path)
                (void) unlink(temp_path);

        return log_error_errno(r, "Failed to save user data %s: %m", u->state_file);
}

int user_save(User *u) {
        assert(u);

        if (!u->started)
                return 0;

        return user_save_internal (u);
}

int user_load(User *u) {
        _cleanup_free_ char *display = NULL, *realtime = NULL, *monotonic = NULL;
        Session *s = NULL;
        int r;

        assert(u);

        r = parse_env_file(u->state_file, NEWLINE,
                           "SERVICE_JOB", &u->service_job,
                           "SLICE_JOB",   &u->slice_job,
                           "DISPLAY",     &display,
                           "REALTIME",    &realtime,
                           "MONOTONIC",   &monotonic,
                           NULL);
        if (r < 0) {
                if (r == -ENOENT)
                        return 0;

                return log_error_errno(r, "Failed to read %s: %m", u->state_file);
        }

        if (display)
                s = hashmap_get(u->manager->sessions, display);

        if (s && s->display && display_is_local(s->display))
                u->display = s;

        if (realtime)
                timestamp_deserialize(realtime, &u->timestamp.realtime);
        if (monotonic)
                timestamp_deserialize(monotonic, &u->timestamp.monotonic);

        return r;
}

static int user_mkdir_runtime_path(User *u) {
        int r;

        assert(u);

        r = mkdir_safe_label("/run/user", 0755, 0, 0);
        if (r < 0)
                return log_error_errno(r, "Failed to create /run/user: %m");

        if (path_is_mount_point(u->runtime_path, 0) <= 0) {
                _cleanup_free_ char *t = NULL;

                (void) mkdir_label(u->runtime_path, 0700);

                if (mac_smack_use())
                        r = asprintf(&t, "mode=0700,smackfsroot=*,uid=" UID_FMT ",gid=" GID_FMT ",size=%zu", u->uid, u->gid, u->manager->runtime_dir_size);
                else
                        r = asprintf(&t, "mode=0700,uid=" UID_FMT ",gid=" GID_FMT ",size=%zu", u->uid, u->gid, u->manager->runtime_dir_size);
                if (r < 0) {
                        r = log_oom();
                        goto fail;
                }

                r = mount("tmpfs", u->runtime_path, "tmpfs", MS_NODEV|MS_NOSUID, t);
                if (r < 0) {
                        if (errno != EPERM && errno != EACCES) {
                                r = log_error_errno(errno, "Failed to mount per-user tmpfs directory %s: %m", u->runtime_path);
                                goto fail;
                        }

                        log_debug_errno(errno, "Failed to mount per-user tmpfs directory %s, assuming containerized execution, ignoring: %m", u->runtime_path);

                        r = chmod_and_chown(u->runtime_path, 0700, u->uid, u->gid);
                        if (r < 0) {
                                log_error_errno(r, "Failed to change runtime directory ownership and mode: %m");
                                goto fail;
                        }
                }

                r = label_fix(u->runtime_path, false, false);
                if (r < 0)
                        log_warning_errno(r, "Failed to fix label of '%s', ignoring: %m", u->runtime_path);
        }

        return 0;

fail:
        /* Try to clean up, but ignore errors */
        (void) rmdir(u->runtime_path);
        return r;
}

static int user_start_slice(User *u) {
        _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
        const char *description;
        char *job;
        int r;

        assert(u);

        u->slice_job = mfree(u->slice_job);
        description = strjoina("User Slice of ", u->name);

        r = manager_start_slice(
                        u->manager,
                        u->slice,
                        description,
                        "systemd-logind.service",
                        "systemd-user-sessions.service",
                        u->manager->user_tasks_max,
                        &error,
                        &job);
        if (r >= 0)
                u->slice_job = job;
        else if (!sd_bus_error_has_name(&error, BUS_ERROR_UNIT_EXISTS))
                /* we don't fail due to this, let's try to continue */
                log_error_errno(r, "Failed to start user slice %s, ignoring: %s (%s)",
                                u->slice, bus_error_message(&error, r), error.name);

        return 0;
}

static int user_start_service(User *u) {
        _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
        char *job;
        int r;

        assert(u);

        u->service_job = mfree(u->service_job);

        r = manager_start_unit(
                        u->manager,
                        u->service,
                        &error,
                        &job);
        if (r < 0) {
                /* we don't fail due to this, let's try to continue */
                log_error_errno(r, "Failed to start user service, ignoring: %s", bus_error_message(&error, r));
        } else {
                u->service_job = job;
        }

        return 0;
}

int user_start(User *u) {
        int r;

        assert(u);

        if (u->started && !u->stopping)
                return 0;

        /*
         * If u->stopping is set, the user is marked for removal and the slice
         * and service stop-jobs are queued. We have to clear that flag before
         * queing the start-jobs again. If they succeed, the user object can be
         * re-used just fine (pid1 takes care of job-ordering and proper
         * restart), but if they fail, we want to force another user_stop() so
         * possibly pending units are stopped.
         * Note that we don't clear u->started, as we have no clue what state
         * the user is in on failure here. Hence, we pretend the user is
         * running so it will be properly taken down by GC. However, we clearly
         * return an error from user_start() in that case, so no further
         * reference to the user is taken.
         */
        u->stopping = false;

        if (!u->started) {
                log_debug("New user %s logged in.", u->name);

                /* Make XDG_RUNTIME_DIR */
                r = user_mkdir_runtime_path(u);
                if (r < 0)
                        return r;
        }

        /* Create cgroup */
        r = user_start_slice(u);
        if (r < 0)
                return r;

        /* Save the user data so far, because pam_systemd will read the
         * XDG_RUNTIME_DIR out of it while starting up systemd --user.
         * We need to do user_save_internal() because we have not
         * "officially" started yet. */
        user_save_internal(u);

        /* Spawn user systemd */
        r = user_start_service(u);
        if (r < 0)
                return r;

        if (!u->started) {
                if (!dual_timestamp_is_set(&u->timestamp))
                        dual_timestamp_get(&u->timestamp);
                user_send_signal(u, true);
                u->started = true;
        }

        /* Save new user data */
        user_save(u);

        return 0;
}

static int user_stop_slice(User *u) {
        _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
        char *job;
        int r;

        assert(u);

        r = manager_stop_unit(u->manager, u->slice, &error, &job);
        if (r < 0) {
                log_error("Failed to stop user slice: %s", bus_error_message(&error, r));
                return r;
        }

        free(u->slice_job);
        u->slice_job = job;

        return r;
}

static int user_stop_service(User *u) {
        _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
        char *job;
        int r;

        assert(u);

        r = manager_stop_unit(u->manager, u->service, &error, &job);
        if (r < 0) {
                log_error("Failed to stop user service: %s", bus_error_message(&error, r));
                return r;
        }

        free(u->service_job);
        u->service_job = job;

        return r;
}

static int user_remove_runtime_path(User *u) {
        int r;

        assert(u);

        r = rm_rf(u->runtime_path, 0);
        if (r < 0)
                log_error_errno(r, "Failed to remove runtime directory %s: %m", u->runtime_path);

        /* Ignore cases where the directory isn't mounted, as that's
         * quite possible, if we lacked the permissions to mount
         * something */
        r = umount2(u->runtime_path, MNT_DETACH);
        if (r < 0 && errno != EINVAL && errno != ENOENT)
                log_error_errno(errno, "Failed to unmount user runtime directory %s: %m", u->runtime_path);

        r = rm_rf(u->runtime_path, REMOVE_ROOT);
        if (r < 0)
                log_error_errno(r, "Failed to remove runtime directory %s: %m", u->runtime_path);

        return r;
}

int user_stop(User *u, bool force) {
        Session *s;
        int r = 0, k;
        assert(u);

        /* Stop jobs have already been queued */
        if (u->stopping) {
                user_save(u);
                return r;
        }

        LIST_FOREACH(sessions_by_user, s, u->sessions) {
                k = session_stop(s, force);
                if (k < 0)
                        r = k;
        }

        /* Kill systemd */
        k = user_stop_service(u);
        if (k < 0)
                r = k;

        /* Kill cgroup */
        k = user_stop_slice(u);
        if (k < 0)
                r = k;

        u->stopping = true;

        user_save(u);

        return r;
}

int user_finalize(User *u) {
        Session *s;
        int r = 0, k;

        assert(u);

        if (u->started)
                log_debug("User %s logged out.", u->name);

        LIST_FOREACH(sessions_by_user, s, u->sessions) {
                k = session_finalize(s);
                if (k < 0)
                        r = k;
        }

        /* Kill XDG_RUNTIME_DIR */
        k = user_remove_runtime_path(u);
        if (k < 0)
                r = k;

        /* Clean SysV + POSIX IPC objects, but only if this is not a system user. Background: in many setups cronjobs
         * are run in full PAM and thus logind sessions, even if the code run doesn't belong to actual users but to
         * system components. Since enable RemoveIPC= globally for all users, we need to be a bit careful with such
         * cases, as we shouldn't accidentally remove a system service's IPC objects while it is running, just because
         * a cronjob running as the same user just finished. Hence: exclude system users generally from IPC clean-up,
         * and do it only for normal users. */
        if (u->manager->remove_ipc && u->uid > SYSTEM_UID_MAX) {
                k = clean_ipc_by_uid(u->uid);
                if (k < 0)
                        r = k;
        }

        unlink(u->state_file);
        user_add_to_gc_queue(u);

        if (u->started) {
                user_send_signal(u, false);
                u->started = false;
        }

        return r;
}

int user_get_idle_hint(User *u, dual_timestamp *t) {
        Session *s;
        bool idle_hint = true;
        dual_timestamp ts = DUAL_TIMESTAMP_NULL;

        assert(u);

        LIST_FOREACH(sessions_by_user, s, u->sessions) {
                dual_timestamp k;
                int ih;

                ih = session_get_idle_hint(s, &k);
                if (ih < 0)
                        return ih;

                if (!ih) {
                        if (!idle_hint) {
                                if (k.monotonic < ts.monotonic)
                                        ts = k;
                        } else {
                                idle_hint = false;
                                ts = k;
                        }
                } else if (idle_hint) {

                        if (k.monotonic > ts.monotonic)
                                ts = k;
                }
        }

        if (t)
                *t = ts;

        return idle_hint;
}

int user_check_linger_file(User *u) {
        _cleanup_free_ char *cc = NULL;
        char *p = NULL;

        cc = cescape(u->name);
        if (!cc)
                return -ENOMEM;

        p = strjoina("/var/lib/systemd/linger/", cc);

        return access(p, F_OK) >= 0;
}

bool user_check_gc(User *u, bool drop_not_started) {
        assert(u);

        if (drop_not_started && !u->started)
                return false;

        if (u->sessions)
                return true;

        if (user_check_linger_file(u) > 0)
                return true;

        if (u->slice_job && manager_job_is_active(u->manager, u->slice_job))
                return true;

        if (u->service_job && manager_job_is_active(u->manager, u->service_job))
                return true;

        return false;
}

void user_add_to_gc_queue(User *u) {
        assert(u);

        if (u->in_gc_queue)
                return;

        LIST_PREPEND(gc_queue, u->manager->user_gc_queue, u);
        u->in_gc_queue = true;
}

UserState user_get_state(User *u) {
        Session *i;

        assert(u);

        if (u->stopping)
                return USER_CLOSING;

        if (!u->started || u->slice_job || u->service_job)
                return USER_OPENING;

        if (u->sessions) {
                bool all_closing = true;

                LIST_FOREACH(sessions_by_user, i, u->sessions) {
                        SessionState state;

                        state = session_get_state(i);
                        if (state == SESSION_ACTIVE)
                                return USER_ACTIVE;
                        if (state != SESSION_CLOSING)
                                all_closing = false;
                }

                return all_closing ? USER_CLOSING : USER_ONLINE;
        }

        if (user_check_linger_file(u) > 0)
                return USER_LINGERING;

        return USER_CLOSING;
}

int user_kill(User *u, int signo) {
        assert(u);

        return manager_kill_unit(u->manager, u->slice, KILL_ALL, signo, NULL);
}

static bool elect_display_filter(Session *s) {
        /* Return true if the session is a candidate for the user???s ???primary
         * session??? or ???display???. */
        assert(s);

        return (s->class == SESSION_USER && !s->stopping);
}

static int elect_display_compare(Session *s1, Session *s2) {
        /* Indexed by SessionType. Lower numbers mean more preferred. */
        const int type_ranks[_SESSION_TYPE_MAX] = {
                [SESSION_UNSPECIFIED] = 0,
                [SESSION_TTY] = -2,
                [SESSION_X11] = -3,
                [SESSION_WAYLAND] = -3,
                [SESSION_MIR] = -3,
                [SESSION_WEB] = -1,
        };

        /* Calculate the partial order relationship between s1 and s2,
         * returning < 0 if s1 is preferred as the user???s ???primary session???,
         * 0 if s1 and s2 are equally preferred or incomparable, or > 0 if s2
         * is preferred.
         *
         * s1 or s2 may be NULL. */
        if (!s1 && !s2)
                return 0;

        if ((s1 == NULL) != (s2 == NULL))
                return (s1 == NULL) - (s2 == NULL);

        if (s1->stopping != s2->stopping)
                return s1->stopping - s2->stopping;

        if ((s1->class != SESSION_USER) != (s2->class != SESSION_USER))
                return (s1->class != SESSION_USER) - (s2->class != SESSION_USER);

        if ((s1->type == _SESSION_TYPE_INVALID) != (s2->type == _SESSION_TYPE_INVALID))
                return (s1->type == _SESSION_TYPE_INVALID) - (s2->type == _SESSION_TYPE_INVALID);

        if (s1->type != s2->type)
                return type_ranks[s1->type] - type_ranks[s2->type];

        return 0;
}

void user_elect_display(User *u) {
        Session *s;

        assert(u);

        /* This elects a primary session for each user, which we call
         * the "display". We try to keep the assignment stable, but we
         * "upgrade" to better choices. */
        log_debug("Electing new display for user %s", u->name);

        LIST_FOREACH(sessions_by_user, s, u->sessions) {
                if (!elect_display_filter(s)) {
                        log_debug("Ignoring session %s", s->id);
                        continue;
                }

                if (elect_display_compare(s, u->display) < 0) {
                        log_debug("Choosing session %s in preference to %s", s->id, u->display ? u->display->id : "-");
                        u->display = s;
                }
        }
}

static const char* const user_state_table[_USER_STATE_MAX] = {
        [USER_OFFLINE] = "offline",
        [USER_OPENING] = "opening",
        [USER_LINGERING] = "lingering",
        [USER_ONLINE] = "online",
        [USER_ACTIVE] = "active",
        [USER_CLOSING] = "closing"
};

DEFINE_STRING_TABLE_LOOKUP(user_state, UserState);

int config_parse_tmpfs_size(
                const char* unit,
                const char *filename,
                unsigned line,
                const char *section,
                unsigned section_line,
                const char *lvalue,
                int ltype,
                const char *rvalue,
                void *data,
                void *userdata) {

        size_t *sz = data;
        int r;

        assert(filename);
        assert(lvalue);
        assert(rvalue);
        assert(data);

        /* First, try to parse as percentage */
        r = parse_percent(rvalue);
        if (r > 0 && r < 100)
                *sz = physical_memory_scale(r, 100U);
        else {
                uint64_t k;

                /* If the passed argument was not a percentage, or out of range, parse as byte size */

                r = parse_size(rvalue, 1024, &k);
                if (r < 0 || k <= 0 || (uint64_t) (size_t) k != k) {
                        log_syntax(unit, LOG_ERR, filename, line, r, "Failed to parse size value, ignoring: %s", rvalue);
                        return 0;
                }

                *sz = PAGE_ALIGN((size_t) k);
        }

        return 0;
}

int config_parse_user_tasks_max(
                const char* unit,
                const char *filename,
                unsigned line,
                const char *section,
                unsigned section_line,
                const char *lvalue,
                int ltype,
                const char *rvalue,
                void *data,
                void *userdata) {

        uint64_t *m = data;
        uint64_t k;
        int r;

        assert(filename);
        assert(lvalue);
        assert(rvalue);
        assert(data);

        if (isempty(rvalue)) {
                *m = system_tasks_max_scale(DEFAULT_USER_TASKS_MAX_PERCENTAGE, 100U);
                return 0;
        }

        if (streq(rvalue, "infinity")) {
                *m = CGROUP_LIMIT_MAX;
                return 0;
        }

        /* Try to parse as percentage */
        r = parse_percent(rvalue);
        if (r >= 0)
                k = system_tasks_max_scale(r, 100U);
        else {

                /* If the passed argument was not a percentage, or out of range, parse as byte size */

                r = safe_atou64(rvalue, &k);
                if (r < 0) {
                        log_syntax(unit, LOG_ERR, filename, line, r, "Failed to parse tasks maximum, ignoring: %s", rvalue);
                        return 0;
                }
        }

        if (k <= 0 || k >= UINT64_MAX) {
                log_syntax(unit, LOG_ERR, filename, line, 0, "Tasks maximum out of range, ignoring: %s", rvalue);
                return 0;
        }

        *m = k;
        return 0;
}

/****************************************************************************
 * libs/libc/wqueue/work_usrthread.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <assert.h>

#include <nuttx/clock.h>
#include <nuttx/list.h>
#include <nuttx/wqueue.h>

#include "wqueue/wqueue.h"

#if defined(CONFIG_LIBC_USRWORK) && !defined(__KERNEL__)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef CONFIG_SYSTEM_TIME64
#  define WORK_DELAY_MAX UINT64_MAX
#else
#  define WORK_DELAY_MAX UINT32_MAX
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/* The state of the user mode work queue. */

struct usr_wqueue_s g_usrwork =
{
  LIST_INITIAL_VALUE(g_usrwork.q),
  NXMUTEX_INITIALIZER,
  SEM_INITIALIZER(0),
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: work_process
 *
 * Description:
 *   This is the logic that performs actions placed on any work list.  This
 *   logic is the common underlying logic to all work queues.  This logic is
 *   part of the internal implementation of each work queue; it should not
 *   be called from application level logic.
 *
 * Input Parameters:
 *   wqueue - Describes the work queue to be processed
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

static void work_process(FAR struct usr_wqueue_s *wqueue)
{
  FAR struct work_s *work;
  worker_t worker;
  FAR void *arg;
  clock_t tick;
  clock_t next;
  int ret;

  /* Then process queued work.  Lock the work queue while we process items
   * in the work list.
   */

  next = WORK_DELAY_MAX;
  ret = nxmutex_lock(&wqueue->lock);
  if (ret < 0)
    {
      /* Break out earlier if we were awakened by a signal */

      return;
    }

  /* And check each entry in the work queue.  Since we have locked the
   * work queue we know:  (1) we will not be suspended unless we do
   * so ourselves, and (2) there will be no changes to the work queue
   */

  while (!list_is_empty(&wqueue->q))
    {
      work = list_first_entry(&wqueue->q, struct work_s, node);

      /* Is this work ready? It is ready if there is no delay or if
       * the delay has elapsed.  is the time that the work was added
       * to the work queue. Therefore a delay of equal or less than
       * zero will always execute immediately.
       */

      tick = clock();

      /* Is this delay work ready? */

      if (clock_compare(work->qtime, tick))
        {
          /* Remove the ready-to-execute work from the list */

          list_delete(&work->node);

          /* Extract the work description from the entry (in case the work
           * instance by the reused after it has been de-queued).
           */

          worker = work->worker;

          /* Check for a race condition where the work may be nullified
           * before it is removed from the queue.
           */

          if (worker != NULL)
            {
              /* Extract the work argument before unlocking the work queue */

              arg = work->arg;

              /* Mark the work as no longer being queued */

              work->worker = NULL;

              /* Do the work.  Unlock the work queue while the work is being
               * performed... we don't have any idea how long this will take!
               */

              nxmutex_unlock(&wqueue->lock);
              worker(arg);

              /* Now, unfortunately, since we unlocked the work queue we
               * don't know the state of the work list and we will have to
               * start back at the head of the list.
               */

              ret = nxmutex_lock(&wqueue->lock);
              if (ret < 0)
                {
                  /* Break out earlier if we were awakened by a signal */

                  return;
                }
            }
        }
      else
        {
          next = work->qtime - clock();
          break;
        }
    }

  /* Unlock the work queue before waiting. */

  nxmutex_unlock(&wqueue->lock);

  if (next == WORK_DELAY_MAX)
    {
      /* Wait indefinitely until work_queue has new items */

      nxsem_wait(&wqueue->wake);
    }
  else
    {
      struct timespec now;
      struct timespec delay;
      struct timespec rqtp;

      /* Wait awhile to check the work list.  We will wait here until
       * either the time elapses or until we are awakened by a semaphore.
       * Interrupts will be re-enabled while we wait.
       */

      clock_gettime(CLOCK_REALTIME, &now);
      clock_ticks2time(&delay, next);
      clock_timespec_add(&now, &delay, &rqtp);

      nxsem_timedwait(&wqueue->wake, &rqtp);
    }
}

/****************************************************************************
 * Name: work_usrthread
 *
 * Description:
 *   This is the worker thread that performs the actions placed on the user
 *   work queue.
 *
 *   This is a user mode work queue.  It must be used by applications for
 *   miscellaneous operations.  The user work thread must be started by
 *   application start-up logic by calling work_usrstart().
 *
 * Input Parameters:
 *   argc, argv (not used)
 *
 * Returned Value:
 *   Does not return
 *
 ****************************************************************************/

#ifdef CONFIG_BUILD_PROTECTED
static int work_usrthread(int argc, char *argv[])
#else
static pthread_addr_t work_usrthread(pthread_addr_t arg)
#endif
{
  /* Loop forever */

  for (; ; )
    {
      /* Then process queued work.  We need to keep the work queue locked
       * while we process items in the work list.
       */

      work_process(&g_usrwork);
    }

#ifdef CONFIG_BUILD_PROTECTED
  return OK; /* To keep some compilers happy */
#else
  return NULL; /* To keep some compilers happy */
#endif
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: work_usrstart
 *
 * Description:
 *   Start the user mode work queue.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   The task ID of the worker thread is returned on success.  A negated
 *   errno value is returned on failure.
 *
 ****************************************************************************/

int work_usrstart(void)
{
  int ret;
#ifndef CONFIG_BUILD_PROTECTED
  pthread_t usrwork;
  pthread_attr_t attr;
  struct sched_param param;
#endif

  /* Initialize the work queue */

  list_initialize(&g_usrwork.q);

#ifdef CONFIG_BUILD_PROTECTED

  /* Start a user-mode worker thread for use by applications. */

  ret = task_create("uwork",
                    CONFIG_LIBC_USRWORKPRIORITY,
                    CONFIG_LIBC_USRWORKSTACKSIZE,
                    work_usrthread, NULL);
  if (ret < 0)
    {
      int errcode = get_errno();
      DEBUGASSERT(errcode > 0);
      return -errcode;
    }

  return ret;
#else
  /* Start a user-mode worker thread for use by applications. */

  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, CONFIG_LIBC_USRWORKSTACKSIZE);

  pthread_attr_getschedparam(&attr, &param);
  param.sched_priority = CONFIG_LIBC_USRWORKPRIORITY;
  pthread_attr_setschedparam(&attr, &param);

  ret = pthread_create(&usrwork, &attr, work_usrthread, NULL);
  if (ret != 0)
    {
      return -ret;
    }

  /* Detach because the return value and completion status will not be
   * requested.
   */

  pthread_detach(usrwork);

  return (pid_t)usrwork;
#endif
}

#endif /* CONFIG_LIBC_USRWORK && !__KERNEL__*/

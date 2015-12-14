
/*
 * Copyright (C) Niklaus F.Schen.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "mln_connection.h"
#include "mln_log.h"
#include "mln_file.h"
#if defined(MLN_WRITEV)
#include <sys/uio.h>
#endif
#if defined(MLN_SENDFILE)
#include <sys/sendfile.h>
#endif


static inline int mln_fd_is_nonblock(int fd);
static inline mln_chain_t *
mln_tcp_conn_pop_inline(mln_tcp_conn_t *tc, int type);
static inline int
mln_tcp_conn_recv_chain(mln_tcp_conn_t *tc, mln_u32_t flag);
static inline int
mln_tcp_conn_recv_chain_file(int sockfd, \
                             mln_alloc_t *pool, \
                             mln_buf_t *b, \
                             mln_buf_t *last);
static inline int
mln_tcp_conn_recv_chain_mem(int sockfd, mln_alloc_t *pool, mln_buf_t *b);
static inline ssize_t
mln_tcp_conn_send_chain_memory(mln_tcp_conn_t *tc);
static inline ssize_t
mln_tcp_conn_send_chain_file(mln_tcp_conn_t *tc);


static inline int mln_fd_is_nonblock(int fd)
{
    int flg;
    if ((flg = fcntl(fd, F_GETFL, NULL)) < 0) {
        mln_log(error, "fcntl F_GETFL failed. %s\n", strerror(errno));
        abort();
    }
    return flg & O_NONBLOCK;
}


/*
 * mln_tcp_conn_t
 */

int mln_tcp_conn_init(mln_tcp_conn_t *tc, int sockfd)
{
    tc->pool = mln_alloc_init();
    if (tc->pool == NULL) return -1;
    tc->rcv_head = tc->rcv_tail = NULL;
    tc->snd_head = tc->snd_tail = NULL;
    tc->sent_head = tc->sent_tail = NULL;
    tc->sockfd = sockfd;
    return 0;
}

void mln_tcp_conn_destroy(mln_tcp_conn_t *tc)
{
    if (tc == NULL) return;

    mln_chain_pool_release_all(mln_tcp_conn_remove(tc, M_C_SEND));
    mln_chain_pool_release_all(mln_tcp_conn_remove(tc, M_C_RECV));
    mln_chain_pool_release_all(mln_tcp_conn_remove(tc, M_C_SENT));
    mln_alloc_destroy(tc->pool);
}

void mln_tcp_conn_append_chain(mln_tcp_conn_t *tc, mln_chain_t *c_head, mln_chain_t *c_tail, int type)
{
    if (c_head == NULL) return;

    mln_chain_t **head, **tail;
    if (type == M_C_SEND) {
        head = &(tc->snd_head);
        tail = &(tc->snd_tail);
    } else if (type == M_C_RECV) {
        head = &(tc->rcv_head);
        tail = &(tc->rcv_tail);
    } else if (type == M_C_SENT) {
        head = &(tc->sent_head);
        tail = &(tc->sent_tail);
    } else {
        mln_log(error, "flag error.\n");
        abort();
    }

    if (c_tail == NULL) {
        for (c_tail = c_head; c_tail->next != NULL; c_tail = c_tail->next)
            ;
    }
    if (*head == NULL) {
        *head = c_head;
        *tail = c_tail;
    } else {
        (*tail)->next = c_head;
        *tail = c_tail;
    }
}

void mln_tcp_conn_append(mln_tcp_conn_t *tc, mln_chain_t *c, int type)
{
    mln_chain_t **head, **tail;
    if (type == M_C_SEND) {
        head = &(tc->snd_head);
        tail = &(tc->snd_tail);
    } else if (type == M_C_RECV) {
        head = &(tc->rcv_head);
        tail = &(tc->rcv_tail);
    } else if (type == M_C_SENT) {
        head = &(tc->sent_head);
        tail = &(tc->sent_tail);
    } else {
        mln_log(error, "flag error.\n");
        abort();
    }

    if (*head == NULL) {
        *head = *tail = c;
    } else {
        (*tail)->next = c;
        *tail = c;
    }
}

mln_chain_t *mln_tcp_conn_get_head(mln_tcp_conn_t *tc, int type)
{
    mln_chain_t *rc = NULL;

    if (type == M_C_SEND) {
        rc = tc->snd_head;
    } else if (type == M_C_RECV) {
        rc = tc->rcv_head;
    } else if (type == M_C_SENT) {
        rc = tc->sent_head;
    } else {
        mln_log(error, "flag error.\n");
        abort();
    }

    return rc;
}

mln_chain_t *mln_tcp_conn_remove(mln_tcp_conn_t *tc, int type)
{
    mln_chain_t *rc;

    if (type == M_C_SEND) {
        rc = tc->snd_head;
        tc->snd_head = tc->snd_tail = NULL;
    } else if (type == M_C_RECV) {
        rc = tc->rcv_head;
        tc->rcv_head = tc->rcv_tail = NULL;
    } else if (type == M_C_SENT) {
        rc = tc->sent_head;
        tc->sent_head = tc->sent_tail = NULL;
    } else {
        mln_log(error, "flag error.\n");
        abort();
    }

    return rc;
}

mln_chain_t *mln_tcp_conn_pop(mln_tcp_conn_t *tc, int type)
{
    mln_chain_t **head, **tail;
    if (type == M_C_SEND) {
        head = &(tc->snd_head);
        tail = &(tc->snd_tail);
    } else if (type == M_C_RECV) {
        head = &(tc->rcv_head);
        tail = &(tc->rcv_tail);
    } else if (type == M_C_SENT) {
        head = &(tc->sent_head);
        tail = &(tc->sent_tail);
    } else {
        mln_log(error, "flag error.\n");
        abort();
    }

    mln_chain_t *rc = *head;
    if (rc == *tail) {
        *head = *tail = NULL;
        return rc;
    }

    *head = rc->next;
    rc->next = NULL;
    return rc;
}

mln_chain_t *mln_tcp_conn_get_tail(mln_tcp_conn_t *tc, int type)
{
    mln_chain_t *rc;

    if (type == M_C_SEND) {
        rc = tc->snd_tail;
    } else if (type == M_C_RECV) {
        rc = tc->rcv_tail;
    } else if (type == M_C_SENT) {
        rc = tc->sent_tail;
    } else {
        mln_log(error, "flag error.\n");
        abort();
    }

    return rc;
}

int mln_tcp_conn_send(mln_tcp_conn_t *tc)
{
    ssize_t n;

    if (tc->snd_head == NULL) return M_C_NOTYET;

me:
    n = mln_tcp_conn_send_chain_memory(tc);
    if (n == 0 && \
        tc->snd_head != NULL && \
        tc->snd_head->buf != NULL && \
        tc->snd_head->buf->in_file)
    {
        goto fi;
    }
    if (n == 0) return M_C_NOTYET;
    if (n > 0) return M_C_FINISH;
    return M_C_ERROR;

fi:
    n = mln_tcp_conn_send_chain_file(tc);
    if (n == 0 && \
        tc->snd_head != NULL && \
        tc->snd_head->buf != NULL && \
        tc->snd_head->buf->in_memory)
    {
        goto me;
    }
    if (n == 0) return M_C_NOTYET;
    if (n > 0) return M_C_FINISH;
    return M_C_ERROR;
}


#if defined(MLN_WRITEV)
static inline ssize_t
mln_tcp_conn_send_chain_memory(mln_tcp_conn_t *tc)
{
    mln_chain_t *c;
    mln_buf_t *b;
    ssize_t n, is_done = 0;
    register mln_size_t buf_left_size;
    int proc_vec, nvec = 256;
    struct iovec vector[256];

    if (mln_fd_is_nonblock(tc->sockfd)) {
        while (1) {
            proc_vec = 0;
            for (c = tc->snd_head; c != NULL; c = c->next) {
                if (proc_vec >= nvec) break;
                if ((b = c->buf) == NULL) continue;
                if (!b->in_memory) break;
                buf_left_size = mln_buf_left_size(b);
                if (buf_left_size) {
                    vector[proc_vec].iov_base = b->send_pos;
                    vector[proc_vec].iov_len = buf_left_size;
                    proc_vec++;
                }
                if (b->last_in_chain) break;
            }

            if (!proc_vec) return 0;

non:
            n = writev(tc->sockfd, vector, proc_vec);
            if (n <= 0) {
                if (errno == EINTR) goto non;
                if (errno == EAGAIN) return 0;
                return -1;
            }

            while ((c = tc->snd_head) != NULL) {
                if ((b = c->buf) == NULL) {
                    c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
                    mln_tcp_conn_append(tc, c, M_C_SENT);
                    continue;
                }
                if (!b->in_memory) break;
                if (b->last_in_chain) is_done = 1;

                buf_left_size = mln_buf_left_size(b);
                if (n >= buf_left_size) {
                    b->send_pos += buf_left_size;
                    n -= buf_left_size;
                    c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
                    mln_tcp_conn_append(tc, c, M_C_SENT);
                } else {
                    b->send_pos += n;
                    n = 0;
                }
                if (is_done || n == 0) break;
            }

            if (is_done) break;
        }
        return 1;
    }

    proc_vec = 0;
    for (c = tc->snd_head; c != NULL; c = c->next) {
        if (proc_vec >= nvec) break;
        if ((b = c->buf) == NULL) continue;
        if (!b->in_memory) break;
        buf_left_size = mln_buf_left_size(b);
        if (buf_left_size) {
            vector[proc_vec].iov_base = b->send_pos;
            vector[proc_vec].iov_len = buf_left_size;
            proc_vec++;
        }
        if (b->last_in_chain) break;
    }

    if (!proc_vec) return 0;

blk:
    n = writev(tc->sockfd, vector, proc_vec);
    if (n <= 0) {
        if (errno == EINTR) goto blk;
        return -1;
    }

    while ((c = tc->snd_head) != NULL) {
        if ((b = c->buf) == NULL) {
            c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
            mln_tcp_conn_append(tc, c, M_C_SENT);
            continue;
        }
        if (!b->in_memory) break;
        if (b->last_in_chain) is_done = 1;

        buf_left_size = mln_buf_left_size(b);
        if (n >= buf_left_size) {
            b->send_pos += buf_left_size;
            n -= buf_left_size;
            c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
            mln_tcp_conn_append(tc, c, M_C_SENT);
        } else {
            b->send_pos += n;
            n = 0;
        }
        if (is_done || n == 0) break;
    }

    return is_done;
}
#else
static inline ssize_t
mln_tcp_conn_send_chain_memory(mln_tcp_conn_t *tc)
{
    mln_u8_t buf[8192], *p;
    mln_chain_t *c;
    mln_buf_t *b;
    mln_size_t left_size;
    register mln_size_t buf_left_size;
    ssize_t n, is_done = 0;

    if (mln_fd_is_nonblock(tc->sockfd)) {
        while (1) {
            p = buf;
            left_size = sizeof(buf);

            for (c = tc->snd_head; c != NULL; c = c->next) {
                if ((b = c->buf) == NULL) continue;
                if (!b->in_memory) break;
                buf_left_size = mln_buf_left_size(b);

                if (buf_left_size > left_size) {
                    memcpy(p, b->send_pos, left_size);
                    p += left_size;
                    left_size = 0; 
                    break;
                } else {
                    if (buf_left_size > 0) {
                        memcpy(p, b->send_pos, buf_left_size);
                        p += buf_left_size;
                        left_size -= buf_left_size;
                    }
                    if (b->last_in_chain) break;
                }
            }

            if (left_size == sizeof(buf)) return 0;

non:
            n = send(tc->sockfd, buf, sizeof(buf) - left_size, 0);
            if (n <= 0) {
                if (errno == EINTR) goto non;
                if (errno == EAGAIN) return 0;
                return -1;
            }

            while ((c = tc->snd_head) != NULL) {
                if ((b = c->buf) == NULL) {
                    c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
                    mln_tcp_conn_append(tc, c, M_C_SENT);
                    continue;
                }
                buf_left_size = mln_buf_left_size(b);
                if (buf_left_size > n) {
                    b->send_pos += n;
                    n = 0;
                } else {
                    if (b->last_in_chain) is_done = 1;
                    n -= buf_left_size;
                    b->send_pos += buf_left_size;
                    c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
                    mln_tcp_conn_append(tc, c, M_C_SENT);
                }
                if (is_done || n == 0) break;
            }

            if (is_done) break;
        }

        return 1;
    }

    p = buf;
    left_size = sizeof(buf);

    for (c = tc->snd_head; c != NULL; c = c->next) {
        if ((b = c->buf) == NULL) continue;
        if (!b->in_memory) break;
        buf_left_size = mln_buf_left_size(b);

        if (buf_left_size > left_size) {
            memcpy(p, b->send_pos, left_size);
            p += left_size;
            left_size = 0;
            break;
        } else {
            if (buf_left_size > 0) {
                memcpy(p, b->send_pos, buf_left_size);
                p += buf_left_size;
                left_size -= buf_left_size;
            }
            if (b->last_in_chain) break;
        }
    }

    if (left_size == sizeof(buf)) return 0;

blk:
    n = send(tc->sockfd, buf, sizeof(buf) - left_size, 0);
    if (n <= 0) {
        if (errno == EINTR) goto blk;
        return -1;
    }

    while ((c = tc->snd_head) != NULL) {
        if ((b = c->buf) == NULL) {
            c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
            mln_tcp_conn_append(tc, c, M_C_SENT);
            continue;
        }
        buf_left_size = mln_buf_left_size(b);
        if (buf_left_size > n) {
            b->send_pos += n;
            n = 0;
        } else {
            if (b->last_in_chain) is_done = 1;
            n -= buf_left_size;
            b->send_pos += buf_left_size;
            c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
            mln_tcp_conn_append(tc, c, M_C_SENT);
        }
        if (is_done || n == 0) break;
    }

    return is_done;
}
#endif


#if defined(MLN_SENDFILE)
static inline ssize_t
mln_tcp_conn_send_chain_file(mln_tcp_conn_t *tc)
{
    int sockfd = tc->sockfd;
    ssize_t n, is_done = 0;
    mln_chain_t *c;
    mln_buf_t *b;
    mln_size_t buf_left_size;

    if (mln_fd_is_nonblock(sockfd)) {
        while ((c = tc->snd_head) != NULL) {
            if ((b = c->buf) == NULL) {
                c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
                mln_tcp_conn_append(tc, c, M_C_SENT);
                continue;
            }
            if (!b->in_file) break;
            buf_left_size = mln_buf_left_size(b);
            if (b->last_in_chain) is_done = 1;

            if (buf_left_size) {
non:
                n = sendfile(sockfd, \
                             mln_file_fd(b->file), \
                             &b->file_send_pos, \
                             buf_left_size);
                if (n <= 0) {
                    if (errno == EINTR) goto non;
                    if (errno == EAGAIN) return 0;
                    return -1;
                }

                if (mln_buf_left_size(b)) {
                    goto non;
                }
            }
            c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
            mln_tcp_conn_append(tc, c, M_C_SENT);
            if (is_done) break;
        }
        return 1;
    }

    while ((c = tc->snd_head) != NULL) {
        if ((b = c->buf) != NULL) {
            if (mln_buf_left_size(b)) break;
            if (b->last_in_chain) is_done = 1;
        }
        c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
        mln_tcp_conn_append(tc, c, M_C_SENT);
        if (is_done) return 1;
    }
    if (tc->snd_head == NULL) return 0;
    if (!b->in_file) return 0;

blk:
    n = sendfile(sockfd, \
                 mln_file_fd(b->file), \
                 &b->file_send_pos, \
                 mln_buf_left_size(b));
    if (n <= 0) {
        if (errno == EINTR) goto blk;
        return -1;
    }

    if (mln_buf_left_size(b)) goto blk;
    if (b->last_in_chain) is_done = 1;
    c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
    mln_tcp_conn_append(tc, c, M_C_SENT);

    return is_done;
}
#else
static inline ssize_t
mln_tcp_conn_send_chain_file(mln_tcp_conn_t *tc)
{
    int sockfd = tc->sockfd;
    ssize_t n;
    mln_buf_t *b;
    mln_chain_t *c;
    mln_u8_t buf[4096];
    mln_size_t len, buf_left_size;

    if (mln_fd_is_nonblock(sockfd)) {
        while ((c = tc->snd_head) != NULL) {
            if ((b = c->buf) == NULL) {
                c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
                mln_tcp_conn_append(tc, c, M_C_SENT);
                continue;
            }
            if (!b->in_file) break;

            buf_left_size = mln_buf_left_size(b);
            if (buf_left_size == 0) {
                c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
                mln_tcp_conn_append(tc, c, M_C_SENT);
                if (b->last_in_chain) return 1;
                continue;
            }

            lseek(mln_file_fd(b->file), b->file_send_pos, SEEK_SET);
            len = buf_left_size > sizeof(buf)? sizeof(buf): buf_left_size;
non_rd:
            n = read(mln_file_fd(b->file), buf, len);
            if (n <= 0) {
                if (errno == EINTR) goto non_rd;
                return -1;
            }

            len = n;
non_snd:
            n = send(sockfd, buf, len, 0);
            if (n <= 0) {
                if (errno == EINTR) goto non_snd;
                if (errno == EAGAIN) return 0;
                return -1;
            }
            b->file_send_pos += n;
            if (mln_buf_left_size(b)) continue;

            c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
            mln_tcp_conn_append(tc, c, M_C_SENT);
            if (b->last_in_chain) return 1;
        }
        return 0;
    }

    while ((c = tc->snd_head) != NULL) {
        if ((b = c->buf) == NULL) {
            c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
            mln_tcp_conn_append(tc, c, M_C_SENT);
            continue;
        }
        if (!b->in_file) return 0;

        buf_left_size = mln_buf_left_size(b);
        if (buf_left_size == 0) {
            c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
            mln_tcp_conn_append(tc, c, M_C_SENT);
            if (b->last_in_chain) return 1;
            continue;
        }

        break;
    }
    if (c == NULL) return 0;

    lseek(mln_file_fd(b->file), b->file_send_pos, SEEK_SET);
    len = buf_left_size > sizeof(buf)? sizeof(buf): buf_left_size;
blk_rd:
    n = read(mln_file_fd(b->file), buf, len);
    if (n <= 0) {
        if (errno == EINTR) goto blk_rd;
        return -1;
    }

    len = n;
blk_snd:
    n = send(sockfd, buf, len, 0);
    if (n <= 0) {
        if (errno == EINTR) goto blk_snd;
        return -1;
    }
    b->file_send_pos += n;
    if (mln_buf_left_size(b)) return 0;

    c = mln_tcp_conn_pop_inline(tc, M_C_SEND);
    mln_tcp_conn_append(tc, c, M_C_SENT);

    return b->last_in_chain == 0? 0: 1;
}
#endif

static inline mln_chain_t *
mln_tcp_conn_pop_inline(mln_tcp_conn_t *tc, int type)
{
    mln_chain_t **head, **tail;
    if (type == M_C_SEND) {
        head = &(tc->snd_head);
        tail = &(tc->snd_tail);
    } else if (type == M_C_RECV) {
        head = &(tc->rcv_head);
        tail = &(tc->rcv_tail);
    } else if (type == M_C_SENT) {
        head = &(tc->sent_head);
        tail = &(tc->sent_tail);
    } else {
        mln_log(error, "flag error.\n");
        abort();
    }

    mln_chain_t *rc = *head;
    if (rc == *tail) {
        *head = *tail = NULL;
        return rc;
    }

    *head = rc->next;
    rc->next = NULL;
    return rc;
}

int mln_tcp_conn_recv(mln_tcp_conn_t *tc, mln_u32_t flag)
{
    if (flag != M_C_TYPE_MEMORY && flag != M_C_TYPE_FILE) {
        mln_log(error, "Flag error.\n");
        abort();
    }

    int n;

    if (mln_fd_is_nonblock(tc->sockfd)) {
goon_non:
        while ((n = mln_tcp_conn_recv_chain(tc, flag)) > 0) {
            /*do nothing*/
        }
    } else {
goon_blk:
        if ((n = mln_tcp_conn_recv_chain(tc, flag)) > 0) {
            return M_C_NOTYET;
        }
    }

    if (n == 0) {
        return M_C_CLOSED;
    }

    if (errno == EINTR) {
        if (mln_fd_is_nonblock(tc->sockfd)) {
            goto goon_non;
        } else {
            goto goon_blk;
        }
    } else if (errno == EAGAIN) {
        return M_C_NOTYET;
    }
    return M_C_ERROR;
}

static inline int
mln_tcp_conn_recv_chain(mln_tcp_conn_t *tc, mln_u32_t flag)
{
    mln_buf_t *last = NULL;
    int n = -1;
    mln_buf_t *b;
    mln_chain_t *c;
    mln_alloc_t *pool = mln_tcp_conn_get_pool(tc);

    c = mln_chain_new(pool);
    b = mln_buf_new(pool);
    if (c == NULL || b == NULL) {
        errno = ENOMEM;
        return -1;
    }
    c->buf = b;

    if (flag & M_C_TYPE_FILE) {
        if (flag & M_C_TYPE_FOLLOW && tc->rcv_tail != NULL && tc->rcv_tail->buf != NULL) {
            last = tc->rcv_tail->buf;
            if (!last->in_file) {
                last = NULL;
            }
        }
        n = mln_tcp_conn_recv_chain_file(tc->sockfd, pool, b, last);
    } else if (flag & M_C_TYPE_MEMORY) {
        n = mln_tcp_conn_recv_chain_mem(tc->sockfd, pool, b);
    } else {
        mln_log(error, "Flag error.\n");
        abort();
    }

    if (n <= 0) {
        mln_chain_pool_release(c);
    } else {
        mln_tcp_conn_append(tc, c, M_C_RECV);
    }

    return n;
}

static inline int
mln_tcp_conn_recv_chain_file(int sockfd, \
                             mln_alloc_t *pool, \
                             mln_buf_t *b, \
                             mln_buf_t *last)
{
    int n;
    mln_u8_t buf[1024];

    n = recv(sockfd, buf, sizeof(buf), 0);
    if (n <= 0) return n;

    if (last == NULL) {
        if ((b->file = mln_file_tmp_open(pool)) == NULL) {
            return -1;
        }
        b->file_send_pos = b->file_pos = 0;
    } else {
        b->file_send_pos = b->file_pos = last->file_last;
        b->file = last->file;
        last->shadow = b;
    }
    b->file_last = b->file_pos + n;
    b->in_file = 1;
    b->last_buf = 1;

    if (write(mln_file_fd(b->file), buf, n) < 0) {
        return -1;
    }

    return n;
}

static inline int
mln_tcp_conn_recv_chain_mem(int sockfd, mln_alloc_t *pool, mln_buf_t *b)
{
    mln_u8ptr_t buf;
    int n;

    buf = (mln_u8ptr_t)mln_alloc_m(pool, 1024);
    if (buf == NULL) {
        errno = ENOMEM;
        return -1;
    }

    n = recv(sockfd, buf, 1024, 0);
    if (n <= 0) {
        mln_alloc_free(buf);
        return n;
    }

    b->send_pos = b->pos = b->start = buf;
    b->last = b->end = buf + n;
    b->in_memory = 1;
    b->last_buf = 1;

    return n;
}


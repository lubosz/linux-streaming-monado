// Copyright 2020, Collabora, Ltd.
// SPDX-License-Identifier: BSL-1.0
/*!
 * @file
 * @brief  Server helper functions.
 * @author Pete Black <pblack@collabora.com>
 * @author Jakob Bornecrantz <jakob@collabora.com>
 * @ingroup ipc_server
 */

#include "ipc_server_utils.h"

#include "util/u_misc.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>


/*
 *
 * Actual senders
 *
 */

int
ipc_reply(int socket, void *data, size_t len)
{
	struct msghdr msg = {0};
	struct iovec iov = {0};

	iov.iov_base = data;
	iov.iov_len = len;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;

	ssize_t ret = sendmsg(socket, &msg, MSG_NOSIGNAL);
	if (ret < 0) {
		printf(
		    "sending plain message on socket %d failed with error: "
		    "%s\n",
		    socket, strerror(errno));
	}

	return ret;
}

int
ipc_reply_fds(int socket, void *data, size_t size, int *fds, uint32_t num_fds)
{
	uint8_t cmsgbuf[CMSG_SPACE(sizeof(int) * num_fds)];

	struct iovec iov = {0};
	iov.iov_base = data;
	iov.iov_len = size;

	struct msghdr msg = {0};
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_flags = 0;
	msg.msg_control = cmsgbuf;
	msg.msg_controllen = CMSG_LEN(sizeof(int) * num_fds);

	struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(int) * num_fds);

	memcpy(CMSG_DATA(cmsg), fds, num_fds * sizeof(int));

	ssize_t ret = sendmsg(socket, &msg, MSG_NOSIGNAL);
	if (ret < 0) {
		printf("sending %d FDs on socket %d failed with error: %s\n",
		       num_fds, socket, strerror(errno));
	}

	return ret;
}

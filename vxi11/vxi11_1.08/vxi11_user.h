/* Revision history: */
/* $Id: vxi11_user.h,v 1.9 2007/07/11 14:20:56 sds Exp $ */
/*
 * $Log: vxi11_user.h,v $
 * Revision 1.9  2007/07/11 14:20:56  sds
 * removed #include <iostream> as not needed
 * removed using namespace std
 *
 * Revision 1.8  2007/07/10 13:54:11  sds
 * Added extra function:
 * int     vxi11_open_device(char *ip, CLINK *clink, char *device);
 * This replaces the original vxi11_open_device fn, which did not pass
 * a char *device. Wrapper fn used for backwards compatibility.
 *
 * Revision 1.7  2007/07/10 11:20:43  sds
 * removed the following function:
 * int   vxi11_open_link(CLIENT **client, VXI11_LINK **link);
 * ...since it was no longer needed, following the patch by
 * Robert Larice.
 *
 * Revision 1.6  2006/12/08 11:47:14  ijc
 * error on last ci, sorted.
 *
 * Revision 1.5  2006/12/08 11:45:29  ijc
 * added #define VXI11_NULL_READ_RESP
 *
 * Revision 1.4  2006/12/07 12:26:17  sds
 * added VXI11_NULL_READ_RESP #define
 *
 * Revision 1.3  2006/07/06 13:03:28  sds
 * Surrounded the whole header with #ifndef __VXI11_USER__.
 * Added a couple of vxi11_open_link() fns and a vxi11_close_link() fn, to
 * separate the link stuff from the client stuff.
 *
 * Revision 1.2  2006/06/26 12:42:54  sds
 * Introduced a new CLINK structure, to reduce the number of arguments
 * passed to functions. Wrote wrappers for open(), close(), send()
 * and receieve() functions, then adjusted all the other functions built
 * on those to make use of the CLINK structure.
 *
 * Revision 1.1  2006/06/26 10:36:02  sds
 * Initial revision
 *
 */

/* vxi11_user.h
 * Copyright (C) 2006 Steve D. Sharples
 *
 * User library for opening, closing, sending to and receiving from
 * a device enabled with the VXI11 RPC ethernet protocol. Uses the files
 * generated by rpcgen vxi11.x.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The author's email address is steve.sharples@nottingham.ac.uk
 */

#ifndef	__VXI11_USER__
#define	__VXI11_USER__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rpc/rpc.h>
#include "vxi11.h"

#define	VXI11_DEFAULT_TIMEOUT	10000	/* in ms */
#define	VXI11_READ_TIMEOUT	2000	/* in ms */
#define	VXI11_CLIENT		CLIENT
#define	VXI11_LINK		Create_LinkResp
#define	VXI11_MAX_CLIENTS	256	/* maximum no of unique IP addresses/clients */
#define	VXI11_NULL_READ_RESP	50	/* vxi11_receive() return value if a query
					 * times out ON THE INSTRUMENT (and so we have
					 * to resend the query again) */
#define	VXI11_NULL_WRITE_RESP	51	/* vxi11_send() return value if a sent command
					 * times out ON THE INSTURMENT. */

struct	CLINK {
	VXI11_CLIENT	*client;
	VXI11_LINK	*link;
	} ;
typedef	struct	CLINK CLINK;

/* The four main functions: open, close, send, receieve (plus a couple of wrappers) */
/* In fact all 6 of these are wrappers to the original functions listed at the
 * bottom, that use separate CLIENT and VXI11_LINK structures. It was easier to 
 * write wrappers for these functions than to re-write the original functions
 * themselves. These are the 4 (or 6 if you like) key user functions that you
 * should probably be using. They all use the CLINK structure. */
int	vxi11_open_device(const char *ip, CLINK *clink);
int	vxi11_open_device(const char *ip, CLINK *clink, char *device);
int	vxi11_close_device(const char *ip, CLINK *clink);
int	vxi11_send(CLINK *clink, const char *cmd);
int	vxi11_send(CLINK *clink, const char *cmd, unsigned long len);
long	vxi11_receive(CLINK *clink, char *buffer, unsigned long len);
long	vxi11_receive(CLINK *clink, char *buffer, unsigned long len, unsigned long timeout);

/* Utility functions, that use send() and receive(). Use these too. */
int	vxi11_send_data_block(CLINK *clink, char *cmd, char *buffer, unsigned long len);
long	vxi11_receive_data_block(CLINK *clink, char *buffer, unsigned long len, unsigned long timeout);
long	vxi11_send_and_receive(CLINK *clink, char *cmd, char *buf, unsigned long buf_len, unsigned long timeout);
long	vxi11_obtain_long_value(CLINK *clink, char *cmd, unsigned long timeout);
double	vxi11_obtain_double_value(CLINK *clink, char *cmd, unsigned long timeout);
long	vxi11_obtain_long_value(CLINK *clink, char *cmd);
double	vxi11_obtain_double_value(CLINK *link, char *cmd);

/* When I first wrote this library I used separate client and links. I've
 * retained the original functions and just written clink wrappers for them
 * (see above) as it's perhaps a little clearer this way. Probably not worth
 * delving this deep in use, but it's where the real nitty gritty is. */
int	vxi11_open_device(const char *ip, CLIENT **client, VXI11_LINK **link, char *device);
int	vxi11_open_link(const char *ip, CLIENT **client, VXI11_LINK **link, char *device);
int	vxi11_close_device(const char *ip, CLIENT *client, VXI11_LINK *link);
int	vxi11_close_link(const char *ip, CLIENT *client, VXI11_LINK *link);
int	vxi11_send(CLIENT *client, VXI11_LINK *link, const char *cmd);
int	vxi11_send(CLIENT *client, VXI11_LINK *link, const char *cmd, unsigned long len);
long	vxi11_receive(CLIENT *client, VXI11_LINK *link, char *buffer, unsigned long len);
long	vxi11_receive(CLIENT *client, VXI11_LINK *link, char *buffer, unsigned long len, unsigned long timeout);

#endif
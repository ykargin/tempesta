/**
 *		Tempesta FW
 *
 * Copyright (C) 2019 Tempesta Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef __HTTP_STREAM__
#define __HTTP_STREAM__

#include <linux/rbtree.h>

#define TFW_STREAMS_TBL_HBITS	10

/**
 * States for HTTP/2 streams processing.
 */
typedef enum {
	HTTP2_STREAM_IDLE,
	HTTP2_STREAM_LOC_RESERVED,
	HTTP2_STREAM_REM_RESERVED,
	HTTP2_STREAM_OPENED,
	HTTP2_STREAM_LOC_HALF_CLOSED,
	HTTP2_STREAM_REM_HALF_CLOSED,
	HTTP2_STREAM_CLOSED
} TfwStreamState;

/**
 * Representation of HTTP/2 stream entity.
 *
 * @node	- entry in per-connection storage of streams (red-black tree);
 * @id		- stream ID;
 * @state	- stream's current state;
 * @weight	- stream's priority weight;
 */
typedef struct {
	struct rb_node		node;
	unsigned int		id;
	TfwStreamState		state;
	unsigned short		weight;
} TfwStream;

/**
 * Scheduler for stream's processing distribution based on dependency/priority
 * values.
 * TODO: the structure is not completed yet and should be finished in context
 * of #1196.
 *
 * @streams	- root red-black tree entry for per-connection streams' storage;
 */
typedef struct {
	struct rb_root streams;
} TfwStreamSched;


TfwStream *tfw_http2_find_stream(TfwStreamSched *sched, unsigned int id);
int tfw_http2_add_stream(TfwStreamSched *sched, TfwStream *stream);
void tfw_http2_remove_stream(TfwStreamSched *sched, TfwStream *stream);
void tfw_http2_streams_cleanup(TfwStreamSched *sched);
int tfw_http2_find_stream_dep(TfwStreamSched *sched, unsigned int id,
			      TfwStream **dep);
void tfw_http2_add_stream_dep(TfwStreamSched *sched, TfwStream *stream,
			      TfwStream *dep, bool excl);
void tfw_http2_change_stream_dep(TfwStreamSched *sched, unsigned int stream_id,
				 unsigned int new_dep, unsigned short new_weight,
				 bool excl);
void tfw_http2_remove_stream_dep(TfwStreamSched *sched, TfwStream *stream);

#endif /* __HTTP_STREAM__ */

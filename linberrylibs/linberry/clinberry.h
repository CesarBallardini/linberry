/**
 * \file	clinberry.h
 *		Main header file for Linberry C API - incomplete
 */

/*
    Copyright (C) 2007-2009, Net Direct Inc. (http://www.netdirect.ca/)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License in the COPYING file at the
    root directory of this project for more details.
*/

#ifndef __LINBERRY_CLINBERRY_H__
#define __LINBERRY_CLINBERRY_H__

/*
 * Supporting C headers
 */
#include "dll.h"
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Linberry C API
 *
 * All functions that can fail will either return a handle, which can
 * be compared to NULL, or return int, which will == -1 on error.
 *
 * Functions that return a "count" can also fail, but rarely (such as
 * if you pass in a bad handle).
 */

/* Handle types */
typedef void* probe_handle_t;
typedef void* con_handle_t;
typedef void* state_table_handle_t;
typedef void* record_handle_t;
typedef void (*process_record_callback_t)(int record_type,
	record_handle_t filled_record);
typedef void (*fill_record_callback_t)(int record_type,
	record_handle_t empty_record);

/* Record type codes */
#define LINBERRY_RECORD_CONTACT		1
#define LINBERRY_RECORD_MESSAGE		2
#define LINBERRY_RECORD_CALENDAR		3
#define LINBERRY_RECORD_SERVICEBOOK	4

/* Record field type codes */
#define LINBERRY_FIELDTYPE_NUMBER		1	/* uint32_t */
#define LINBERRY_FIELDTYPE_STRING		2	/* null terminated or raw str */
#define LINBERRY_FIELDTYPE_TIME		3	/* time_t */

#define IS_NUMBER(fieldcode)	(((fieldcode & 0xff0000) == 0x010000)
#define IS_STRING(fieldcode)	(((fieldcode & 0xff0000) == 0x020000)
#define IS_TIME(fieldcode)	(((fieldcode & 0xff0000) == 0x030000)

/* Contact record field codes */
#define LINBERRY_CONTACT_RECORDID		0x010101	/* uint32_t */
#define LINBERRY_CONTACT_EMAIL		0x020102	/* strings below... */
#define LINBERRY_CONTACT_PHONE		0x020103
#define LINBERRY_CONTACT_FAX		0x020104
#define LINBERRY_CONTACT_WORKPHONE		0x020105
#define LINBERRY_CONTACT_HOMEPHONE		0x020106
#define LINBERRY_CONTACT_MOBILEPHONE	0x020107
#define LINBERRY_CONTACT_PAGER		0x020108
#define LINBERRY_CONTACT_PIN		0x020109
#define LINBERRY_CONTACT_FIRSTNAME		0x02010a
#define LINBERRY_CONTACT_LASTNAME		0x02010b
#define LINBERRY_CONTACT_COMPANY		0x02010c
#define LINBERRY_CONTACT_DEFAULTCOMMMETHOD	0x02010d
#define LINBERRY_CONTACT_ADDRESS1		0x02010e
#define LINBERRY_CONTACT_ADDRESS2		0x02010f
#define LINBERRY_CONTACT_ADDRESS3		0x020110
#define LINBERRY_CONTACT_CITY		0x020111
#define LINBERRY_CONTACT_PROVINCE		0x020112
#define LINBERRY_CONTACT_POSTALCODE	0x020113
#define LINBERRY_CONTACT_COUNTRY		0x020114
#define LINBERRY_CONTACT_TITLE		0x020115
#define LINBERRY_CONTACT_PUBLICKEY		0x020116
#define LINBERRY_CONTACT_NOTES		0x020117

/* Message record field codes */
#define LINBERRY_MESSAGE_FROM_NAME		0x020201	/* all strings */
#define LINBERRY_MESSAGE_FROM_EMAIL	0x020202
#define LINBERRY_MESSAGE_TO_NAME		0x020203
#define LINBERRY_MESSAGE_TO_EMAIL		0x020204
#define LINBERRY_MESSAGE_CC_NAME		0x020205
#define LINBERRY_MESSAGE_CC_EMAIL		0x020206
#define LINBERRY_MESSAGE_SUBJECT		0x020207
#define LINBERRY_MESSAGE_BODY		0x020208	/* string or raw */

/* Calendar record field codes */
#define LINBERRY_CALENDAR_ALLDAYEVENT	0x010301	/* true/false number */
#define LINBERRY_CALENDAR_SUBJECT		0x020302	/* strings... */
#define LINBERRY_CALENDAR_NOTES		0x020303
#define LINBERRY_CALENDAR_LOCATION		0x020304
#define LINBERRY_CALENDAR_NOTIFICATIONTIME	0x030305	/* time_t... */
#define LINBERRY_CALENDAR_STARTTIME	0x030306
#define LINBERRY_CALENDAR_ENDTIME		0x030307
#define LINBERRY_CALENDAR_INTERVAL		0x010308	/* number */
#define LINBERRY_CALENDAR_RECURRINGENDTIME	0x030309	/* time_t */
#define LINBERRY_CALENDAR_PERPETUAL	0x01030a	/* true/false number */
#define LINBERRY_CALENDAR_TIMEZONE		0x01030b	/* numeric code */

/* Service Book record field codes */
#define LINBERRY_SERVICEBOOK_RECORDID	0x010401	/* number */
#define LINBERRY_SERVICEBOOK_NAME		0x020402	/* strings... */
#define LINBERRY_SERVICEBOOK_HIDDENNAME	0x020403
#define LINBERRY_SERVICEBOOK_DESCRIPTION	0x020404
#define LINBERRY_SERVICEBOOK_DSID		0x020405
#define LINBERRY_SERVICEBOOK_BESDOMAIN	0x020406
#define LINBERRY_SERVICEBOOK_UNIQUEID	0x020407
#define LINBERRY_SERVICEBOOK_CONTENTID	0x020408
#define LINBERRY_SERVICEBOOK_CONFIG_FORMAT	0x010409	/* number */

/* Initialization */
BXEXPORT void linberry_init(int data_dump_mode);
BXEXPORT const char *linberry_version(int *major, int *minor);

/* Error string retrieval */
BXEXPORT const char *linberry_get_last_error();

/* Probe API */
BXEXPORT probe_handle_t linberry_probe(void);
BXEXPORT void linberry_probe_close(probe_handle_t handle);
BXEXPORT int linberry_probe_count(probe_handle_t handle);
BXEXPORT int linberry_probe_find_active(probe_handle_t handle, uint32_t pin);
BXEXPORT void linberry_probe_result(probe_handle_t handle, int index,
	struct ProbeResult *result);

/* Controller API */
BXEXPORT con_handle_t linberry_con_open(struct ProbeResult *result);
BXEXPORT void linberry_con_close(con_handle_t handle);
BXEXPORT int linberry_con_mode(con_handle_t handle, int mode);
BXEXPORT int linberry_con_get_dbid(con_handle_t handle, const char *dbname);
BXEXPORT int linberry_con_get_db_count(con_handle_t handle);
BXEXPORT int linberry_con_get_db_info(con_handle_t handle, int index,
	unsigned int *number, unsigned int *record_count,
	char *name, int name_buf_size);
BXEXPORT int linberry_con_add_record(con_handle_t handle, unsigned int dbid,
	record_handle_t rec);
BXEXPORT int linberry_con_get_record(con_handle_t handle, unsigned int dbid,
	unsigned int state_table_index, record_handle_t *rec);
BXEXPORT int linberry_con_set_record(con_handle_t handle, unsigned int dbid,
	unsigned int state_table_index, record_handle_t rec);
BXEXPORT int linberry_con_clear_dirty(con_handle_t handle, unsigned int dbid,
	unsigned int state_table_index);
BXEXPORT int linberry_con_delete_record(con_handle_t handle, unsigned int dbid,
	unsigned int state_table_index);
BXEXPORT int linberry_con_load_database(con_handle_t handle, unsigned int dbid,
	process_record_callback_t callback);
BXEXPORT int linberry_con_save_database(con_handle_t handle, unsigned int dbid,
	fill_record_callback_t callback);

/* State table API */
BXEXPORT state_table_handle_t linberry_get_state_table(con_handle_t handle,
	unsigned int dbid);
BXEXPORT void linberry_free_state_table(state_table_handle_t handle);
BXEXPORT int linberry_make_new_record_id(state_table_handle_t handle);
BXEXPORT int linberry_get_state_count(state_handle_t handle);
BXEXPORT int linberry_get_state(state_table_handle_t handle, unsigned int index,
	uint32_t *record_id, int *dirty_flag);
	/* note: not every index from 0 to "state_count" is guaranteed to
	   exist... check the return value */
BXEXPORT int linberry_get_state_by_record_id(state_table_handle_t handle, unsigned int rec,
	unsigned int *index, int *dirty_flag);

/* Record API */
BXEXPORT record_handle_t linberry_create_record(int record_type);
BXEXPORT void linberry_free_record(record_handle_t handle);
BXEXPORT uint32_t linberry_rec_get_num(record_handle_t handle, int field_type);
BXEXPORT int linberry_rec_set_num(record_handle_t handle, int field_type, uint32_t val);
BXEXPORT const char *linberry_rec_get_str(record_handle_t handle, int field_type);
BXEXPORT int linberry_rec_set_str(record_handle_t handle, int field_type, const char *str);
BXEXPORT const char *linberry_rec_get_raw(record_handle_t handle, int field_type,
	int *raw_size);
BXEXPORT int linberry_rec_set_raw(record_handle_t handle, int field_type,
	const char *buf, int size);
BXEXPORT time_t linberry_rec_get_time(record_handle_t handle, int field_type);
BXEXPORT int linberry_rec_set_time(record_handle_t handle, int field_type, time_t t);

/* Calendar record special API */
BXEXPORT int linberry_calendar_set_daily(record_handle_t handle);
BXEXPORT int linberry_calendar_set_monthly_by_date(record_handle_t handle,
	int day_of_month);
BXEXPORT int linberry_calendar_set_monthly_by_day(record_handle_t handle,
	int day_of_week, int week_of_month);
BXEXPORT int linberry_calendar_set_yearly_by_date(record_handle_t handle,
	int day_of_month, int month_of_year);
BXEXPORT int linberry_calendar_set_yearly_by_day(record_handle_t handle,
	int day_of_week, int week_of_month, int month_of_year);
BXEXPORT int linberry_calendar_set_weekly(record_handle_t handle, int weekday_bits);
BXEXPORT int linberry_calendar_get_fixme_need_read_access_to_this

/* Time zone API */
BXEXPORT const TimeZone* linberry_get_time_zone_table();
BXEXPORT const TimeZone* linberry_get_time_zone(unsigned short code);
BXEXPORT unsigned short linberry_get_time_zone_code(signed short hour_offset,
	signed short min_offset);	/* returns TIME_ZONE_CODE_ERR on error*/

#ifdef __cplusplus
}
#endif

#endif


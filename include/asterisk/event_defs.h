/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2007 - 2008, Digium, Inc.
 *
 * Russell Bryant <russell@digium.com>
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

/*!
 * \file
 * \author Russell Bryant <russell@digium.com>
 * \brief Generic event system
 */

#ifndef AST_EVENT_DEFS_H
#define AST_EVENT_DEFS_H

/*! \brief Event types
 * \note These values no longer go over the wire and can change when items are removed. */
enum ast_event_type {
	/*! Reserved to provide the ability to subscribe to all events.  A specific
	 *  event should never have a payload of 0. */
	AST_EVENT_ALL                 = 0x00,
	/*! This event type is reserved for use by third-party modules to create
	 *  custom events without having to modify this file. 
	 *  \note There are no "custom" IE types, because IEs only have to be
	 *  unique to the event itself, not necessarily across all events. */
	AST_EVENT_CUSTOM              = 0x01,
	/*! Someone has subscribed to events */
	AST_EVENT_SUB                 = 0x02,
	/*! Channel Event Logging events */
	AST_EVENT_CEL                 = 0x03,
	/*! Number of event types.  This should be the last event type + 1 */
	AST_EVENT_TOTAL               = 0x04,
};

/*! \brief Event Information Element types */
enum ast_event_ie_type {
	/*! Used to terminate the arguments to event functions */
	AST_EVENT_IE_END                 = -1,

	/*! 
	 * \brief Unique ID
	 * Used by: AST_EVENT_SUB, AST_EVENT_UNSUB
	 * Payload type: UINT
	 */
	AST_EVENT_IE_UNIQUEID            = 0x0001,
	/*! 
	 * \brief Event type 
	 * Used by: AST_EVENT_SUB, AST_EVENT_UNSUB
	 * Payload type: UINT
	 */
	AST_EVENT_IE_EVENTTYPE           = 0x0002,
	/*!
	 * \brief Hint that someone cares that an IE exists
	 * Used by: AST_EVENT_SUB
	 * Payload type: UINT (ast_event_ie_type)
	 */
	AST_EVENT_IE_EXISTS              = 0x0003,
	 /*!
	  * \brief Context IE
	  * Used by AST_EVENT_MWI
	  * Payload type: str
	  */
	 AST_EVENT_IE_CONTEXT            = 0x0004,
	/*! 
	 * \brief Channel Event Type
	 * Used by: AST_EVENT_CEL
	 * Payload type: UINT
	 */
	AST_EVENT_IE_CEL_EVENT_TYPE      = 0x0005,
	/*! 
	 * \brief Channel Event Time (seconds)
	 * Used by: AST_EVENT_CEL
	 * Payload type: UINT
	 */
	AST_EVENT_IE_CEL_EVENT_TIME      = 0x0006,
	/*! 
	 * \brief Channel Event Time (micro-seconds)
	 * Used by: AST_EVENT_CEL
	 * Payload type: UINT
	 */
	AST_EVENT_IE_CEL_EVENT_TIME_USEC = 0x0007,
	/*! 
	 * \brief Channel Event User Event Name
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_USEREVENT_NAME  = 0x0008,
	/*! 
	 * \brief Channel Event CID name
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_CIDNAME         = 0x0009,
	/*! 
	 * \brief Channel Event CID num
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_CIDNUM          = 0x000a,
	/*! 
	 * \brief Channel Event extension name
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_EXTEN           = 0x000b,
	/*! 
	 * \brief Channel Event context name
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_CONTEXT         = 0x000c,
	/*! 
	 * \brief Channel Event channel name
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_CHANNAME        = 0x000d,
	/*! 
	 * \brief Channel Event app name
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_APPNAME         = 0x000e,
	/*! 
	 * \brief Channel Event app args/data
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_APPDATA         = 0x000f,
	/*! 
	 * \brief Channel Event AMA flags
	 * Used by: AST_EVENT_CEL
	 * Payload type: UINT
	 */
	AST_EVENT_IE_CEL_AMAFLAGS        = 0x0010,
	/*! 
	 * \brief Channel Event AccountCode
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_ACCTCODE        = 0x0011,
	/*! 
	 * \brief Channel Event UniqueID
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_UNIQUEID        = 0x0012,
	/*! 
	 * \brief Channel Event Userfield
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_USERFIELD       = 0x0013,
	/*! 
	 * \brief Channel Event CID ANI field
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_CIDANI          = 0x0014,
	/*! 
	 * \brief Channel Event CID RDNIS field
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_CIDRDNIS        = 0x0015,
	/*! 
	 * \brief Channel Event CID dnid
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_CIDDNID         = 0x0016,
	/*! 
	 * \brief Channel Event Peer -- for Things involving multiple channels, like BRIDGE
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_PEER            = 0x0017,
	/*! 
	 * \brief Channel Event LinkedID
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_LINKEDID        = 0x0018,
	/*! 
	 * \brief Channel Event peeraccount
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_PEERACCT        = 0x0019,
	/*! 
	 * \brief Channel Event extra data
	 * Used by: AST_EVENT_CEL
	 * Payload type: STR
	 */
	AST_EVENT_IE_CEL_EXTRA           = 0x001a,
	/*!
	 * \brief Description
	 * Used by: AST_EVENT_SUB, AST_EVENT_UNSUB
	 * Payload type: STR
	 */
	AST_EVENT_IE_DESCRIPTION         = 0x001b,
	/*!
	 * \brief Entity ID
	 * Used by All events
	 * Payload type: RAW
	 * This IE indicates which server the event originated from
	 */
	AST_EVENT_IE_EVENT_VERSION       = 0x001c,
	AST_EVENT_IE_SERVICE             = 0x001d,
	AST_EVENT_IE_MODULE              = 0x001e,
	AST_EVENT_IE_ACCOUNT_ID          = 0x001f,
	AST_EVENT_IE_SESSION_ID          = 0x0020,
	AST_EVENT_IE_SESSION_TV          = 0x0021,
	AST_EVENT_IE_ACL_NAME            = 0x0022,
	AST_EVENT_IE_LOCAL_ADDR          = 0x0023,
	AST_EVENT_IE_REMOTE_ADDR         = 0x0024,
	AST_EVENT_IE_EVENT_TV            = 0x0025,
	AST_EVENT_IE_REQUEST_TYPE        = 0x0026,
	AST_EVENT_IE_REQUEST_PARAMS      = 0x0027,
	AST_EVENT_IE_AUTH_METHOD         = 0x0028,
	AST_EVENT_IE_SEVERITY            = 0x0029,
	AST_EVENT_IE_EXPECTED_ADDR       = 0x002a,
	AST_EVENT_IE_CHALLENGE           = 0x002b,
	AST_EVENT_IE_RESPONSE            = 0x002c,
	AST_EVENT_IE_EXPECTED_RESPONSE   = 0x002e,
	AST_EVENT_IE_RECEIVED_CHALLENGE  = 0x002f,
	AST_EVENT_IE_RECEIVED_HASH       = 0x0030,
	AST_EVENT_IE_USING_PASSWORD      = 0x0031,
	AST_EVENT_IE_ATTEMPTED_TRANSPORT = 0x0032,

	/*! \brief Must be the last IE value +1 */
	AST_EVENT_IE_TOTAL               = 0x0033,
};

/*!
 * \brief Payload types for event information elements
 */
enum ast_event_ie_pltype {
	AST_EVENT_IE_PLTYPE_UNKNOWN = -1,
	/*! Unsigned Integer (Can be used for signed, too ...) */
	AST_EVENT_IE_PLTYPE_UINT,
	/*! String */
	AST_EVENT_IE_PLTYPE_STR,
	/*! Raw data, compared with memcmp */
	AST_EVENT_IE_PLTYPE_RAW,
};

/*!
 * \brief Results for checking for subscribers
 *
 * \ref ast_event_check_subscriber()
 */
enum ast_event_subscriber_res {
	/*! No subscribers exist */
	AST_EVENT_SUB_NONE,
	/*! At least one subscriber exists */
	AST_EVENT_SUB_EXISTS,
};

struct ast_event;
struct ast_event_ie;
struct ast_event_iterator;

/*!
 * \brief supposed to be an opaque type
 *
 * This is only here so that it can be declared on the stack.
 */
struct ast_event_iterator {
	uint16_t event_len;
	const struct ast_event *event;
	struct ast_event_ie *ie;
};

#endif /* AST_EVENT_DEFS_H */

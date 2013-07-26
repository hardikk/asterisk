/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2013, Digium, Inc.
 *
 * Mark Spencer <markster@digium.com>
 *
 * Author: Jonathan Rose <jrose@digium.com>
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

/*! \file
 *
 * \brief Application to place the channel into a holding Bridge
 *
 * \author Jonathan Rose <jrose@digium.com>
 *
 * \ingroup applications
 */

/*** MODULEINFO
	<depend>bridge_holding</depend>
	<support_level>core</support_level>
 ***/

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/file.h"
#include "asterisk/channel.h"
#include "asterisk/pbx.h"
#include "asterisk/module.h"
#include "asterisk/features.h"
#include "asterisk/say.h"
#include "asterisk/lock.h"
#include "asterisk/utils.h"
#include "asterisk/app.h"
#include "asterisk/bridge.h"
#include "asterisk/musiconhold.h"
#include "asterisk/astobj2.h"

/*** DOCUMENTATION
	<application name="BridgeWait" language="en_US">
		<synopsis>
			Put a call into the holding bridge.
		</synopsis>
		<syntax>
			<parameter name="name">
				<para>Name of the holding bridge to join. This is a handle for <literal>BridgeWait</literal>
				only and does not affect the actual bridges that are created. If not provided,
				the reserved name <literal>default</literal> will be used.
				</para>
			</parameter>
			<parameter name="role" required="false">
				<para>Defines the channel's purpose for entering the holding bridge. Values are case sensitive.
				</para>
				<enumlist>
					<enum name="participant">
						<para>The channel will enter the holding bridge to be placed on hold
						until it is removed from the bridge for some reason. (default)</para>
					</enum>
					<enum name="announcer">
						<para>The channel will enter the holding bridge to make announcements
						to channels that are currently in the holding bridge. While an
						announcer is present, holding for the participants will be
						suspended.</para>
					</enum>
				</enumlist>
			</parameter>
			<parameter name="options">
				<optionlist>
					<option name="m">
						<argument name="class" required="true" />
						<para>The specified MOH class will be used/suggested for
						music on hold operations. This option will only be useful for
						entertainment modes that use it (m and h).</para>
					</option>
					<option name="e">
						<para>Which entertainment mechanism should be used while on hold
						in the holding bridge. Only the first letter is read.</para>
						<enumlist>
							<enum name="m"><para>Play music on hold (default)</para></enum>
							<enum name="r"><para>Ring without pause</para></enum>
							<enum name="s"><para>Generate silent audio</para></enum>
							<enum name="h"><para>Put the channel on hold</para></enum>
							<enum name="n"><para>No entertainment</para></enum>
						</enumlist>
					</option>
					<option name="S">
						<argument name="duration" required="true" />
						<para>Automatically exit the bridge and return to the PBX after
						<emphasis>duration</emphasis> seconds.</para>
					</option>
				</optionlist>
			</parameter>
		</syntax>
		<description>
			<para>This application places the incoming channel into a holding bridge.
			The channel will then wait in the holding bridge until some event occurs
			which removes it from the holding bridge.</para>
			<note><para>This application will answer calls which haven't already
			been answered.</para></note>
		</description>
	</application>
 ***/

#define APP_NAME "BridgeWait"
#define DEFAULT_BRIDGE_NAME "default"

static struct ao2_container *wait_bridge_wrappers;

struct wait_bridge_wrapper {
	struct ast_bridge *bridge;     /*!< Bridge being wrapped by this wrapper */
	char name[0];                  /*!< Name of the holding bridge wrapper */
};

static void wait_bridge_wrapper_destructor(void *obj)
{
	struct wait_bridge_wrapper *wrapper = obj;
	if (wrapper->bridge) {
		ast_bridge_destroy(wrapper->bridge);
	}
}

static struct wait_bridge_wrapper *wait_bridge_wrapper_find_by_name(const char *bridge_name)
{
	return ao2_find(wait_bridge_wrappers, bridge_name, OBJ_KEY);
}

static int wait_bridge_hash_fn(const void *obj, const int flags)
{
	const struct wait_bridge_wrapper *entry;
	const char *key;

	switch (flags & (OBJ_POINTER | OBJ_KEY | OBJ_PARTIAL_KEY)) {
	case OBJ_KEY:
		key = obj;
		return ast_str_hash(key);
	case OBJ_POINTER:
		entry = obj;
		return ast_str_hash(entry->name);
	default:
		/* Hash can only work on something with a full key. */
		ast_assert(0);
		return 0;
	}
}

static int wait_bridge_sort_fn(const void *obj_left, const void *obj_right, const int flags)
{
	const struct wait_bridge_wrapper *left = obj_left;
	const struct wait_bridge_wrapper *right = obj_right;
	const char *right_key = obj_right;
	int cmp;

	switch (flags & (OBJ_POINTER | OBJ_KEY | OBJ_PARTIAL_KEY)) {
	case OBJ_POINTER:
		right_key = right->name;
		/* Fall through */
	case OBJ_KEY:
		cmp = strcmp(left->name, right_key);
		break;
	case OBJ_PARTIAL_KEY:
		cmp = strncmp(left->name, right_key, strlen(right_key));
		break;
	default:
		/* Sort can only work on something with a full or partial key. */
		ast_assert(0);
		cmp = 0;
		break;
	}
	return cmp;
}

enum bridgewait_flags {
	MUXFLAG_MOHCLASS = (1 << 0),
	MUXFLAG_ENTERTAINMENT = (1 << 1),
	MUXFLAG_TIMEOUT = (1 << 2),
};

enum bridgewait_args {
	OPT_ARG_ENTERTAINMENT,
	OPT_ARG_MOHCLASS,
	OPT_ARG_TIMEOUT,
	OPT_ARG_ARRAY_SIZE, /* Always the last element of the enum */
};

AST_APP_OPTIONS(bridgewait_opts, {
	AST_APP_OPTION_ARG('e', MUXFLAG_ENTERTAINMENT, OPT_ARG_ENTERTAINMENT),
	AST_APP_OPTION_ARG('m', MUXFLAG_MOHCLASS, OPT_ARG_MOHCLASS),
	AST_APP_OPTION_ARG('S', MUXFLAG_TIMEOUT, OPT_ARG_TIMEOUT),
});

static int apply_option_timeout(struct ast_bridge_features *features, char *duration_arg)
{
	struct ast_bridge_features_limits hold_limits;

	if (ast_strlen_zero(duration_arg)) {
		ast_log(LOG_ERROR, "No duration value provided for the timeout ('S') option.\n");
		return -1;
	}

	if (ast_bridge_features_limits_construct(&hold_limits)) {
		ast_log(LOG_ERROR, "Could not construct duration limits. Bridge canceled.\n");
		return -1;
	}

	if (sscanf(duration_arg, "%u", &hold_limits.duration) != 1
		|| hold_limits.duration == 0) {
		ast_log(LOG_ERROR, "Duration value provided for the timeout ('S') option must be greater than 0\n");
		ast_bridge_features_limits_destroy(&hold_limits);
		return -1;
	}

	/* Limits struct holds time as milliseconds, so muliply 1000x */
	hold_limits.duration *= 1000;
	ast_bridge_features_set_limits(features, &hold_limits, AST_BRIDGE_HOOK_REMOVE_ON_PULL);
	ast_bridge_features_limits_destroy(&hold_limits);

	return 0;
}

static int apply_option_moh(struct ast_channel *chan, const char *class_arg)
{
	return ast_channel_set_bridge_role_option(chan, "holding_participant", "moh_class", class_arg);
}

static int apply_option_entertainment(struct ast_channel *chan, const char *entertainment_arg)
{
	char entertainment = entertainment_arg[0];

	switch (entertainment) {
	case 'm':
		return ast_channel_set_bridge_role_option(chan, "holding_participant", "idle_mode", "musiconhold");
	case 'r':
		return ast_channel_set_bridge_role_option(chan, "holding_participant", "idle_mode", "ringing");
	case 's':
		return ast_channel_set_bridge_role_option(chan, "holding_participant", "idle_mode", "silence");
	case 'h':
		return ast_channel_set_bridge_role_option(chan, "holding_participant", "idle_mode", "hold");
	case 'n':
		return ast_channel_set_bridge_role_option(chan, "holding_participant", "idle_mode", "none");
	default:
		ast_log(LOG_ERROR, "Invalid argument for BridgeWait entertainment '%s'\n", entertainment_arg);
		return -1;
	}
}

enum wait_bridge_roles {
	ROLE_PARTICIPANT = 0,
	ROLE_ANNOUNCER,
	ROLE_INVALID,
};

static int process_options(struct ast_channel *chan, struct ast_flags *flags, char **opts, struct ast_bridge_features *features, enum wait_bridge_roles role)
{
	if (ast_test_flag(flags, MUXFLAG_TIMEOUT)) {
		if (apply_option_timeout(features, opts[OPT_ARG_TIMEOUT])) {
			return -1;
		}
	}

	switch (role) {
	case ROLE_PARTICIPANT:
		if (ast_channel_add_bridge_role(chan, "holding_participant")) {
			return -1;
		}

		if (ast_test_flag(flags, MUXFLAG_MOHCLASS)) {
			if (apply_option_moh(chan, opts[OPT_ARG_MOHCLASS])) {
				return -1;
			}
		}

		if (ast_test_flag(flags, MUXFLAG_ENTERTAINMENT)) {
			if (apply_option_entertainment(chan, opts[OPT_ARG_ENTERTAINMENT])) {
				return -1;
			}
		}

		break;
	case ROLE_ANNOUNCER:
		if (ast_channel_add_bridge_role(chan, "announcer")) {
			return -1;
		}
		break;
	case ROLE_INVALID:
		ast_assert(0);
		return -1;
	}

	return 0;
}

/*!
 * \internal
 * \since 12.0.0
 * \brief Allocate a new holding bridge wrapper with the given bridge name and bridge ID.
 *
 * \param bridge_name name of the bridge wrapper
 * \param bridge the bridge being wrapped
 *
 * \retval Pointer to the newly allocated holding bridge wrapper
 * \retval NULL if allocation failed. The bridge will be destroyed if this function fails.
 */
static struct wait_bridge_wrapper *wait_bridge_wrapper_alloc(const char *bridge_name, struct ast_bridge *bridge)
{
	struct wait_bridge_wrapper *bridge_wrapper;

	bridge_wrapper = ao2_alloc_options(sizeof(*bridge_wrapper) + strlen(bridge_name) + 1,
		wait_bridge_wrapper_destructor, AO2_ALLOC_OPT_LOCK_NOLOCK);

	if (!bridge_wrapper) {
		ast_bridge_destroy(bridge);
		return NULL;
	}

	strcpy(bridge_wrapper->name, bridge_name);
	bridge_wrapper->bridge = bridge;

	if (!ao2_link(wait_bridge_wrappers, bridge_wrapper)) {
		ao2_cleanup(bridge_wrapper);
		return NULL;
	}

	return bridge_wrapper;
}

static struct wait_bridge_wrapper *get_wait_bridge_wrapper(const char *bridge_name)
{
	struct wait_bridge_wrapper * wrapper;
	struct ast_bridge *bridge = NULL;

	SCOPED_AO2LOCK(lock, wait_bridge_wrappers);

	if ((wrapper = wait_bridge_wrapper_find_by_name(bridge_name))) {
		return wrapper;
	}

	bridge = ast_bridge_base_new(AST_BRIDGE_CAPABILITY_HOLDING,
		AST_BRIDGE_FLAG_MERGE_INHIBIT_TO | AST_BRIDGE_FLAG_MERGE_INHIBIT_FROM
		| AST_BRIDGE_FLAG_SWAP_INHIBIT_FROM | AST_BRIDGE_FLAG_TRANSFER_PROHIBITED
		| AST_BRIDGE_FLAG_DISSOLVE_EMPTY);

	if (!bridge) {
		return NULL;
	}

	/* The bridge reference is unconditionally passed. */
	return wait_bridge_wrapper_alloc(bridge_name, bridge);
}

/*!
 * \internal
 * \since 12.0.0
 * \brief If we are down to the last reference of a wrapper and it's still contained within the list, remove it from the list.
 *
 * \param wrapper reference to wait bridge wrapper being checked for list culling - will be cleared on exit
 */
static void wait_wrapper_removal(struct wait_bridge_wrapper *wrapper)
{
	if (!wrapper) {
		return;
	}

	ao2_lock(wait_bridge_wrappers);
	if (ao2_ref(wrapper, 0) == 2) {
		/* Either we have the only real reference or else wrapper isn't in the container anyway. */
		ao2_unlink(wait_bridge_wrappers, wrapper);
	}
	ao2_unlock(wait_bridge_wrappers);

	ao2_cleanup(wrapper);
}

/*!
 * \internal
 * \since 12.0.0
 * \brief Application callback for the bridgewait application
 *
 * \param chan channel running the application
 * \param data Arguments to the application
 *
 * \retval 0 Ran successfully and the call didn't hang up
 * \retval -1 Failed or the call was hung up by the time the channel exited the holding bridge
 */
static enum wait_bridge_roles validate_role(const char *role)
{
	if (!strcmp(role, "participant")) {
		return ROLE_PARTICIPANT;
	} else if (!strcmp(role, "announcer")) {
		return ROLE_ANNOUNCER;
	} else {
		return ROLE_INVALID;
	}
}

static int bridgewait_exec(struct ast_channel *chan, const char *data)
{
	char *bridge_name = DEFAULT_BRIDGE_NAME;
	struct ast_bridge_features chan_features;
	struct ast_flags flags = { 0 };
	char *parse;
	int bridge_join_failed = 0;
	enum wait_bridge_roles role = ROLE_PARTICIPANT;
	char *opts[OPT_ARG_ARRAY_SIZE] = { NULL, };

	AST_DECLARE_APP_ARGS(args,
		AST_APP_ARG(name);
		AST_APP_ARG(role);
		AST_APP_ARG(options);
		AST_APP_ARG(other);		/* Any remaining unused arguments */
	);

	parse = ast_strdupa(data);
	AST_STANDARD_APP_ARGS(args, parse);

	if (!ast_strlen_zero(args.name)) {
		bridge_name = args.name;
	}

	if (!ast_strlen_zero(args.role)) {
		role = validate_role(args.role);
		if (role == ROLE_INVALID) {
			ast_log(LOG_ERROR, "Requested waiting bridge role '%s' is invalid.\n", args.role);
			return -1;
		}
	}

	if (ast_bridge_features_init(&chan_features)) {
		ast_bridge_features_cleanup(&chan_features);
		ast_log(LOG_ERROR, "'%s' failed to enter the waiting bridge - could not set up channel features\n",
			ast_channel_name(chan));
		return -1;
	}

	if (args.options) {
		ast_app_parse_options(bridgewait_opts, &flags, opts, args.options);
	}

	/* Answer the channel if needed */
	if (ast_channel_state(chan) != AST_STATE_UP) {
		ast_answer(chan);
	}

	if (process_options(chan, &flags, opts, &chan_features, role)) {
		ast_bridge_features_cleanup(&chan_features);
		return -1;
	}

	for (;;) {
		RAII_VAR(struct wait_bridge_wrapper *, bridge_wrapper, get_wait_bridge_wrapper(bridge_name), wait_wrapper_removal);

		if (!bridge_wrapper) {
			ast_log(LOG_WARNING, "Failed to find or create waiting bridge '%s' for '%s'.\n", bridge_name, ast_channel_name(chan));
			bridge_join_failed = 1;
			break;
		}

		ast_verb(3, "%s is entering waiting bridge %s:%s\n", ast_channel_name(chan), bridge_name, bridge_wrapper->bridge->uniqueid);

		if (ast_bridge_join(bridge_wrapper->bridge, chan, NULL, &chan_features, NULL, 0)) {
			/* It's possible for a holding bridge to vanish out from under us since we can't lock it.
			 * Unlink the wrapper and then loop if the bridge we try to enter is dissolved. */
			ast_verb(3, "Waiting bridge '%s:%s' is no longer joinable. Creating new bridge and trying again.\n",
				bridge_name, bridge_wrapper->bridge->uniqueid);
			ao2_unlink(wait_bridge_wrappers, bridge_wrapper);
			continue;
		}

		break;
	}

	ast_bridge_features_cleanup(&chan_features);

	if (bridge_join_failed) {
		return -1;
	}

	return ast_check_hangup_locked(chan) ? -1 : 0;
}

static int unload_module(void)
{
	ao2_cleanup(wait_bridge_wrappers);

	return ast_unregister_application(APP_NAME);
}

static int load_module(void)
{
	wait_bridge_wrappers = ao2_container_alloc_hash(
		AO2_ALLOC_OPT_LOCK_MUTEX, AO2_CONTAINER_ALLOC_OPT_DUPS_REJECT,
		37, wait_bridge_hash_fn, wait_bridge_sort_fn, NULL);

	if (!wait_bridge_wrappers) {
		return -1;
	}

	return ast_register_application_xml(APP_NAME, bridgewait_exec);
}

AST_MODULE_INFO_STANDARD(ASTERISK_GPL_KEY, "Place the channel into a holding bridge application");

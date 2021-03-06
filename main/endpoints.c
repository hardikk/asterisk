/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2013, Digium, Inc.
 *
 * David M. Lee, II <dlee@digium.com>
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
 * \brief Asterisk endpoint API.
 *
 * \author David M. Lee, II <dlee@digium.com>
 */

/*** MODULEINFO
	<support_level>core</support_level>
 ***/

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/astobj2.h"
#include "asterisk/endpoints.h"
#include "asterisk/stasis.h"
#include "asterisk/stasis_channels.h"
#include "asterisk/stasis_endpoints.h"
#include "asterisk/stasis_message_router.h"
#include "asterisk/stringfields.h"

/*! Buckets for endpoint->channel mappings. Keep it prime! */
#define ENDPOINT_BUCKETS 127

struct ast_endpoint {
	AST_DECLARE_STRING_FIELDS(
		AST_STRING_FIELD(tech);	/*!< Technology (SIP, IAX2, etc.). */
		AST_STRING_FIELD(resource);	/*!< Name, unique to the tech. */
		AST_STRING_FIELD(id);	/*!< tech/resource id */
		);
	/*! Endpoint's current state */
	enum ast_endpoint_state state;
	/*!
	 * \brief Max channels for this endpoint. -1 means unlimited or unknown.
	 *
	 * Note that this simply documents the limits of an endpoint, and does
	 * nothing to try to enforce the limit.
	 */
	int max_channels;
	/*! Topic for this endpoint's messages */
	struct stasis_cp_single *topics;
	/*! Router for handling this endpoint's messages */
	struct stasis_message_router *router;
	/*! ast_str_container of channels associated with this endpoint */
	struct ao2_container *channel_ids;
};

struct stasis_topic *ast_endpoint_topic(struct ast_endpoint *endpoint)
{
	if (!endpoint) {
		return ast_endpoint_topic_all();
	}
	return stasis_cp_single_topic(endpoint->topics);
}

struct stasis_topic *ast_endpoint_topic_cached(struct ast_endpoint *endpoint)
{
	if (!endpoint) {
		return ast_endpoint_topic_all_cached();
	}
	return stasis_cp_single_topic_cached(endpoint->topics);
}

const char *ast_endpoint_state_to_string(enum ast_endpoint_state state)
{
	switch (state) {
	case AST_ENDPOINT_UNKNOWN:
		return "unknown";
	case AST_ENDPOINT_OFFLINE:
		return "offline";
	case AST_ENDPOINT_ONLINE:
		return "online";
	}
	return "?";
}

static void endpoint_publish_snapshot(struct ast_endpoint *endpoint)
{
	RAII_VAR(struct ast_endpoint_snapshot *, snapshot, NULL, ao2_cleanup);
	RAII_VAR(struct stasis_message *, message, NULL, ao2_cleanup);

	ast_assert(endpoint != NULL);
	ast_assert(endpoint->topics != NULL);

	snapshot = ast_endpoint_snapshot_create(endpoint);
	if (!snapshot) {
		return;
	}
	message = stasis_message_create(ast_endpoint_snapshot_type(), snapshot);
	if (!message) {
		return;
	}
	stasis_publish(ast_endpoint_topic(endpoint), message);
}

static void endpoint_dtor(void *obj)
{
	struct ast_endpoint *endpoint = obj;

	/* The router should be shut down already */
	ast_assert(stasis_message_router_is_done(endpoint->router));
	ao2_cleanup(endpoint->router);
	endpoint->router = NULL;

	stasis_cp_single_unsubscribe(endpoint->topics);
	endpoint->topics = NULL;

	ao2_cleanup(endpoint->channel_ids);
	endpoint->channel_ids = NULL;

	ast_string_field_free_memory(endpoint);
}


int ast_endpoint_add_channel(struct ast_endpoint *endpoint,
	struct ast_channel *chan)
{
	ast_assert(chan != NULL);
	ast_assert(endpoint != NULL);

	ast_channel_forward_endpoint(chan, endpoint);

	ao2_lock(endpoint);
	ast_str_container_add(endpoint->channel_ids, ast_channel_uniqueid(chan));
	ao2_unlock(endpoint);

	ast_publish_channel_state(chan);
	endpoint_publish_snapshot(endpoint);

	return 0;
}

/*! \brief Handler for channel snapshot cache clears */
static void endpoint_cache_clear(void *data,
	struct stasis_subscription *sub,
	struct stasis_message *message)
{
	struct ast_endpoint *endpoint = data;
	struct stasis_message *clear_msg = stasis_message_data(message);
	struct ast_channel_snapshot *clear_snapshot;

	if (stasis_message_type(clear_msg) != ast_channel_snapshot_type()) {
		return;
	}

	clear_snapshot = stasis_message_data(clear_msg);

	ast_assert(endpoint != NULL);

	ao2_lock(endpoint);
	ast_str_container_remove(endpoint->channel_ids, clear_snapshot->uniqueid);
	ao2_unlock(endpoint);
	endpoint_publish_snapshot(endpoint);
}

static void endpoint_default(void *data,
	struct stasis_subscription *sub,
	struct stasis_message *message)
{
	struct stasis_endpoint *endpoint = data;

	if (stasis_subscription_final_message(sub, message)) {
		ao2_cleanup(endpoint);
	}
}

struct ast_endpoint *ast_endpoint_create(const char *tech, const char *resource)
{
	RAII_VAR(struct ast_endpoint *, endpoint, NULL, ao2_cleanup);
	int r = 0;

	if (ast_strlen_zero(tech)) {
		ast_log(LOG_ERROR, "Endpoint tech cannot be empty\n");
		return NULL;
	}

	if (ast_strlen_zero(resource)) {
		ast_log(LOG_ERROR, "Endpoint resource cannot be empty\n");
		return NULL;
	}

	endpoint = ao2_alloc(sizeof(*endpoint), endpoint_dtor);
	if (!endpoint) {
		return NULL;
	}

	endpoint->max_channels = -1;
	endpoint->state = AST_ENDPOINT_UNKNOWN;

	if (ast_string_field_init(endpoint, 80) != 0) {
		return NULL;
	}

	ast_string_field_set(endpoint, tech, tech);
	ast_string_field_set(endpoint, resource, resource);
	ast_string_field_build(endpoint, id, "%s/%s", tech, resource);

	/* All access to channel_ids should be covered by the endpoint's
	 * lock; no extra lock needed. */
	endpoint->channel_ids = ast_str_container_alloc_options(
		AO2_ALLOC_OPT_LOCK_NOLOCK, ENDPOINT_BUCKETS);
	if (!endpoint->channel_ids) {
		return NULL;
	}

	endpoint->topics = stasis_cp_single_create(ast_endpoint_cache_all(),
		endpoint->id);
	if (!endpoint->topics) {
		return NULL;
	}

	endpoint->router = stasis_message_router_create(ast_endpoint_topic(endpoint));
	if (!endpoint->router) {
		return NULL;
	}
	r |= stasis_message_router_add(endpoint->router,
		stasis_cache_clear_type(), endpoint_cache_clear,
		endpoint);
	r |= stasis_message_router_set_default(endpoint->router,
		endpoint_default, endpoint);

	endpoint_publish_snapshot(endpoint);

	ao2_ref(endpoint, +1);
	return endpoint;
}

const char *ast_endpoint_get_tech(const struct ast_endpoint *endpoint)
{
	ast_assert(endpoint != NULL);
	return endpoint->tech;
}

static struct stasis_message *create_endpoint_snapshot_message(struct ast_endpoint *endpoint)
{
	RAII_VAR(struct ast_endpoint_snapshot *, snapshot, NULL, ao2_cleanup);
	snapshot = ast_endpoint_snapshot_create(endpoint);
	if (!snapshot) {
		return NULL;
	}

	return stasis_message_create(ast_endpoint_snapshot_type(), snapshot);
}

void ast_endpoint_shutdown(struct ast_endpoint *endpoint)
{
	RAII_VAR(struct stasis_message *, clear_msg, NULL, ao2_cleanup);

	if (endpoint == NULL) {
		return;
	}

	clear_msg = create_endpoint_snapshot_message(endpoint);
	if (clear_msg) {
		RAII_VAR(struct stasis_message *, message, NULL, ao2_cleanup);
		message = stasis_cache_clear_create(clear_msg);
		if (message) {
			stasis_publish(ast_endpoint_topic(endpoint), message);
		}
	}

	/* Bump refcount to hold on to the router */
	ao2_ref(endpoint->router, +1);
	stasis_message_router_unsubscribe(endpoint->router);
}

const char *ast_endpoint_get_resource(const struct ast_endpoint *endpoint)
{
	return endpoint->resource;
}

void ast_endpoint_set_state(struct ast_endpoint *endpoint,
	enum ast_endpoint_state state)
{
	ast_assert(endpoint != NULL);
	ao2_lock(endpoint);
	endpoint->state = state;
	ao2_unlock(endpoint);
	endpoint_publish_snapshot(endpoint);
}

void ast_endpoint_set_max_channels(struct ast_endpoint *endpoint,
	int max_channels)
{
	ast_assert(endpoint != NULL);
	ao2_lock(endpoint);
	endpoint->max_channels = max_channels;
	ao2_unlock(endpoint);
	endpoint_publish_snapshot(endpoint);
}

static void endpoint_snapshot_dtor(void *obj)
{
	struct ast_endpoint_snapshot *snapshot = obj;

	ast_assert(snapshot != NULL);
	ast_string_field_free_memory(snapshot);
}

struct ast_endpoint_snapshot *ast_endpoint_snapshot_create(
	struct ast_endpoint *endpoint)
{
	RAII_VAR(struct ast_endpoint_snapshot *, snapshot, NULL, ao2_cleanup);
	int channel_count;
	struct ao2_iterator i;
	void *obj;
	SCOPED_AO2LOCK(lock, endpoint);

	channel_count = ao2_container_count(endpoint->channel_ids);

	snapshot = ao2_alloc(
		sizeof(*snapshot) + channel_count * sizeof(char *),
		endpoint_snapshot_dtor);

	if (ast_string_field_init(snapshot, 80) != 0) {
		return NULL;
	}

	ast_string_field_build(snapshot, id, "%s/%s", endpoint->tech,
		endpoint->resource);
	ast_string_field_set(snapshot, tech, endpoint->tech);
	ast_string_field_set(snapshot, resource, endpoint->resource);

	snapshot->state = endpoint->state;
	snapshot->max_channels = endpoint->max_channels;

	i = ao2_iterator_init(endpoint->channel_ids, 0);
	while ((obj = ao2_iterator_next(&i))) {
		RAII_VAR(char *, channel_id, obj, ao2_cleanup);
		snapshot->channel_ids[snapshot->num_channels++] = channel_id;
	}
	ao2_iterator_destroy(&i);

	ao2_ref(snapshot, +1);
	return snapshot;
}

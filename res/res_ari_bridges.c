/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2012 - 2013, Digium, Inc.
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

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!!!!                               DO NOT EDIT                        !!!!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * This file is generated by a mustache template. Please see the original
 * template in rest-api-templates/res_ari_resource.c.mustache
 */

/*! \file
 *
 * \brief Bridge resources
 *
 * \author David M. Lee, II <dlee@digium.com>
 */

/*** MODULEINFO
	<depend type="module">res_ari</depend>
	<depend type="module">res_stasis</depend>
	<support_level>core</support_level>
 ***/

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/app.h"
#include "asterisk/module.h"
#include "asterisk/stasis_app.h"
#include "ari/resource_bridges.h"
#if defined(AST_DEVMODE)
#include "ari/ari_model_validators.h"
#endif

#define MAX_VALS 128

/*!
 * \brief Parameter parsing callback for /bridges.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_get_bridges_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_get_bridges_args args = {};
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	ast_ari_get_bridges(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_list(response->message,
				ast_ari_validate_bridge_fn());
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /bridges\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /bridges\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	return;
}
/*!
 * \brief Parameter parsing callback for /bridges.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_new_bridge_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_new_bridge_args args = {};
	struct ast_variable *i;
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "type") == 0) {
			args.type = (i->value);
		} else
		{}
	}
	ast_ari_new_bridge(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_bridge(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /bridges\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /bridges\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	return;
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_get_bridge_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_get_bridge_args args = {};
	struct ast_variable *i;
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	ast_ari_get_bridge(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
	case 404: /* Bridge not found */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_bridge(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /bridges/{bridgeId}\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /bridges/{bridgeId}\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	return;
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_delete_bridge_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_delete_bridge_args args = {};
	struct ast_variable *i;
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	ast_ari_delete_bridge(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
	case 404: /* Bridge not found */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_void(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /bridges/{bridgeId}\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /bridges/{bridgeId}\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	return;
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}/addChannel.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_add_channel_to_bridge_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_add_channel_to_bridge_args args = {};
	struct ast_variable *i;
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "channel") == 0) {
			/* Parse comma separated list */
			char *vals[MAX_VALS];
			size_t j;

			args.channel_parse = ast_strdup(i->value);
			if (!args.channel_parse) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			args.channel_count = ast_app_separate_args(
				args.channel_parse, ',', vals, ARRAY_LEN(vals));
			if (args.channel_count == 0) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			if (args.channel_count >= MAX_VALS) {
				ast_ari_response_error(response, 400,
					"Bad Request",
					"Too many values for channel");
				goto fin;
			}

			args.channel = ast_malloc(sizeof(*args.channel) * args.channel_count);
			if (!args.channel) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			for (j = 0; j < args.channel_count; ++j) {
				args.channel[j] = (vals[j]);
			}
		} else
		if (strcmp(i->name, "role") == 0) {
			args.role = (i->value);
		} else
		{}
	}
	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	ast_ari_add_channel_to_bridge(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
	case 404: /* Bridge not found */
	case 409: /* Bridge not in Stasis application */
	case 422: /* Channel not found, or not in Stasis application */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_void(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /bridges/{bridgeId}/addChannel\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /bridges/{bridgeId}/addChannel\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	ast_free(args.channel_parse);
	ast_free(args.channel);
	return;
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}/removeChannel.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_remove_channel_from_bridge_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_remove_channel_from_bridge_args args = {};
	struct ast_variable *i;
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "channel") == 0) {
			/* Parse comma separated list */
			char *vals[MAX_VALS];
			size_t j;

			args.channel_parse = ast_strdup(i->value);
			if (!args.channel_parse) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			args.channel_count = ast_app_separate_args(
				args.channel_parse, ',', vals, ARRAY_LEN(vals));
			if (args.channel_count == 0) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			if (args.channel_count >= MAX_VALS) {
				ast_ari_response_error(response, 400,
					"Bad Request",
					"Too many values for channel");
				goto fin;
			}

			args.channel = ast_malloc(sizeof(*args.channel) * args.channel_count);
			if (!args.channel) {
				ast_ari_response_alloc_failed(response);
				goto fin;
			}

			for (j = 0; j < args.channel_count; ++j) {
				args.channel[j] = (vals[j]);
			}
		} else
		{}
	}
	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	ast_ari_remove_channel_from_bridge(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_void(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /bridges/{bridgeId}/removeChannel\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /bridges/{bridgeId}/removeChannel\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	ast_free(args.channel_parse);
	ast_free(args.channel);
	return;
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}/mohStart.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_moh_start_bridge_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_moh_start_bridge_args args = {};
	struct ast_variable *i;
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "mohClass") == 0) {
			args.moh_class = (i->value);
		} else
		{}
	}
	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	ast_ari_moh_start_bridge(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
	case 404: /* Bridge not found */
	case 409: /* Bridge not in Stasis application */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_void(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /bridges/{bridgeId}/mohStart\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /bridges/{bridgeId}/mohStart\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	return;
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}/mohStop.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_moh_stop_bridge_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_moh_stop_bridge_args args = {};
	struct ast_variable *i;
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	ast_ari_moh_stop_bridge(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
	case 404: /* Bridge not found */
	case 409: /* Bridge not in Stasis application */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_void(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /bridges/{bridgeId}/mohStop\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /bridges/{bridgeId}/mohStop\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	return;
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}/play.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_play_on_bridge_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_play_on_bridge_args args = {};
	struct ast_variable *i;
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "media") == 0) {
			args.media = (i->value);
		} else
		if (strcmp(i->name, "lang") == 0) {
			args.lang = (i->value);
		} else
		if (strcmp(i->name, "offsetms") == 0) {
			args.offsetms = atoi(i->value);
		} else
		if (strcmp(i->name, "skipms") == 0) {
			args.skipms = atoi(i->value);
		} else
		{}
	}
	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	ast_ari_play_on_bridge(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
	case 404: /* Bridge not found */
	case 409: /* Bridge not in a Stasis application */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_playback(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /bridges/{bridgeId}/play\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /bridges/{bridgeId}/play\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	return;
}
/*!
 * \brief Parameter parsing callback for /bridges/{bridgeId}/record.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void ast_ari_record_bridge_cb(
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_record_bridge_args args = {};
	struct ast_variable *i;
#if defined(AST_DEVMODE)
	int is_valid;
	int code;
#endif /* AST_DEVMODE */

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "name") == 0) {
			args.name = (i->value);
		} else
		if (strcmp(i->name, "format") == 0) {
			args.format = (i->value);
		} else
		if (strcmp(i->name, "maxDurationSeconds") == 0) {
			args.max_duration_seconds = atoi(i->value);
		} else
		if (strcmp(i->name, "maxSilenceSeconds") == 0) {
			args.max_silence_seconds = atoi(i->value);
		} else
		if (strcmp(i->name, "ifExists") == 0) {
			args.if_exists = (i->value);
		} else
		if (strcmp(i->name, "beep") == 0) {
			args.beep = ast_true(i->value);
		} else
		if (strcmp(i->name, "terminateOn") == 0) {
			args.terminate_on = (i->value);
		} else
		{}
	}
	for (i = path_vars; i; i = i->next) {
		if (strcmp(i->name, "bridgeId") == 0) {
			args.bridge_id = (i->value);
		} else
		{}
	}
	ast_ari_record_bridge(headers, &args, response);
#if defined(AST_DEVMODE)
	code = response->response_code;

	switch (code) {
	case 0: /* Implementation is still a stub, or the code wasn't set */
		is_valid = response->message == NULL;
		break;
	case 500: /* Internal Server Error */
	case 501: /* Not Implemented */
		is_valid = 1;
		break;
	default:
		if (200 <= code && code <= 299) {
			is_valid = ast_ari_validate_live_recording(
				response->message);
		} else {
			ast_log(LOG_ERROR, "Invalid error response %d for /bridges/{bridgeId}/record\n", code);
			is_valid = 0;
		}
	}

	if (!is_valid) {
		ast_log(LOG_ERROR, "Response validation failed for /bridges/{bridgeId}/record\n");
		ast_ari_response_error(response, 500,
			"Internal Server Error", "Response validation failed");
	}
#endif /* AST_DEVMODE */

fin: __attribute__((unused))
	return;
}

/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges_bridgeId_addChannel = {
	.path_segment = "addChannel",
	.callbacks = {
		[AST_HTTP_POST] = ast_ari_add_channel_to_bridge_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges_bridgeId_removeChannel = {
	.path_segment = "removeChannel",
	.callbacks = {
		[AST_HTTP_POST] = ast_ari_remove_channel_from_bridge_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges_bridgeId_mohStart = {
	.path_segment = "mohStart",
	.callbacks = {
		[AST_HTTP_POST] = ast_ari_moh_start_bridge_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges_bridgeId_mohStop = {
	.path_segment = "mohStop",
	.callbacks = {
		[AST_HTTP_POST] = ast_ari_moh_stop_bridge_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges_bridgeId_play = {
	.path_segment = "play",
	.callbacks = {
		[AST_HTTP_POST] = ast_ari_play_on_bridge_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges_bridgeId_record = {
	.path_segment = "record",
	.callbacks = {
		[AST_HTTP_POST] = ast_ari_record_bridge_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges_bridgeId = {
	.path_segment = "bridgeId",
	.is_wildcard = 1,
	.callbacks = {
		[AST_HTTP_GET] = ast_ari_get_bridge_cb,
		[AST_HTTP_DELETE] = ast_ari_delete_bridge_cb,
	},
	.num_children = 6,
	.children = { &bridges_bridgeId_addChannel,&bridges_bridgeId_removeChannel,&bridges_bridgeId_mohStart,&bridges_bridgeId_mohStop,&bridges_bridgeId_play,&bridges_bridgeId_record, }
};
/*! \brief REST handler for /api-docs/bridges.{format} */
static struct stasis_rest_handlers bridges = {
	.path_segment = "bridges",
	.callbacks = {
		[AST_HTTP_GET] = ast_ari_get_bridges_cb,
		[AST_HTTP_POST] = ast_ari_new_bridge_cb,
	},
	.num_children = 1,
	.children = { &bridges_bridgeId, }
};

static int load_module(void)
{
	int res = 0;
	stasis_app_ref();
	res |= ast_ari_add_handler(&bridges);
	return res;
}

static int unload_module(void)
{
	ast_ari_remove_handler(&bridges);
	stasis_app_unref();
	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "RESTful API module - Bridge resources",
	.load = load_module,
	.unload = unload_module,
	.nonoptreq = "res_ari,res_stasis",
	);
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
 * template in rest-api-templates/res_stasis_http_resource.c.mustache
 */

/*! \file
 *
 * \brief Asterisk resources
 *
 * \author David M. Lee, II <dlee@digium.com>
 */

/*** MODULEINFO
	<depend type="module">res_stasis_http</depend>
	<support_level>core</support_level>
 ***/

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/module.h"
#include "stasis_http/resource_asterisk.h"

/*!
 * \brief Parameter parsing callback for /asterisk/info.
 * \param get_params GET parameters in the HTTP request.
 * \param path_vars Path variables extracted from the request.
 * \param headers HTTP headers.
 * \param[out] response Response to the HTTP request.
 */
static void stasis_http_get_asterisk_info_cb(
    struct ast_variable *get_params, struct ast_variable *path_vars,
    struct ast_variable *headers, struct stasis_http_response *response)
{
	struct ast_get_asterisk_info_args args = {};
	struct ast_variable *i;

	for (i = get_params; i; i = i->next) {
		if (strcmp(i->name, "only") == 0) {
			args.only = (i->value);
		} else
		{}
	}
	stasis_http_get_asterisk_info(headers, &args, response);
}

/*! \brief REST handler for /api-docs/asterisk.{format} */
static struct stasis_rest_handlers asterisk_info = {
	.path_segment = "info",
	.callbacks = {
		[AST_HTTP_GET] = stasis_http_get_asterisk_info_cb,
	},
	.num_children = 0,
	.children = {  }
};
/*! \brief REST handler for /api-docs/asterisk.{format} */
static struct stasis_rest_handlers asterisk = {
	.path_segment = "asterisk",
	.callbacks = {
	},
	.num_children = 1,
	.children = { &asterisk_info, }
};

static int load_module(void)
{
	return stasis_http_add_handler(&asterisk);
}

static int unload_module(void)
{
	stasis_http_remove_handler(&asterisk);
	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT,
	"RESTful API module - Asterisk resources",
	.load = load_module,
	.unload = unload_module,
	.nonoptreq = "res_stasis_http",
	);

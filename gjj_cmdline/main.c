/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2018 Intel Corporation
 */

#include <getopt.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "cmdline_parse.h"
#include "cmdline_socket.h"
#include "cmdline_parse_string.h"
#include "cmdline_parse_num.h"
#include "cmdline.h"

#define MAX_PATH_LEN 128
#define MAX_VDPA_SAMPLE_PORTS 1024

struct vdpa_port {
	char ifname[MAX_PATH_LEN];
	//struct rte_vdpa_device *dev;
	int vid;
	uint64_t flags;
	int stats_n;
	//struct rte_vdpa_stat_name *stats_names;
	//struct rte_vdpa_stat *stats;
};

static struct vdpa_port vports[MAX_VDPA_SAMPLE_PORTS];

static char iface[MAX_PATH_LEN];
static int devcnt;
static int interactive = 1;
static int client_mode;

/* display usage */
static void
vdpa_usage(const char *prgname)
{
	printf("Usage: %s [EAL options] -- "
				 "	--interactive|-i: run in interactive mode.\n"
				 "	--iface <path>: specify the path prefix of the socket files, e.g. /tmp/vhost-user-.\n"
				 "	--client: register a vhost-user socket as client mode.\n",
				 prgname);
}

static int
parse_args(int argc, char **argv)
{
	static const char *short_option = "i";
	static struct option long_option[] = {
		{"iface", required_argument, NULL, 0},
		{"interactive", no_argument, &interactive, 1},
		{"client", no_argument, &client_mode, 1},
		{NULL, 0, 0, 0},
	};
	int opt, idx;
	char *prgname = argv[0];

	while ((opt = getopt_long(argc, argv, short_option, long_option, &idx))
			!= EOF) {
		switch (opt) {
		case 'i':
			printf("Interactive-mode selected\n");
			interactive = 1;
			break;
		/* long options */
		case 0:
			if (strncmp(long_option[idx].name, "iface",
						MAX_PATH_LEN) == 0) {
				//rte_strscpy(iface, optarg, MAX_PATH_LEN);
				printf("iface %s\n", iface);
			}
			if (!strcmp(long_option[idx].name, "interactive")) {
				printf("Interactive-mode selected\n");
				interactive = 1;
			}
			break;

		default:
			vdpa_usage(prgname);
			return -1;
		}
	}

	if (iface[0] == '\0' && interactive == 0) {
		vdpa_usage(prgname);
		return -1;
	}

	return 0;
}

static int
new_device(int vid)
{
	return 0;
}

static void
destroy_device(int vid)
{
}


static int
vdpa_blk_device_set_features_and_protocol(const char *path)
{
	return 0;

}

static int
start_vdpa(struct vdpa_port *vport)
{
	return 0;
}

static void
close_vdpa(struct vdpa_port *vport)
{
}

static void
vdpa_sample_quit(void)
{
	int i;
	for (i = 0; i < RTE_MIN(MAX_VDPA_SAMPLE_PORTS, devcnt); i++) {
		if (vports[i].ifname[0] != '\0')
			close_vdpa(&vports[i]);
	}
}

static void
signal_handler(int signum)
{
	if (signum == SIGINT || signum == SIGTERM) {
		printf("\nSignal %d received, preparing to exit...\n", signum);
		vdpa_sample_quit();
		exit(0);
	}
}

/* interactive cmds */

/* *** Help command with introduction. *** */
struct cmd_help_result {
	cmdline_fixed_string_t help;
};

static void cmd_help_parsed(__rte_unused void *parsed_result,
		struct cmdline *cl,
		__rte_unused void *data)
{
	cmdline_printf(
		cl,
		"\n"
		"The following commands are currently available:\n\n"
		"Control:\n"
		"    help                                      : Show interactive instructions.\n"
		"    list                                      : list all available vdpa devices.\n"
		"    create <socket file> <vdev addr>          : create a new vdpa port.\n"
		"    stats <device ID> <virtio queue ID>       : show statistics of virtio queue, 0xffff for all.\n"
		"    quit                                      : exit vdpa sample app.\n"
	);
}

cmdline_parse_token_string_t cmd_help_help =
	TOKEN_STRING_INITIALIZER(struct cmd_help_result, help, "help");

cmdline_parse_inst_t cmd_help = {
	.f = cmd_help_parsed,
	.data = NULL,
	.help_str = "show help",
	.tokens = {
		(void *)&cmd_help_help,
		NULL,
	},
};

/* *** List all available vdpa devices *** */
struct cmd_list_result {
	cmdline_fixed_string_t action;
};

static void cmd_list_vdpa_devices_parsed(
		__rte_unused void *parsed_result,
		struct cmdline *cl,
		__rte_unused void *data)
{
}

cmdline_parse_token_string_t cmd_action_list =
	TOKEN_STRING_INITIALIZER(struct cmd_list_result, action, "list");

cmdline_parse_inst_t cmd_list_vdpa_devices = {
	.f = cmd_list_vdpa_devices_parsed,
	.data = NULL,
	.help_str = "list all available vdpa devices",
	.tokens = {
		(void *)&cmd_action_list,
		NULL,
	},
};

/* *** Create new vdpa port *** */
struct cmd_create_result {
	cmdline_fixed_string_t action;
	cmdline_fixed_string_t socket_path;
	cmdline_fixed_string_t bdf;
};

static void cmd_create_vdpa_port_parsed(void *parsed_result,
		struct cmdline *cl,
		__rte_unused void *data)
{
}

cmdline_parse_token_string_t cmd_action_create =
	TOKEN_STRING_INITIALIZER(struct cmd_create_result, action, "create");
cmdline_parse_token_string_t cmd_socket_path =
	TOKEN_STRING_INITIALIZER(struct cmd_create_result, socket_path, NULL);
cmdline_parse_token_string_t cmd_bdf =
	TOKEN_STRING_INITIALIZER(struct cmd_create_result, bdf, NULL);

cmdline_parse_inst_t cmd_create_vdpa_port = {
	.f = cmd_create_vdpa_port_parsed,
	.data = NULL,
	.help_str = "create a new vdpa port",
	.tokens = {
		(void *)&cmd_action_create,
		(void *)&cmd_socket_path,
		(void *)&cmd_bdf,
		NULL,
	},
};

/* *** STATS *** */
struct cmd_stats_result {
	cmdline_fixed_string_t stats;
	cmdline_fixed_string_t bdf;
	uint16_t qid;
};

static void cmd_device_stats_parsed(void *parsed_result, struct cmdline *cl,
				    __rte_unused void *data)
{
}

cmdline_parse_token_string_t cmd_device_stats_ =
	TOKEN_STRING_INITIALIZER(struct cmd_stats_result, stats, "stats");
cmdline_parse_token_string_t cmd_device_bdf =
	TOKEN_STRING_INITIALIZER(struct cmd_stats_result, bdf, NULL);
cmdline_parse_token_num_t cmd_queue_id =
	TOKEN_NUM_INITIALIZER(struct cmd_stats_result, qid, RTE_UINT32, "qid");

cmdline_parse_inst_t cmd_device_stats = {
	.f = cmd_device_stats_parsed,
	.data = NULL,
	.help_str = "stats: show device statistics",
	.tokens = {
		(void *)&cmd_device_stats_,
		(void *)&cmd_device_bdf,
		(void *)&cmd_queue_id,
		NULL,
	},
};


#if 1
struct cmd_i2crw {
	cmdline_fixed_string_t aa;
	cmdline_fixed_string_t bb;
	uint32_t addr;
	uint32_t val;
	cmdline_fixed_string_t cc;
};


static void cmd_i2c_rd( void *parsed_result, struct cmdline *cl, void *data)
{
    struct cmd_i2crw *re = (struct cmd_i2crw *)parsed_result;

    printf("addr=0x%x\r\n", re->addr);
}

static void cmd_i2c_wr( void *parsed_result, struct cmdline *cl, void *data)
{
    struct cmd_i2crw *re = (struct cmd_i2crw *)parsed_result;

    printf("addr=0x%x, val=0x%x\r\n", re->addr, re->val);
}


static void cmd_i2c_dis( void *parsed_result, struct cmdline *cl, void *data)
{
    struct cmd_i2crw *re = (struct cmd_i2crw *)parsed_result;

    printf("cc=%s\r\n", re->cc);
}


cmdline_parse_token_string_t cmd_para_iic =
	TOKEN_STRING_INITIALIZER(struct cmd_i2crw, aa, "iic");
cmdline_parse_token_string_t cmd_para_iicrd =
	TOKEN_STRING_INITIALIZER(struct cmd_i2crw, bb, "rd");
cmdline_parse_token_string_t cmd_para_iicwr =
	TOKEN_STRING_INITIALIZER(struct cmd_i2crw, bb, "wr");
cmdline_parse_token_string_t cmd_para_iicdis =
    TOKEN_STRING_INITIALIZER(struct cmd_i2crw, bb, "dis");

cmdline_parse_token_num_t cmd_para_iicaddr =
	TOKEN_NUM_INITIALIZER(struct cmd_i2crw, addr, RTE_UINT32, "addr");
cmdline_parse_token_num_t cmd_para_iicval =
	TOKEN_NUM_INITIALIZER(struct cmd_i2crw, val, RTE_UINT32, "val");
cmdline_parse_token_string_t cmd_para_iiccc =
    TOKEN_STRING_INITIALIZER(struct cmd_i2crw, cc, 0);


cmdline_parse_inst_t cmd_iicrd = {
    .f = cmd_i2c_rd,
    .data = NULL,
    .help_str = NULL,
    .tokens = {
        (void *)&cmd_para_iic,
        (void *)&cmd_para_iicrd,
        (void *)&cmd_para_iicaddr,
        NULL,
    },
};

cmdline_parse_inst_t cmd_iicwr = {
    .f = cmd_i2c_wr,
    .data = NULL,
    .help_str = NULL,
    .tokens = {
        (void *)&cmd_para_iic,
        (void *)&cmd_para_iicwr,
        (void *)&cmd_para_iicaddr,
        (void *)&cmd_para_iicval,
        NULL,
    },
};

cmdline_parse_inst_t cmd_iicdis = {
    .f = cmd_i2c_dis,
    .data = NULL,
    .help_str = NULL,
    .tokens = {
        (void *)&cmd_para_iic,
        (void *)&cmd_para_iicdis,
        (void *)&cmd_para_iiccc,
        NULL,
    },
};


#endif

/* *** QUIT *** */
struct cmd_quit_result {
	cmdline_fixed_string_t quit;
};

static void cmd_quit_parsed(__rte_unused void *parsed_result,
		struct cmdline *cl,
		__rte_unused void *data)
{
	vdpa_sample_quit();
	cmdline_quit(cl);
}

cmdline_parse_token_string_t cmd_quit_quit =
	TOKEN_STRING_INITIALIZER(struct cmd_quit_result, quit, "q");

cmdline_parse_inst_t cmd_quit = {
	.f = cmd_quit_parsed,
	.data = NULL,
	.help_str = "quit: exit application",
	.tokens = {
		(void *)&cmd_quit_quit,
		NULL,
	},
};



cmdline_parse_ctx_t main_ctx[] = {
    //(cmdline_parse_inst_t *)&cmd_help,
    //(cmdline_parse_inst_t *)&cmd_list_vdpa_devices,
    //(cmdline_parse_inst_t *)&cmd_create_vdpa_port,
    //(cmdline_parse_inst_t *)&cmd_device_stats,
    (cmdline_parse_inst_t *)&cmd_iicrd,
    (cmdline_parse_inst_t *)&cmd_iicwr,
    (cmdline_parse_inst_t *)&cmd_iicdis,
    (cmdline_parse_inst_t *)&cmd_quit,
    NULL,
};

int main(int argc, char *argv[])
{
    char ch;
    int ret;
    struct cmdline *cl;
    argc -= ret;
    argv += ret;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);


    if (1) {
        cl = cmdline_stdin_new(main_ctx, "BZW> ");
        if (cl == NULL) {
            printf("Cannot create cmdline instance\n");

        }
        cmdline_interact(cl);
        cmdline_stdin_exit(cl);
    } 


    return 0;
}



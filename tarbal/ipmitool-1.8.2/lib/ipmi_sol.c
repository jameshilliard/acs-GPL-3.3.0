/*
 * Copyright (c) 2003 Sun Microsystems, Inc.  All Rights Reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * Redistribution of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * Redistribution in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of Sun Microsystems, Inc. or the names of
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * 
 * This software is provided "AS IS," without a warranty of any kind.
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES,
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.
 * SUN MICROSYSTEMS, INC. ("SUN") AND ITS LICENSORS SHALL NOT BE LIABLE
 * FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.  IN NO EVENT WILL
 * SUN OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA,
 * OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR
 * PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF
 * LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * 
 * You acknowledge that this software is not designed or intended for use
 * in the design, construction, operation or maintenance of any nuclear
 * facility.
 */

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>


#include <termios.h>

#include <ipmitool/helper.h>
#include <ipmitool/log.h>
#include <ipmitool/ipmi.h>
#include <ipmitool/ipmi_intf.h>
#include <ipmitool/ipmi_sol.h>
#include <ipmitool/ipmi_strings.h>
#include <ipmitool/bswap.h>


#define SOL_PARAMETER_SET_IN_PROGRESS           0x00
#define SOL_PARAMETER_SOL_ENABLE                0x01
#define SOL_PARAMETER_SOL_AUTHENTICATION        0x02
#define SOL_PARAMETER_CHARACTER_INTERVAL        0x03
#define SOL_PARAMETER_SOL_RETRY                 0x04
#define SOL_PARAMETER_SOL_NON_VOLATILE_BIT_RATE 0x05
#define SOL_PARAMETER_SOL_VOLATILE_BIT_RATE     0x06
#define SOL_PARAMETER_SOL_PAYLOAD_CHANNEL       0x07
#define SOL_PARAMETER_SOL_PAYLOAD_PORT          0x08

#define SOL_ESCAPE_CHARACTER                    '~'

const struct valstr sol_parameter_vals[] = {
	{ SOL_PARAMETER_SET_IN_PROGRESS,           "Set In Progress (0)" },
	{ SOL_PARAMETER_SOL_ENABLE,                "Enable (1)" },
	{ SOL_PARAMETER_SOL_AUTHENTICATION,        "Authentication (2)" },
	{ SOL_PARAMETER_CHARACTER_INTERVAL,        "Character Interval (3)" },
	{ SOL_PARAMETER_SOL_RETRY,                 "Retry (4)" },
	{ SOL_PARAMETER_SOL_NON_VOLATILE_BIT_RATE, "Nonvolatile Bitrate (5)" },
	{ SOL_PARAMETER_SOL_VOLATILE_BIT_RATE,     "Volatile Bitrate (6)" },
	{ SOL_PARAMETER_SOL_PAYLOAD_CHANNEL,       "Payload Channel (7)" },
	{ SOL_PARAMETER_SOL_PAYLOAD_PORT,          "Payload Port (8)" },
	{ 0x00, NULL },
};


static struct termios _saved_tio;
static int            _in_raw_mode = 0;

extern int verbose;


/*
 * ipmi_get_sol_info
 */
int
ipmi_get_sol_info(
				  struct ipmi_intf * intf,
				  uint8_t channel,
				  struct sol_config_parameters * params)
{
	struct ipmi_rs * rsp;
	struct ipmi_rq req;
	uint8_t data[4];	

	req.msg.netfn    = IPMI_NETFN_TRANSPORT;
	req.msg.cmd      = IMPI_GET_SOL_CONFIG_PARAMETERS;
	req.msg.data_len = 4;
	req.msg.data     = data;


	/*
	 * set in progress
	 */
	memset(data, 0, sizeof(data));
	data[0] = channel;                       /* channel number     */
	data[1] = SOL_PARAMETER_SET_IN_PROGRESS; /* parameter selector */
	data[2] = 0x00;                          /* set selector       */
	data[3] = 0x00;                          /* block selector     */
	
	rsp = intf->sendrecv(intf, &req);

	if (NULL != rsp) {
		switch (rsp->ccode) {
			case 0x00: 
				if (rsp->data_len == 2) {
					params->set_in_progress = rsp->data[1];
				} else {
					lprintf(LOG_ERR, "Error: Unexpected data length (%d) received "
						   "for SOL parameter '%s'",
						   rsp->data_len,
						   val2str(data[1], sol_parameter_vals));
				}
				break;
			case 0x80:
				lprintf(LOG_ERR, "Info: SOL parameter '%s' not supported", 
				    val2str(data[1], sol_parameter_vals));
				break;
			default:
				lprintf(LOG_ERR, "Error requesting SOL parameter '%s': %s",
					val2str(data[1], sol_parameter_vals),	
					val2str(rsp->ccode, completion_code_vals));
				return -1;
		}
	} else {
		lprintf(LOG_ERR, "Error: No response requesting SOL parameter '%s'",
			    val2str(data[1], sol_parameter_vals));
	}

	/*
	 * SOL enable
	 */
 	memset(data, 0, sizeof(data));
	data[0] = channel;                  /* channel number     */
	data[1] = SOL_PARAMETER_SOL_ENABLE; /* parameter selector */
	data[2] = 0x00;                     /* set selector       */
	data[3] = 0x00;                     /* block selector     */
	
	rsp = intf->sendrecv(intf, &req);

	if (NULL != rsp) {
		switch (rsp->ccode) {
			case 0x00: 
				if (rsp->data_len == 2) {
					params->enabled = rsp->data[1];
				} else {
					lprintf(LOG_ERR, "Error: Unexpected data length (%d) received "
						   "for SOL parameter '%s'",
						   rsp->data_len,
						   val2str(data[1], sol_parameter_vals));
				}
				break;
			case 0x80:
				lprintf(LOG_ERR, "Info: SOL parameter '%s' not supported", 
				    val2str(data[1], sol_parameter_vals));
				break;
			default:
				lprintf(LOG_ERR, "Error requesting SOL parameter '%s': %s",
					val2str(data[1], sol_parameter_vals),
					val2str(rsp->ccode, completion_code_vals));
				return -1;
		}
	} else {
		lprintf(LOG_ERR, "Error: No response requesting SOL parameter '%s'",
			    val2str(data[1], sol_parameter_vals));
	}

	/*
	 * SOL authentication
	 */
	memset(data, 0, sizeof(data));
	data[0] = channel;                          /* channel number     */
	data[1] = SOL_PARAMETER_SOL_AUTHENTICATION; /* parameter selector */
	data[2] = 0x00;                             /* set selector       */
	data[3] = 0x00;                             /* block selector     */
	
	rsp = intf->sendrecv(intf, &req);
	if (NULL != rsp) {
		switch (rsp->ccode) {
			case 0x00: 
				if (rsp->data_len == 2) {
					params->force_encryption     = ((rsp->data[1] & 0x80)? 1 : 0);
					params->force_authentication = ((rsp->data[1] & 0x40)? 1 : 0);
					params->privilege_level      = rsp->data[1] & 0x0F;
				} else {
					lprintf(LOG_ERR, "Error: Unexpected data length (%d) received "
						   "for SOL parameter '%s'",
						   rsp->data_len,
						   val2str(data[1], sol_parameter_vals));
				}
				break;
			case 0x80:
				lprintf(LOG_ERR, "Info: SOL parameter '%s' not supported", 
				    val2str(data[1], sol_parameter_vals));
				break;
			default:
				lprintf(LOG_ERR, "Error requesting SOL parameter '%s': %s",
					val2str(data[1], sol_parameter_vals),
					val2str(rsp->ccode, completion_code_vals));
				return -1;
		}
	} else {
		lprintf(LOG_ERR, "Error: No response requesting SOL parameter '%s'",
			    val2str(data[1], sol_parameter_vals));
	}

	/*
	 * Character accumulate interval and character send interval
	 */
	memset(data, 0, sizeof(data));
	data[0] = channel;                          /* channel number     */
	data[1] = SOL_PARAMETER_CHARACTER_INTERVAL; /* parameter selector */
	data[2] = 0x00;                             /* set selector       */
	data[3] = 0x00;                             /* block selector     */
	
	rsp = intf->sendrecv(intf, &req);
	if (NULL != rsp) {
		switch (rsp->ccode) {
			case 0x00: 
				if (rsp->data_len == 3) {
					params->character_accumulate_level = rsp->data[1];
					params->character_send_threshold   = rsp->data[2];
				} else {
					lprintf(LOG_ERR, "Error: Unexpected data length (%d) received "
						   "for SOL parameter '%s'",
						   rsp->data_len,
						   val2str(data[1], sol_parameter_vals));
				}
				break;
			case 0x80:
				lprintf(LOG_ERR, "Info: SOL parameter '%s' not supported", 
				    val2str(data[1], sol_parameter_vals));
				break;
			default:
				lprintf(LOG_ERR, "Error requesting SOL parameter '%s': %s",
					val2str(data[1], sol_parameter_vals),
					val2str(rsp->ccode, completion_code_vals));
				return -1;
		}
	} else {
		lprintf(LOG_ERR, "Error: No response requesting SOL parameter '%s'",
			    val2str(data[1], sol_parameter_vals));
	}

	/*
	 * SOL retry
	 */
	memset(data, 0, sizeof(data));
	data[0] = channel;                 /* channel number     */
	data[1] = SOL_PARAMETER_SOL_RETRY; /* parameter selector */
	data[2] = 0x00;                    /* set selector       */
	data[3] = 0x00;                    /* block selector     */
	
	rsp = intf->sendrecv(intf, &req);
	if (NULL != rsp) {
		switch (rsp->ccode) {
			case 0x00: 
				if (rsp->data_len == 3) {
					params->retry_count    = rsp->data[1];
					params->retry_interval = rsp->data[2]; 
				} else {
					lprintf(LOG_ERR, "Error: Unexpected data length (%d) received "
						   "for SOL parameter '%s'",
						   rsp->data_len,
						   val2str(data[1], sol_parameter_vals));
				}
				break;
			case 0x80:
				lprintf(LOG_ERR, "Info: SOL parameter '%s' not supported", 
				    val2str(data[1], sol_parameter_vals));
				break;
			default:
				lprintf(LOG_ERR, "Error requesting SOL parameter '%s': %s",
					val2str(data[1], sol_parameter_vals),
					val2str(rsp->ccode, completion_code_vals));
				return -1;
		}
	} else {
		lprintf(LOG_ERR, "Error: No response requesting SOL parameter '%s'",
			    val2str(data[1], sol_parameter_vals));
	}

	/*
	 * SOL non-volatile bit rate
	 */
	memset(data, 0, sizeof(data));
	data[0] = channel;                                 /* channel number     */
 	data[1] = SOL_PARAMETER_SOL_NON_VOLATILE_BIT_RATE; /* parameter selector */
 	data[2] = 0x00;                                    /* set selector       */
	data[3] = 0x00;                                    /* block selector     */
	
	rsp = intf->sendrecv(intf, &req);
	if (NULL != rsp) {
		switch (rsp->ccode) {
			case 0x00: 
				if (rsp->data_len == 2) {
					params->non_volatile_bit_rate = rsp->data[1] & 0x0F;
				} else {
					lprintf(LOG_ERR, "Error: Unexpected data length (%d) received "
						   "for SOL parameter '%s'",
						   rsp->data_len,
						   val2str(data[1], sol_parameter_vals));
				}
				break;
			case 0x80:
				lprintf(LOG_ERR, "Info: SOL parameter '%s' not supported", 
				    val2str(data[1], sol_parameter_vals));
				break;
			default:
				lprintf(LOG_ERR, "Error requesting SOL parameter '%s': %s",
					val2str(data[1], sol_parameter_vals),
					val2str(rsp->ccode, completion_code_vals));
				return -1;
		}
	} else {
		lprintf(LOG_ERR, "Error: No response requesting SOL parameter '%s'",
			    val2str(data[1], sol_parameter_vals));
	}

	/*
	 * SOL volatile bit rate
	 */
	memset(data, 0, sizeof(data));
	data[0] = channel;                             /* channel number     */
	data[1] = SOL_PARAMETER_SOL_VOLATILE_BIT_RATE; /* parameter selector */
	data[2] = 0x00;                                /* set selector       */
	data[3] = 0x00;                                /* block selector     */
	
	rsp = intf->sendrecv(intf, &req);
	if (NULL != rsp) {
		switch (rsp->ccode) {
			case 0x00: 
				if (rsp->data_len == 2) {
					params->volatile_bit_rate = rsp->data[1] & 0x0F;
				} else {
					lprintf(LOG_ERR, "Error: Unexpected data length (%d) received "
						   "for SOL parameter '%s'",
						   rsp->data_len,
						   val2str(data[1], sol_parameter_vals));
				}
				break;
			case 0x80:
				lprintf(LOG_ERR, "Info: SOL parameter '%s' not supported", 
				    val2str(data[1], sol_parameter_vals));
				break;
			default:
				lprintf(LOG_ERR, "Error requesting SOL parameter '%s': %s",
					val2str(data[1], sol_parameter_vals),
					val2str(rsp->ccode, completion_code_vals));
				return -1;
		}
	} else {
		lprintf(LOG_ERR, "Error: No response requesting SOL parameter '%s'",
			    val2str(data[1], sol_parameter_vals));
	}

	/*
	 * SOL payload channel
	 */
	memset(data, 0, sizeof(data));
	data[0] = channel;                           /* channel number     */
	data[1] = SOL_PARAMETER_SOL_PAYLOAD_CHANNEL; /* parameter selector */
	data[2] = 0x00;                              /* set selector       */
	data[3] = 0x00;                              /* block selector     */
	
	rsp = intf->sendrecv(intf, &req);
	if (NULL != rsp) {
		switch (rsp->ccode) {
			case 0x00: 
				if (rsp->data_len == 2) {
					params->payload_channel = rsp->data[1];
				} else {
					lprintf(LOG_ERR, "Error: Unexpected data length (%d) received "
						   "for SOL parameter '%s'",
						   rsp->data_len,
						   val2str(data[1], sol_parameter_vals));
				}
				break;
			case 0x80:
				lprintf(LOG_ERR, "Info: SOL parameter '%s' not supported - defaulting to 0x%02x", 
				    val2str(data[1], sol_parameter_vals), channel);
				params->payload_channel = channel;
				break;
			default:
				lprintf(LOG_ERR, "Error requesting SOL parameter '%s': %s",
					val2str(data[1], sol_parameter_vals),
					val2str(rsp->ccode, completion_code_vals));
				return -1;
		}
	} else {
		lprintf(LOG_ERR, "Error: No response requesting SOL parameter '%s'",
			    val2str(data[1], sol_parameter_vals));
	}

	/*
	 * SOL payload port
	 */
	memset(data, 0, sizeof(data));
	data[0] = channel;                        /* channel number     */
	data[1] = SOL_PARAMETER_SOL_PAYLOAD_PORT; /* parameter selector */
	data[2] = 0x00;                           /* set selector       */
	data[3] = 0x00;                           /* block selector     */
	
	rsp = intf->sendrecv(intf, &req);
	if (NULL != rsp) {
		switch (rsp->ccode) {
			case 0x00: 
				if (rsp->data_len == 3) {
					params->payload_port = (rsp->data[1]) |	(rsp->data[2] << 8);
				} else {
					lprintf(LOG_ERR, "Error: Unexpected data length (%d) received "
						   "for SOL parameter '%s'",
						   rsp->data_len,
						   val2str(data[1], sol_parameter_vals));
				}
				break;
			case 0x80:
				lprintf(LOG_ERR, "Info: SOL parameter '%s' not supported - defaulting to %d", 
				    val2str(data[1], sol_parameter_vals), intf->session->port);
				params->payload_port = intf->session->port;
				break;
			default:
				lprintf(LOG_ERR, "Error requesting SOL parameter '%s': %s",
					val2str(data[1], sol_parameter_vals),
					val2str(rsp->ccode, completion_code_vals));
				return -1;
		}
	} else {
		lprintf(LOG_ERR, "Error: No response requesting SOL parameter '%s'",
			    val2str(data[1], sol_parameter_vals));
	}

	return 0;
}



/*
 * ipmi_print_sol_info
 */
static int
ipmi_print_sol_info(struct ipmi_intf * intf, uint8_t channel)
{
	struct sol_config_parameters params = {0};
	if (ipmi_get_sol_info(intf, channel, &params))
		return -1;
	
	if (csv_output)
	{
		printf("%s,",
			   val2str(params.set_in_progress & 0x03,
					   ipmi_set_in_progress_vals));
		printf("%s,", params.enabled?"true": "false");
		printf("%s,", params.force_encryption?"true":"false");
		printf("%s,", params.force_encryption?"true":"false");
		printf("%s,",
			   val2str(params.privilege_level, ipmi_privlvl_vals));
		printf("%d,", params.character_accumulate_level * 5);
		printf("%d,", params.character_send_threshold);
		printf("%d,", params.retry_count);
		printf("%d,", params.retry_interval * 10);

		printf("%s,",
			   val2str(params.volatile_bit_rate, impi_bit_rate_vals));

		printf("%s,",
			   val2str(params.non_volatile_bit_rate, impi_bit_rate_vals));

		printf("%d,", params.payload_channel);
		printf("%d\n", params.payload_port);
	}
	else
	{
		printf("Set in progress                 : %s\n",
			   val2str(params.set_in_progress & 0x03,
					   ipmi_set_in_progress_vals));
		printf("Enabled                         : %s\n",
			   params.enabled?"true": "false");
		printf("Force Encryption                : %s\n",
			   params.force_encryption?"true":"false");
		printf("Force Authentication            : %s\n",
			   params.force_authentication?"true":"false");
		printf("Privilege Level                 : %s\n",
			   val2str(params.privilege_level, ipmi_privlvl_vals));
		printf("Character Accumulate Level (ms) : %d\n",
			   params.character_accumulate_level * 5);
		printf("Character Send Threshold        : %d\n",
			   params.character_send_threshold);
		printf("Retry Count                     : %d\n",
			   params.retry_count);
		printf("Retry Interval (ms)             : %d\n",
			   params.retry_interval * 10);

		printf("Volatile Bit Rate (kbps)        : %s\n",
			   val2str(params.volatile_bit_rate, impi_bit_rate_vals));

		printf("Non-Volatile Bit Rate (kbps)    : %s\n",
			   val2str(params.non_volatile_bit_rate, impi_bit_rate_vals));

		printf("Payload Channel                 : %d (0x%02x)\n",
			   params.payload_channel, params.payload_channel);
		printf("Payload Port                    : %d\n",
			   params.payload_port);
	}

	return 0;
}



/*
 * ipmi_sol_set_param
 *
 * Set the specified Serial Over LAN value to the specified
 * value
 *
 * return 0 on success,
 *        -1 on failure
 */
static int
ipmi_sol_set_param(struct ipmi_intf * intf,
				   uint8_t      channel,
				   const char       * param,
				   const char       * value)
{
	struct ipmi_rs * rsp;
	struct ipmi_rq   req;
	uint8_t    data[4];	 
	int              bGuarded = 1; /* Use set-in-progress indicator? */

	req.msg.netfn    = IPMI_NETFN_TRANSPORT;           /* 0x0c */ 
	req.msg.cmd      = IMPI_SET_SOL_CONFIG_PARAMETERS; /* 0x21 */
	req.msg.data     = data;
	
	data[0] = channel;
	

	/*
	 * set-in-progress
	 */
	if (! strcmp(param, "set-in-progress"))
	{
		bGuarded = 0; /* We _ARE_ the set-in-progress indicator */
		req.msg.data_len = 3;
		data[1] = SOL_PARAMETER_SET_IN_PROGRESS;

		if (! strcmp(value, "set-complete"))
			data[2] = 0x00;
		else if (! strcmp(value, "set-in-progress"))
			data[2] = 0x01;
		else if (! strcmp(value, "commit-write"))
			data[2] = 0x02;
		else
		{
			lprintf(LOG_ERR, "Invalid value %s for parameter %s",
				   value, param);
			lprintf(LOG_ERR, "Valid values are set-complete, set-in-progress "
				   "and commit-write");
			return -1;
		}
	}


	/*
	 * enabled
	 */
	else if (! strcmp(param, "enabled"))
	{
		req.msg.data_len = 3;
		data[1] = SOL_PARAMETER_SOL_ENABLE;

		if (! strcmp(value, "true"))
			data[2] = 0x01;
		else if (! strcmp(value, "false"))
			data[2] = 0x00;
		else
		{
			lprintf(LOG_ERR, "Invalid value %s for parameter %s",
				   value, param);
			lprintf(LOG_ERR, "Valid values are true and false");
			return -1;
		}
	}


	/*
	 * force-payload-encryption 
	 */
	else if (! strcmp(param, "force-encryption"))
	{
		struct sol_config_parameters params;

		req.msg.data_len = 3;
		data[1] = SOL_PARAMETER_SOL_AUTHENTICATION;

		if (! strcmp(value, "true"))
			data[2] = 0x80;
		else if (! strcmp(value, "false"))
			data[2] = 0x00;
		else
		{
			lprintf(LOG_ERR, "Invalid value %s for parameter %s",
				   value, param);
			lprintf(LOG_ERR, "Valid values are true and false");
			return -1;
		}


		/* We need other values to complete the request */
		if (ipmi_get_sol_info(intf, channel, &params))
		{
			lprintf(LOG_ERR, "Error fetching SOL parameters for %s update",
				   param);
			return -1;
		}
			
		data[2] |= params.force_authentication? 0x40 : 0x00;
		data[2] |= params.privilege_level; 
	}
	

	/*
	 * force-payload-authentication
	 */
	else if (! strcmp(param, "force-authentication"))
	{
		struct sol_config_parameters params;

		req.msg.data_len = 3;
		data[1] = SOL_PARAMETER_SOL_AUTHENTICATION;

		if (! strcmp(value, "true"))
			data[2] = 0x40;
		else if (! strcmp(value, "false"))
			data[2] = 0x00;
		else
		{
			lprintf(LOG_ERR, "Invalid value %s for parameter %s",
				   value, param);
			lprintf(LOG_ERR, "Valid values are true and false");
			return -1;
		}


		/* We need other values to complete the request */
		if (ipmi_get_sol_info(intf, channel, &params))
		{
			lprintf(LOG_ERR, "Error fetching SOL parameters for %s update",
				   param);
			return -1;
		}

		data[2] |= params.force_encryption? 0x80 : 0x00;
		data[2] |= params.privilege_level; 
	}
	

	/*
	 * privilege-level
	 */
	else if (! strcmp(param, "privilege-level"))
	{
		struct sol_config_parameters params;

		req.msg.data_len = 3;
		data[1] = SOL_PARAMETER_SOL_AUTHENTICATION;

		if (! strcmp(value, "user"))
			data[2] = 0x02;
		else if (! strcmp(value, "operator"))
			data[2] = 0x03;
		else if (! strcmp(value, "admin"))
			data[2] = 0x04;
		else if (! strcmp(value, "oem"))
			data[2] = 0x05;
		else
		{
			lprintf(LOG_ERR, "Invalid value %s for parameter %s",
				   value, param);
			lprintf(LOG_ERR, "Valid values are user, operator, admin, and oem");
			return -1;
		}


		/* We need other values to complete the request */
		if (ipmi_get_sol_info(intf, channel, &params))
		{
			lprintf(LOG_ERR, "Error fetching SOL parameters for %s update",
				   param);
			return -1;
		}

		data[2] |= params.force_encryption?     0x80 : 0x00;
		data[2] |= params.force_authentication? 0x40 : 0x00;
	}


	/*
	 * character-accumulate-level
	 */
	else if (! strcmp(param, "character-accumulate-level"))
	{
		struct sol_config_parameters params;

		req.msg.data_len = 4;
		data[1] = SOL_PARAMETER_CHARACTER_INTERVAL;
		data[2] = (uint8_t)strtol(value, NULL, 0);

		/* We need other values to complete the request */
		if (ipmi_get_sol_info(intf, channel, &params))
		{
			lprintf(LOG_ERR, "Error fetching SOL parameters for %s update",
				   param);
			return -1;
		}
		
		data[3] = params.character_send_threshold;
	}


	/*
	 * character-send-threshold
	 */
	else if (! strcmp(param, "character-send-threshold"))
	{
		struct sol_config_parameters params;

		req.msg.data_len = 4;
		data[1] = SOL_PARAMETER_CHARACTER_INTERVAL;
		data[3] = (uint8_t)strtol(value, NULL, 0);

		/* We need other values to complete the request */
		if (ipmi_get_sol_info(intf, channel, &params))
		{
			lprintf(LOG_ERR, "Error fetching SOL parameters for %s update",
				   param);
			return -1;
		}

		data[2] = params.character_accumulate_level;
	}
	

	/*
	 * retry-count
	 */
	else if (! strcmp(param, "retry-count"))
	{
		struct sol_config_parameters params;

		req.msg.data_len = 4;
		data[1] = SOL_PARAMETER_SOL_RETRY;
		data[2] = (uint8_t)strtol(value, NULL, 0) & 0x03;

		/* We need other values to complete the request */
		if (ipmi_get_sol_info(intf, channel, &params))
		{
			lprintf(LOG_ERR, "Error fetching SOL parameters for %s update",
				   param);
			return -1;
		}

		data[3] = params.retry_interval;
	}


	/*
	 * retry-interval
	 */
	else if (! strcmp(param, "retry-interval"))
	{
		struct sol_config_parameters params;

		req.msg.data_len = 4;
		data[1] = SOL_PARAMETER_SOL_RETRY;
		data[3] = (uint8_t)strtol(value, NULL, 0);

		/* We need other values to complete the request */
		if (ipmi_get_sol_info(intf, channel, &params))
		{
			lprintf(LOG_ERR, "Error fetching SOL parameters for %s update",
				   param);
			return -1;
		}
		
		data[2] = params.retry_count;
	}

	
	/*
	 * non-volatile-bit-rate
	 */
	else if (! strcmp(param, "non-volatile-bit-rate"))
	{
		req.msg.data_len = 3;
		data[1] = SOL_PARAMETER_SOL_NON_VOLATILE_BIT_RATE;

		if (!strcmp(value, "serial"))
		{
			data[2] = 0x00;
		}
		else if (!strcmp(value, "9.6"))
		{
			data[2] = 0x06;
		}
		else if (!strcmp(value, "19.2"))
		{
			data[2] = 0x07;
		}
		else if (!strcmp(value, "38.4"))
		{
			data[2] = 0x08;
		}
		else if (!strcmp(value, "57.6"))
		{
			data[2] = 0x09;
		}
		else if (!strcmp(value, "115.2"))
		{
			data[2] = 0x0A;
		}
		else
		{
			lprintf(LOG_ERR, "Invalid value \"%s\" for parameter \"%s\"",
				   value,
				   param);
			lprintf(LOG_ERR, "Valid values are serial, 9.6 19.2, 38.4, 57.6 and 115.2");
			return -1;
		}
	}
	

	/*
	 * volatile-bit-rate
	 */
	else if (! strcmp(param, "volatile-bit-rate"))
	{
		req.msg.data_len = 3;
		data[1] = SOL_PARAMETER_SOL_VOLATILE_BIT_RATE;

		if (!strcmp(value, "serial"))
		{
			data[2] = 0x00;
		}
		else if (!strcmp(value, "9.6"))
		{
			data[2] = 0x06;
		}
		else if (!strcmp(value, "19.2"))
		{
			data[2] = 0x07;
		}
		else if (!strcmp(value, "38.4"))
		{
			data[2] = 0x08;
		}
		else if (!strcmp(value, "57.6"))
		{
			data[2] = 0x09;
		}
		else if (!strcmp(value, "115.2"))
		{
			data[2] = 0x0A;
		}
		else
		{
			lprintf(LOG_ERR, "Invalid value \"%s\" for parameter \"%s\"",
				   value,
				   param);
			lprintf(LOG_ERR, "Valid values are serial, 9.6 19.2, 38.4, 57.6 and 115.2");
			return -1;
		}
	}
	
	else
	{
		lprintf(LOG_ERR, "Error: invalid SOL parameter %s", param);
		return -1;
	}


	/*
	 * Execute the request
	 */
	if (bGuarded &&
		(ipmi_sol_set_param(intf,
							channel,
							"set-in-progress",
							"set-in-progress")))
	{
		lprintf(LOG_ERR, "Error: set of parameter \"%s\" failed", param);
		return -1;
	}


	/* The command proper */
	rsp = intf->sendrecv(intf, &req);

	if (rsp == NULL) {
		lprintf(LOG_ERR, "Error setting SOL parameter '%s'", param);
		return -1;
	}		

	if (rsp->ccode > 0) {
		lprintf(LOG_ERR, "Error setting SOL parameter '%s': %s",
			   param, val2str(rsp->ccode, completion_code_vals));

		if (bGuarded &&
			(ipmi_sol_set_param(intf,
								channel,
								"set-in-progress",
								"set-complete")))
		{
			lprintf(LOG_ERR, "Error could not set \"set-in-progress\" "
				   "to \"set-complete\"");
			return -1;
		}
		
		return -1;
	}


	/*
	 * The commit write could very well fail, but that's ok.
	 * It may not be implemented.
	 */
	if (bGuarded)
		ipmi_sol_set_param(intf,
						   channel,
						   "set-in-progress",
						   "commit-write");


	if (bGuarded &&
 		ipmi_sol_set_param(intf,
		 				   channel,
			 			   "set-in-progress",
						   "set-complete"))
	{
		lprintf(LOG_ERR, "Error could not set \"set-in-progress\" "
			   "to \"set-complete\"");
		return -1;
	}

	return 0;
}



void
leave_raw_mode(void)
{
	if (!_in_raw_mode)
		return;
	if (tcsetattr(fileno(stdin), TCSADRAIN, &_saved_tio) == -1)
		perror("tcsetattr");
	else
		_in_raw_mode = 0;
}



void
enter_raw_mode(void)
{
	struct termios tio;
	if (tcgetattr(fileno(stdin), &tio) == -1) {
		perror("tcgetattr");
		return;
	}
	_saved_tio = tio;
	tio.c_iflag |= IGNPAR;
	tio.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXANY | IXOFF)\
		;
	tio.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHONL);
	//	#ifdef IEXTEN
	tio.c_lflag &= ~IEXTEN;
	//	#endif
	tio.c_oflag &= ~OPOST;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 0;
	if (tcsetattr(fileno(stdin), TCSADRAIN, &tio) == -1)
		perror("tcsetattr");
	else
		_in_raw_mode = 1;
}


static void
sendBreak(struct ipmi_intf * intf)
{
	struct ipmi_v2_payload  v2_payload;

	memset(&v2_payload, 0, sizeof(v2_payload));

	v2_payload.payload.sol_packet.character_count = 0;
	v2_payload.payload.sol_packet.generate_break  = 1;

	intf->send_sol(intf, &v2_payload);
}



/*
 * suspendSelf
 *
 * Put ourself in the background
 *
 * param bRestoreTty specifies whether we will put our self back
 *       in raw mode when we resume
 */
static void
suspendSelf(int bRestoreTty)
{
	leave_raw_mode();
	kill(getpid(), SIGTSTP);

	if (bRestoreTty)
		enter_raw_mode();
}



/*
 * printSolEscapeSequences
 *
 * Send some useful documentation to the user
 */
static void
printSolEscapeSequences(void)
{
	printf(
		   "%c?\r\n\
	Supported escape sequences:\r\n\
	%c.  - terminate connection\r\n\
	%c^Z - suspend ipmitool\r\n\
	%c^X - suspend ipmitool, but don't restore tty on restart\r\n\
	%cB  - send break\r\n\
	%c?  - this message\r\n\
	%c%c  - send the escape character by typing it twice\r\n\
	(Note that escapes are only recognized immediately after newline.)\r\n",
		   SOL_ESCAPE_CHARACTER,
		   SOL_ESCAPE_CHARACTER,
		   SOL_ESCAPE_CHARACTER,
		   SOL_ESCAPE_CHARACTER,
		   SOL_ESCAPE_CHARACTER,
		   SOL_ESCAPE_CHARACTER,
		   SOL_ESCAPE_CHARACTER,
		   SOL_ESCAPE_CHARACTER);
}



/*
 * output
 *
 * Send the specified data to stdout
 */
static void
output(struct ipmi_rs * rsp)
{
	if (rsp)
	{
		int i;
		for (i = 0; i < rsp->data_len; ++i)
			putc(rsp->data[i], stdout);

		fflush(stdout);
	}
}



/*
 * impi_sol_deactivate
 */
static int
ipmi_sol_deactivate(struct ipmi_intf * intf)
{
	struct ipmi_rs * rsp;
	struct ipmi_rq   req;
	uint8_t    data[6];	 

	req.msg.netfn    = IPMI_NETFN_APP;
	req.msg.cmd      = IPMI_DEACTIVATE_PAYLOAD;
	req.msg.data_len = 6;
	req.msg.data     = data;

	bzero(data, sizeof(data));
	data[0] = IPMI_PAYLOAD_TYPE_SOL;  /* payload type              */
	data[1] = 1;                      /* payload instance.  Guess! */

	/* Lots of important data */
	data[2] = 0;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;

	rsp = intf->sendrecv(intf, &req);

	if (NULL != rsp) {
		switch (rsp->ccode) {
			case 0x00: 
				return 0;
			case 0x80:
				lprintf(LOG_ERR, "Info: SOL payload already de-activated");
				break;
			case 0x81:
				lprintf(LOG_ERR, "Info: SOL payload type disabled");
				break;
			default:
				lprintf(LOG_ERR, "Error de-activating SOL payload: %s",
					val2str(rsp->ccode, completion_code_vals));
				break;
		}
	} else {
		lprintf(LOG_ERR, "Error: No response de-activating SOL payload");
	}

	return -1;
}	



/*
 * processSolUserInput
 *
 * Act on user input into the SOL session.  The only reason this
 * is complicated is that we have to process escape sequences.
 *
 * return   0 on success
 *          1 if we should exit
 *        < 0 on error (BMC probably closed the session)
 */
static int
processSolUserInput(
					struct ipmi_intf * intf,
					uint8_t    * input,
					uint16_t     buffer_length)
{
	static int escape_pending = 0;
	static int last_was_cr    = 1;
	struct ipmi_v2_payload v2_payload;
	int  length               = 0;
	int  retval               = 0;
	char ch;
	int  i;

	memset(&v2_payload, 0, sizeof(v2_payload));

	/*
	 * Our first order of business is to check the input for escape
	 * sequences to act on.
	 */
	for (i = 0; i < buffer_length; ++i)
	{
		ch = input[i];

		if (escape_pending){
			escape_pending = 0;
			
			/*
			 * Process a possible escape sequence.
			 */
			switch (ch) {
			case '.':
				printf("%c. [terminated ipmitool]\r\n", SOL_ESCAPE_CHARACTER);
				retval = 1;
				break;
			case 'Z' - 64:
				printf("%c^Z [suspend ipmitool]\r\n", SOL_ESCAPE_CHARACTER);
				suspendSelf(1); /* Restore tty back to raw */
				continue;

			case 'X' - 64:
				printf("%c^Z [suspend ipmitool]\r\n", SOL_ESCAPE_CHARACTER);
				suspendSelf(0); /* Don't restore to raw mode */
				continue;

			case 'B':
				printf("%cb [send break]\r\n", SOL_ESCAPE_CHARACTER);
				sendBreak(intf);
				continue;

			case '?':
				printSolEscapeSequences();
				continue;
			default:
				if (ch != SOL_ESCAPE_CHARACTER)
					v2_payload.payload.sol_packet.data[length++] =
						SOL_ESCAPE_CHARACTER;
				v2_payload.payload.sol_packet.data[length++] = ch;
			}
		}

		else
		{
			if (last_was_cr && (ch == SOL_ESCAPE_CHARACTER)) {
				escape_pending = 1;
				continue;
			}

			v2_payload.payload.sol_packet.data[length++] =	ch;
		}


		/*
		 * Normal character.  Record whether it was a newline.
		 */
		last_was_cr = (ch == '\r' || ch == '\n');
	}

	
	/*
	 * If there is anything left to process we dispatch it to the BMC,
	 * send intf->session->sol_data.max_outbound_payload_size bytes
	 * at a time.
	 */
	if (length)
	{
		struct ipmi_rs * rsp;
		
		v2_payload.payload.sol_packet.character_count = length;
		rsp = intf->send_sol(intf, &v2_payload);

		if (! rsp)
		{
			lprintf(LOG_ERR, "Error sending SOL data");
			retval = -1;
		}

		/* If the sequence number is set we know we have new data */
		else if ((rsp->session.authtype == IPMI_SESSION_AUTHTYPE_RMCP_PLUS) &&
				 (rsp->session.payloadtype == IPMI_PAYLOAD_TYPE_SOL)        &&
		 		 (rsp->payload.sol_packet.packet_sequence_number))
			output(rsp);
	}

	return retval;
}



/*
 * ipmi_sol_red_pill
 */
static int
ipmi_sol_red_pill(struct ipmi_intf * intf)
{
	
	char   * buffer;
	int    numRead;
	int    bShouldExit       = 0;
	int    bBmcClosedSession = 0;
	fd_set read_fds;
	struct timeval tv;
	int    retval;
	int    buffer_size = intf->session->sol_data.max_inbound_payload_size;

	buffer = (char*)malloc(buffer_size);
	if (buffer == NULL) {
		lprintf(LOG_ERR, "ipmitool: malloc failure");
		return -1;
	}

	enter_raw_mode();

	while (! bShouldExit)
	{
		FD_ZERO(&read_fds);
		FD_SET(0, &read_fds);
		FD_SET(intf->fd, &read_fds);

		/* Wait up to half a second */
		tv.tv_sec =  0;
		tv.tv_usec = 500000;

		retval = select(intf->fd + 1, &read_fds, NULL, NULL, &tv);

		if (retval)
		{
			if (retval == -1)
			{
				/* ERROR */
				perror("select");
				return -1;
			}


			/*
			 * Process input from the user
			 */
			if (FD_ISSET(0, &read_fds))
	 		{
				bzero(buffer, sizeof(buffer));
				numRead = read(fileno(stdin),
							   buffer,
							   buffer_size);
				
				if (numRead > 0)
				{
					int rc = processSolUserInput(intf, buffer, numRead);
					
					if (rc)
					{
						if (rc < 0)
							bShouldExit = bBmcClosedSession = 1;
						else
							bShouldExit = 1;
					}
				}
				else
				{
					bShouldExit = 1;
				}
			}


			/*
			 * Process input from the BMC
			 */
			else if (FD_ISSET(intf->fd, &read_fds))
			{
				struct ipmi_rs * rs =intf->recv_sol(intf);
				if (! rs)
				{
					bShouldExit = bBmcClosedSession = 1;
				}
				else
					output(rs);
 			}

			
			/*
			 * ERROR in select
			 */
 			else
			{
				lprintf(LOG_ERR, "Error: Select returned with nothing to read");
				bShouldExit = 1;
			}
		}
	}		

	leave_raw_mode();

	if (bBmcClosedSession)
	{
		lprintf(LOG_ERR, "SOL session closed by BMC");
		exit(1);
	}
	else
		ipmi_sol_deactivate(intf);

	return 0;
}




/*
 * impi_sol_activate
 */
static int
ipmi_sol_activate(struct ipmi_intf * intf)
{
	struct ipmi_rs * rsp;
	struct ipmi_rq   req;
	struct activate_payload_rsp ap_rsp;
	uint8_t    data[6];	 
	uint8_t    bSolEncryption     = 1;
	uint8_t    bSolAuthentication = 1;

	/*
	 * This command is only available over RMCP+ (the lanplus
	 * interface).
	 */
	if (strncmp(intf->name, "lanplus", 7) != 0)
	{
		lprintf(LOG_ERR, "Error: This command is only available over the "
			   "lanplus interface");
		return -1;
	}


	/*
	 * Setup a callback so that the lanplus processing knows what
	 * to do with packets that come unexpectedly (while waiting for
	 * an ACK, perhaps.
	 */
	intf->session->sol_data.sol_input_handler = output;


	req.msg.netfn    = IPMI_NETFN_APP;
	req.msg.cmd      = IPMI_ACTIVATE_PAYLOAD;
	req.msg.data_len = 6;
	req.msg.data     = data;

	data[0] = IPMI_PAYLOAD_TYPE_SOL;  /* payload type     */
	data[1] = 1;                      /* payload instance */

	/* Lots of important data.  Most is default */
	data[2]  = bSolEncryption?     0x80 : 0;
	data[2] |= bSolAuthentication? 0x40 : 0;
	data[2] |= IPMI_SOL_SERIAL_ALERT_MASK_DEFERRED;
	data[2] |= IPMI_SOL_BMC_ASSERTS_CTS_MASK_FALSE;

	data[3] = 0x00; /* reserved */
	data[4] = 0x00; /* reserved */
	data[5] = 0x00; /* reserved */
	
	rsp = intf->sendrecv(intf, &req);

	if (NULL != rsp) {
		switch (rsp->ccode) {
			case 0x00: 
				if (rsp->data_len == 12) {
					break;
				} else {
					lprintf(LOG_ERR, "Error: Unexpected data length (%d) received "
						   "in payload activation response",
						   rsp->data_len);
					return -1;
				}
				break;
			case 0x80:
				lprintf(LOG_ERR, "Info: SOL payload already active on another session");
				return -1;
			case 0x81:
				lprintf(LOG_ERR, "Info: SOL payload disabled");
				return -1;
			case 0x82:
				lprintf(LOG_ERR, "Info: SOL payload activation limit reached");
				return -1;
			case 0x83:
				lprintf(LOG_ERR, "Info: cannot activate SOL payload with encryption");
				return -1;
			case 0x84:
				lprintf(LOG_ERR, "Info: cannot activate SOL payload without encryption");
				return -1;
			default:
				lprintf(LOG_ERR, "Error activating SOL payload: %s",
					val2str(rsp->ccode, completion_code_vals));
				return -1;
		}
	} else {
		lprintf(LOG_ERR, "Error: No response activating SOL payload");
		return -1;
	}


	memcpy(&ap_rsp, rsp->data, sizeof(struct activate_payload_rsp));
	
	intf->session->sol_data.max_inbound_payload_size =
		(ap_rsp.inbound_payload_size[1] << 8) |
		ap_rsp.inbound_payload_size[0];

	intf->session->sol_data.max_outbound_payload_size =
		(ap_rsp.outbound_payload_size[1] << 8) |
		ap_rsp.outbound_payload_size[0];

	intf->session->sol_data.port =
		(ap_rsp.payload_udp_port[1] << 8) |
		ap_rsp.payload_udp_port[0];
	


	#if WORDS_BIGENDIAN
	intf->session->sol_data.max_inbound_payload_size =
		BSWAP_16(intf->session->sol_data.max_inbound_payload_size);
	intf->session->sol_data.max_outbound_payload_size =
		BSWAP_16(intf->session->sol_data.max_outbound_payload_size);
	intf->session->sol_data.port =
		BSWAP_16(intf->session->sol_data.port);
	#endif


	intf->session->timeout = 3;


	if (intf->session->sol_data.port != intf->session->port)
	{
		lprintf(LOG_ERR, "Error: BMC requests SOL session on different port");
		return -1;
	}
	

	printf("[SOL Session operational.  Use %c? for help]\r\n",
		   SOL_ESCAPE_CHARACTER);


	/*
	 * At this point we are good to go with our SOL session.  We
	 * need to listen to
	 * 1) STDIN for user input
	 * 2) The FD for incoming SOL packets
	 */
	if (ipmi_sol_red_pill(intf))
	{
		lprintf(LOG_ERR, "Error in SOL session");
		return -1;
	}

	return 0;
}



/*
 * print_sol_usage
 */
static void
print_sol_usage(void)
{
	lprintf(LOG_NOTICE, "SOL Commands: info [<channel number>]");
	lprintf(LOG_NOTICE, "              set <parameter> <value> [channel]");
	lprintf(LOG_NOTICE, "              activate");
	lprintf(LOG_NOTICE, "              deactivate");
}



/*
 * print_sol_set_usage
 */
static void
print_sol_set_usage(void)
{
	lprintf(LOG_NOTICE, "\nSOL set parameters and values: \n");
  	lprintf(LOG_NOTICE, "  set-in-progress             set-complete | "
		"set-in-progress | commit-write");
	lprintf(LOG_NOTICE, "  enabled                     true | false");
	lprintf(LOG_NOTICE, "  force-encryption            true | false");
	lprintf(LOG_NOTICE, "  force-authentication        true | false");
	lprintf(LOG_NOTICE, "  privilege-level             user | operator | admin | oem");
	lprintf(LOG_NOTICE, "  character-accumulate-level  <in 5 ms increments>");
	lprintf(LOG_NOTICE, "  character-send-threshold    N");
	lprintf(LOG_NOTICE, "  retry-count                 N");
	lprintf(LOG_NOTICE, "  retry-interval              <in 10 ms increments>");
	lprintf(LOG_NOTICE, "  non-volatile-bit-rate       "
		"serial | 9.6 | 19.2 | 38.4 | 57.6 | 115.2");
	lprintf(LOG_NOTICE, "  volatile-bit-rate           "
		"serial | 9.6 | 19.2 | 38.4 | 57.6 | 115.2");
	lprintf(LOG_NOTICE, "");
}



/*
 * ipmi_sol_main
 */
int
ipmi_sol_main(struct ipmi_intf * intf, int argc, char ** argv)
{
	int retval = 0;

	/*
	 * Help
	 */
	if (!argc || !strncmp(argv[0], "help", 4))
		print_sol_usage();

	/*
	 * Info
	 */
 	else if (!strncmp(argv[0], "info", 4)) {
		uint8_t channel;

		if (argc == 1)
			channel = 0x0E; /* Ask about the current channel */
		else if (argc == 2)
			channel = (uint8_t)strtol(argv[1], NULL, 0);
		else
		{
			print_sol_usage();	
			return -1;
		}

		retval = ipmi_print_sol_info(intf, channel);
	}


	/*
	 * Set a parameter value
	 */
	else if (!strncmp(argv[0], "set", 3)) {
		uint8_t channel;

		if (argc == 3)
			channel = 0x0E; /* Ask about the current channel */
		else if (argc == 4)
			channel = (uint8_t)strtol(argv[3], NULL, 0);
		else
		{
			print_sol_set_usage();
			return -1;
		}
			
		retval = ipmi_sol_set_param(intf,
									channel,
									argv[1],
									argv[2]);
	}
	

	/*
	 * Activate
	 */
 	else if (!strncmp(argv[0], "activate", 8))
		retval = ipmi_sol_activate(intf);

	/*
	 * Dectivate
	 */
	else if (!strncmp(argv[0], "deactivate", 10))
		retval = ipmi_sol_deactivate(intf);
	

	else
	{
		print_sol_usage();
		retval = -1;
	}

	return retval;
}
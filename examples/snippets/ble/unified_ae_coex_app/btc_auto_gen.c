/*
 * btc_auto_gen.c
 *
 *  Created on: Apr 3, 2023
 *      Author: sadasgup
 */
#include "generic_command_handling.h"

command_lut_entry_t btc_commands_lut[] = { { .command_name = NULL, .handler = NULL } };

/*extern module_lut_entry_t gap_module_lut[];
extern command_lut_entry_t gap_commands_lut[];
extern module_lut_entry_t hfp_module_lut[];
extern command_lut_entry_t hfp_commands_lut[];
extern module_lut_entry_t a2dp_module_lut[];
extern command_lut_entry_t a2dp_commands_lut[];
extern module_lut_entry_t avrcp_module_lut[];
extern command_lut_entry_t avrcp_commands_lut[];
extern module_lut_entry_t avrcp_target_module_lut[];
extern command_lut_entry_t avrcp_target_commands_lut[];*/
extern module_lut_entry_t ble_module_lut[];
extern command_lut_entry_t ble_commands_lut[];

module_lut_entry_t btc_module_lut[] = {
  { .module_name = "gap", .sub_modules = NULL, .commands = NULL, .custom_handler = NULL },
  { .module_name = "hfp", .sub_modules = NULL, .commands = NULL, .custom_handler = NULL },
  { .module_name = "a2dp", .sub_modules = NULL, .commands = NULL, .custom_handler = NULL },
  { .module_name = "avrcp", .sub_modules = NULL, .commands = NULL, .custom_handler = NULL },
  { .module_name = "ble", .sub_modules = ble_module_lut, .commands = ble_commands_lut, .custom_handler = NULL },
  { .module_name = "avrcp_target", .sub_modules = NULL, .commands = NULL, .custom_handler = NULL },
  { .module_name = NULL, .sub_modules = NULL, .commands = NULL, .custom_handler = NULL }
};

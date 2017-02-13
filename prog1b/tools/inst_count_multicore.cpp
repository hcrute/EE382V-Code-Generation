/*BEGIN_LEGAL
This Software is part of Intel(R) SAE. The rights to copy, distribute,
modify, or otherwise make use of this Software may be licensed only
pursuant to the terms of an applicable Intel license agreement.

Copyright 2010-2016 Intel(R) Corporation
END_LEGAL */

#include <iostream>
#include <map>

#include "ztool-api.h"

using namespace std;

static ztool_handle_t zhandle;
static ztool_config_key_t default_config_key;

static uint64_t *icount = 0;
static unsigned core_count;

void count_inst(ztool_state_handle_t shandle, void *data)
{
	unsigned core_num = ztool_state_get_core_num(shandle);
	icount[core_num]++;
}

void fini(ztool_fini_handle_t fhandle, void *data)
{
	for (unsigned curr_iter = 0; curr_iter < core_count; curr_iter++) {
		cout << "Core " << curr_iter << " total instructions: "
			 << icount[curr_iter] << endl;
	}
}

// Init function. Called by zsim.
extern "C" void ztool_init(ztool_init_handle_t handle)
{
    zhandle = ztool_init_get_tool_handle(handle);
    default_config_key = ztool_init_get_default_config_key(handle);
    core_count = ztool_init_get_core_count(handle);
    
    icount = new uint64_t[core_count]();
    

    {
        ztool_instruction_exe_desc_t desc;

        desc.fn = count_inst;
        desc.when = ZTOOL_INSTRUCTION_WHEN_BEFORE;
        desc.data = NULL;
        desc.order = ZTOOL_CB_ORDER_DEFAULT;
        desc.config_key = default_config_key;
        desc.zhandle = zhandle;

        ztool_instruction_exe_register_cb(&desc);
    }

    {
        ztool_fini_desc_t desc;

        desc.fn = fini;
        desc.data = NULL;
        desc.order = ZTOOL_CB_ORDER_DEFAULT;
        desc.config_key = default_config_key;
        desc.zhandle = zhandle;

        ztool_fini_register_cb(&desc);
    }
}

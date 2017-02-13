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

static ztool_config_key_t in_syscall_key;


struct syscall_icount_t {
	uint64_t calls;
	uint64_t icount_user;
	uint64_t icount_kernel;
	
	syscall_icount_t()
		: calls(0), icount_user(0), icount_kernel(0)
	{}
};

static map<uint64_t, syscall_icount_t *> syscall_icounts;
static uint64_t syscall_number;

void count_inst(ztool_state_handle_t shandle, void *data) {
	// get the cpl from cs register
	ztool_reg_val_t cs_val = ztool_state_seg_get_selector_val(shandle, XED_REG_CS);
	ztool_reg_val_t mask = 3;
	bool user_space = (cs_val & mask) == 3;
	
	if (user_space) {
		syscall_icounts[syscall_number]->icount_user++;
	} else {
		syscall_icounts[syscall_number]->icount_kernel++;
	}
}

void fini(ztool_fini_handle_t fhandle, void *data) {
	
	for (auto const& x : syscall_icounts) {
		cout << "Syscall " << x.first << ":\n";
		cout << "\t Total Calls: " << x.second->calls << endl;
		cout << "\t Total user instructions: " <<
			x.second->icount_user << endl;
		cout << "\t Total kernel instructions: " <<
			x.second->icount_kernel << endl;
	}
}

void count_enable(ztool_config_handle_t handle, void *data)
{
    ztool_config_enable_key(in_syscall_key);
}

void count_disable(ztool_config_handle_t handle, void *data)
{
    ztool_config_disable_key(in_syscall_key);
}

void enter_syscall(ztool_os_staged_event_handle_t evhandle, void *data) {
	 // Get the state handle
    ztool_state_handle_t shandle = ztool_os_get_state_handle(evhandle.oshandle);

    if (!ztool_config_is_key_enabled(in_syscall_key))
    {
        ztool_config_desc_t desc;

        desc.fn = count_enable;
        desc.data = NULL;
        desc.order = ZTOOL_CB_ORDER_DEFAULT;
        desc.shandle = shandle;
        desc.zhandle = zhandle;

        ztool_config_initiate_reconfig_event(&desc);
        //get a handle to the system call
        
        ztool_os_system_call_handle_t system_call_handle =
			ztool_os_staged_event_get_system_call(evhandle);
		ztool_os_system_call_get_number(system_call_handle, &syscall_number);
        
        //how to use the map
        
        //if syscall number is not in the map
        map<uint64_t, syscall_icount_t *>::iterator it;
        it = syscall_icounts.find(syscall_number);
        if (it == syscall_icounts.end()) {
			syscall_icounts[syscall_number] = new syscall_icount_t();
			syscall_icounts[syscall_number]->calls++;
		} else {
			syscall_icounts[syscall_number]->calls++;
		}
    }
}

void exit_syscall(ztool_os_staged_event_handle_t evhandle, void *data) {
	// Get the state handle
    ztool_state_handle_t shandle = ztool_os_get_state_handle(evhandle.oshandle);

    if (ztool_config_is_key_enabled(in_syscall_key))
    {
        ztool_config_desc_t desc;

        desc.fn = count_disable;
        desc.data = NULL;
        desc.order = ZTOOL_CB_ORDER_DEFAULT;
        desc.shandle = shandle;
        desc.zhandle = zhandle;

        ztool_config_initiate_reconfig_event(&desc);
    }
}

// Init function. Called by zsim.
extern "C" void ztool_init(ztool_init_handle_t handle)
{
	zhandle = ztool_init_get_tool_handle(handle);
	default_config_key = ztool_init_get_default_config_key(handle);
	
	//key is disabled by default
	
	ztool_config_alloc_key(zhandle, &in_syscall_key);
	
	{
		ztool_instruction_exe_desc_t desc;
		
		desc.fn = count_inst;
		desc.when = ZTOOL_INSTRUCTION_WHEN_BEFORE;
		desc.data = NULL;
		desc.order = ZTOOL_CB_ORDER_DEFAULT;
		desc.config_key = in_syscall_key;
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
	
	{
		ztool_os_staged_event_desc_t desc;
		
		desc.fn = enter_syscall;
		desc.os_event = ZTOOL_OS_STAGED_EVENT_SYSTEM_CALL;
		desc.when = ZTOOL_OS_WHEN_RECONFIG_BEFORE;
		desc.data = NULL;
		desc.order = ZTOOL_CB_ORDER_DEFAULT;
		desc.config_key = default_config_key;
		desc.zhandle = zhandle;
		
		ztool_os_staged_event_register_cb(&desc);
	}
	
	{
		ztool_os_staged_event_desc_t desc;
		
		desc.fn = exit_syscall;
		desc.os_event = ZTOOL_OS_STAGED_EVENT_SYSTEM_CALL;
		desc.when = ZTOOL_OS_WHEN_RECONFIG_AFTER;
		desc.data = NULL;
		desc.order = ZTOOL_CB_ORDER_DEFAULT;
		desc.config_key = default_config_key;
		desc.zhandle = zhandle;
		
		ztool_os_staged_event_register_cb(&desc);
	}
	
}

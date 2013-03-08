#include "readwrite.h"
#include "config.h"
#include "tracecap.h"
#include "TEMU_main.h"
#include "assert.h"
#include "xed-interface.h"

void get_new_access (
  xed_operand_action_enum_t curr_access,
  xed_operand_action_enum_t *old_access, 
  xed_operand_action_enum_t *new_access) 
{
  switch(curr_access) {
    case XED_OPERAND_ACTION_RW: {
      *old_access = XED_OPERAND_ACTION_R;
      *new_access = XED_OPERAND_ACTION_W;
      break;
    }
    case XED_OPERAND_ACTION_RCW: {
      *old_access = XED_OPERAND_ACTION_R;
      *new_access = XED_OPERAND_ACTION_CW;
      break;
    }
    case XED_OPERAND_ACTION_CRW: {
      *old_access = XED_OPERAND_ACTION_CR;
      *new_access = XED_OPERAND_ACTION_W;
      break;
    }
    default: {
      *old_access = curr_access;
      *new_access = curr_access;
      break;
    }
  }
 
}

// Update all written operands with current value
void update_written_operands (EntryHeader *eh) {
  int i = 0, first_empty_idx = 0;
  xed_operand_action_enum_t old_access, new_access;
  

    // Find number of operands
    while ((eh->operand[i].type != TNone) && (i < MAX_NUM_OPERANDS)) {
      i++;
    }
    first_empty_idx = i;

    // Modify operands
    i = 0;
    while ((eh->operand[i].type != TNone) && (i < MAX_NUM_OPERANDS)) {
      switch(eh->operand[i].access) {
        case XED_OPERAND_ACTION_W:
        case XED_OPERAND_ACTION_CW: {
          // Just update the operand value
	  if (eh->operand[i].type == TRegister) {
            int regnum = get_regnum(eh->operand[i]);
	    eh->operand[i].value = TEMU_cpu_regs[regnum];
	  } 
	  else if (eh->operand[i].type == TMemLoc) {
	    TEMU_read_mem(eh->operand[i].addr,
	      (int)(eh->operand[i].length), 
	      (uint8_t *)&(eh->operand[i].value));
	  }
	  break;
	}
        case XED_OPERAND_ACTION_RW:
        case XED_OPERAND_ACTION_RCW:
        case XED_OPERAND_ACTION_CRW: {
          // Copy operand to empty slot
          assert(first_empty_idx < MAX_NUM_OPERANDS);
          assert(first_empty_idx != i);
          memcpy((void *)&(eh->operand[first_empty_idx]),
	    (void *)&(eh->operand[i]),sizeof(OperandVal)); 

          // Update the number of operands
          eh->num_operands++;

          // Update operand access for both operands
          get_new_access(eh->operand[i].access,&old_access,&new_access);
	  eh->operand[i].access = old_access;
	  eh->operand[first_empty_idx].access = new_access;

          // Update value for new operand
          if (eh->operand[i].type == TRegister) {
            int regnum = get_regnum(eh->operand[i]);
            eh->operand[first_empty_idx].value = TEMU_cpu_regs[regnum];
          }
          else if (eh->operand[i].type == TMemLoc) {
            TEMU_read_mem(eh->operand[i].addr,
              (int)(eh->operand[i].length),
              (uint8_t *)&(eh->operand[first_empty_idx].value));
          }
          first_empty_idx++;
	}
        default: {
          break;
	}
      }

      i++;
    }

}


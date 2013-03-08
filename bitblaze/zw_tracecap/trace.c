#include "config.h"
#include <stdio.h>
#include <sys/user.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>
#include "trace.h"
#include "TEMU_main.h"
#include "read_linux.h"
#include "operandinfo.h"
#include "xed-interface.h"
#include "disasm.h"
#include "tracecap.h"
#include "procmod.h"

int iTracingRdtsc = 0;
int CanTraceRdtsc = 0;
int RdtscOffset = 0;


/* Map to convert register numbers */
int regmapping[] = { -1, -1, -1, -1, -1, -1, -1, -1,
  R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI,
  R_EAX, R_ECX, R_EDX, R_EBX, R_EAX, R_ECX, R_EDX, R_EBX,
  R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI,
  R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI
};

/* Map from XED register numbers to
    0) Christopher's register numbers
    1) Regnum
*/
int xed2chris_regmapping[][2] = {
/* XED_REG_INVALID */ {-1, -1},
/* XED_REG_ERROR */ {-1, -1},
/* XED_REG_RAX */ {-1, -1},
/* XED_REG_EAX */ {eax_reg, R_EAX},
/* XED_REG_AX */ {ax_reg, R_EAX},
/* XED_REG_AH */ {ah_reg, R_EAX},
/* XED_REG_AL */ {al_reg, R_EAX},
/* XED_REG_RCX */ {-1, -1},
/* XED_REG_ECX */ {ecx_reg, R_ECX},
/* XED_REG_CX */ {cx_reg, R_ECX},
/* XED_REG_CH */ {ch_reg, R_ECX},
/* XED_REG_CL */ {cl_reg, R_ECX},
/* XED_REG_RDX */ {-1, -1},
/* XED_REG_EDX */ {edx_reg, R_EDX},
/* XED_REG_DX */ {dx_reg, R_EDX},
/* XED_REG_DH */ {dh_reg, R_EDX},
/* XED_REG_DL */ {dl_reg, R_EDX},
/* XED_REG_RBX */ {-1, -1},
/* XED_REG_EBX */ {ebx_reg, R_EBX},
/* XED_REG_BX */ {bx_reg, R_EBX},
/* XED_REG_BH */ {bh_reg, R_EBX},
/* XED_REG_BL */ {bl_reg, R_EBX},
/* XED_REG_RSP */ {-1, -1},
/* XED_REG_ESP */ {esp_reg, R_ESP},
/* XED_REG_SP */ {sp_reg, R_ESP},
/* XED_REG_SPL */ {-1, -1},
/* XED_REG_RBP */ {-1, -1},
/* XED_REG_EBP */ {ebp_reg, R_EBP},
/* XED_REG_BP */ {bp_reg, R_EBP},
/* XED_REG_BPL */ {-1, -1},
/* XED_REG_RSI */ {-1, -1},
/* XED_REG_ESI */ {esi_reg, R_ESI},
/* XED_REG_SI */ {si_reg, R_ESI},
/* XED_REG_SIL */ {-1, -1},
/* XED_REG_RDI */ {-1, -1},
/* XED_REG_EDI */ {edi_reg, R_EDI},
/* XED_REG_DI */ {di_reg, R_EDI},
/* XED_REG_DIL */ {-1, -1},
/* XED_REG_R8 */ {-1, -1},
/* XED_REG_R8D */ {-1, -1},
/* XED_REG_R8W */ {-1, -1},
/* XED_REG_R8B */ {-1, -1},
/* XED_REG_R9 */ {-1, -1},
/* XED_REG_R9D */ {-1, -1},
/* XED_REG_R9W */ {-1, -1},
/* XED_REG_R9B */ {-1, -1},
/* XED_REG_R10 */ {-1, -1},
/* XED_REG_R10D */ {-1, -1},
/* XED_REG_R10W */ {-1, -1},
/* XED_REG_R10B */ {-1, -1},
/* XED_REG_R11 */ {-1, -1},
/* XED_REG_R11D */ {-1, -1},
/* XED_REG_R11W */ {-1, -1},
/* XED_REG_R11B */ {-1, -1},
/* XED_REG_R12 */ {-1, -1},
/* XED_REG_R12D */ {-1, -1},
/* XED_REG_R12W */ {-1, -1},
/* XED_REG_R12B */ {-1, -1},
/* XED_REG_R13 */ {-1, -1},
/* XED_REG_R13D */ {-1, -1},
/* XED_REG_R13W */ {-1, -1},
/* XED_REG_R13B */ {-1, -1},
/* XED_REG_R14 */ {-1, -1},
/* XED_REG_R14D */ {-1, -1},
/* XED_REG_R14W */ {-1, -1},
/* XED_REG_R14B */ {-1, -1},
/* XED_REG_R15 */ {-1, -1},
/* XED_REG_R15D */ {-1, -1},
/* XED_REG_R15W */ {-1, -1},
/* XED_REG_R15B */ {-1, -1},
/* XED_REG_RIP */ {-1, -1},
/* XED_REG_EIP */ {-1, -1},
/* XED_REG_IP */ {-1, -1},
/* XED_REG_FLAGS */ {-1, -1},
/* XED_REG_EFLAGS */ {-1, -1},
/* XED_REG_RFLAGS */ {-1, -1},
/* XED_REG_CS */ {cs_reg, -1},
/* XED_REG_DS */ {ds_reg, -1},
/* XED_REG_ES */ {es_reg, -1},
/* XED_REG_SS */ {ss_reg, -1},
/* XED_REG_FS */ {fs_reg, -1},
/* XED_REG_GS */ {gs_reg, -1},
/* XED_REG_XMM0 */ {-1, -1},
/* XED_REG_XMM1 */ {-1, -1},
/* XED_REG_XMM2 */ {-1, -1},
/* XED_REG_XMM3 */ {-1, -1},
/* XED_REG_XMM4 */ {-1, -1},
/* XED_REG_XMM5 */ {-1, -1},
/* XED_REG_XMM6 */ {-1, -1},
/* XED_REG_XMM7 */ {-1, -1},
/* XED_REG_XMM8 */ {-1, -1},
/* XED_REG_XMM9 */ {-1, -1},
/* XED_REG_XMM10 */ {-1, -1},
/* XED_REG_XMM11 */ {-1, -1},
/* XED_REG_XMM12 */ {-1, -1},
/* XED_REG_XMM13 */ {-1, -1},
/* XED_REG_XMM14 */ {-1, -1},
/* XED_REG_XMM15 */ {-1, -1},
/* XED_REG_MMX0 */ {-1, -1},
/* XED_REG_MMX1 */ {-1, -1},
/* XED_REG_MMX2 */ {-1, -1},
/* XED_REG_MMX3 */ {-1, -1},
/* XED_REG_MMX4 */ {-1, -1},
/* XED_REG_MMX5 */ {-1, -1},
/* XED_REG_MMX6 */ {-1, -1},
/* XED_REG_MMX7 */ {-1, -1},
/* XED_REG_ST0 */ {-1, -1},
/* XED_REG_ST1 */ {-1, -1},
/* XED_REG_ST2 */ {-1, -1},
/* XED_REG_ST3 */ {-1, -1},
/* XED_REG_ST4 */ {-1, -1},
/* XED_REG_ST5 */ {-1, -1},
/* XED_REG_ST6 */ {-1, -1},
/* XED_REG_ST7 */ {-1, -1},
/* XED_REG_CR0 */ {-1, -1},
/* XED_REG_CR1 */ {-1, -1},
/* XED_REG_CR2 */ {-1, -1},
/* XED_REG_CR3 */ {-1, -1},
/* XED_REG_CR4 */ {-1, -1},
/* XED_REG_CR5 */ {-1, -1},
/* XED_REG_CR6 */ {-1, -1},
/* XED_REG_CR7 */ {-1, -1},
/* XED_REG_CR8 */ {-1, -1},
/* XED_REG_CR9 */ {-1, -1},
/* XED_REG_CR10 */ {-1, -1},
/* XED_REG_CR11 */ {-1, -1},
/* XED_REG_CR12 */ {-1, -1},
/* XED_REG_CR13 */ {-1, -1},
/* XED_REG_CR14 */ {-1, -1},
/* XED_REG_CR15 */ {-1, -1},
/* XED_REG_DR0 */ {-1, -1},
/* XED_REG_DR1 */ {-1, -1},
/* XED_REG_DR2 */ {-1, -1},
/* XED_REG_DR3 */ {-1, -1},
/* XED_REG_DR4 */ {-1, -1},
/* XED_REG_DR5 */ {-1, -1},
/* XED_REG_DR6 */ {-1, -1},
/* XED_REG_DR7 */ {-1, -1},
/* XED_REG_DR8 */ {-1, -1},
/* XED_REG_DR9 */ {-1, -1},
/* XED_REG_DR10 */ {-1, -1},
/* XED_REG_DR11 */ {-1, -1},
/* XED_REG_DR12 */ {-1, -1},
/* XED_REG_DR13 */ {-1, -1},
/* XED_REG_DR14 */ {-1, -1},
/* XED_REG_DR15 */ {-1, -1},
/* XED_REG_ONE */ {-1, -1},
/* XED_REG_STACKPUSH */ {-1, -1},
/* XED_REG_STACKPOP */ {-1, -1},
/* XED_REG_GDTR */ {-1, -1},
/* XED_REG_LDTR */ {-1, -1},
/* XED_REG_IDTR */ {-1, -1},
/* XED_REG_TR */ {-1, -1},
/* XED_REG_TSC */ {-1, -1},
/* XED_REG_TSCAUX */ {-1, -1},
/* XED_REG_MSRS */ {-1, -1},
/* XED_REG_X87CONTROL */ {-1, -1},
/* XED_REG_X87STATUS */ {-1, -1},
/* XED_REG_X87TOP */ {-1, -1},
/* XED_REG_X87TAG */ {-1, -1},
/* XED_REG_X87PUSH */ {-1, -1},
/* XED_REG_X87POP */ {-1, -1},
/* XED_REG_X87POP2 */ {-1, -1},
/* XED_REG_MXCSR */ {-1, -1},
/* XED_REG_TMP0 */ {-1, -1},
/* XED_REG_TMP1 */ {-1, -1},
/* XED_REG_TMP2 */ {-1, -1},
/* XED_REG_TMP3 */ {-1, -1},
/* XED_REG_TMP4 */ {-1, -1},
/* XED_REG_TMP5 */ {-1, -1},
/* XED_REG_TMP6 */ {-1, -1},
/* XED_REG_TMP7 */ {-1, -1},
/* XED_REG_TMP8 */ {-1, -1},
/* XED_REG_TMP9 */ {-1, -1},
/* XED_REG_TMP10 */ {-1, -1},
/* XED_REG_TMP11 */ {-1, -1},
/* XED_REG_TMP12 */ {-1, -1},
/* XED_REG_TMP13 */ {-1, -1},
/* XED_REG_TMP14 */ {-1, -1},
/* XED_REG_TMP15 */ {-1, -1},
/* XED_REG_LAST */ {-1, -1},
};

/* Buffer to store instructions */
char cfilebuf[FILEBUFSIZE]; //very inmportant
char tfilebuf[FILEBUFSIZE]; // liulu 

/* Variables for statistics */
long insn_counter_decoded = 0; // Number of instructions decoded
long insn_counter_traced = 0; // Number of instructions written to trace
long insn_counter_traced_tainted = 0; // Number of tainted instructions written to trace
long operand_counter = 0;

/* This flags we might want to put as part of the EntryHeader without 
 * writing to file */
int has_page_fault = 0;
int access_user_mem = 0;
int insn_already_written = 0;

/* Variables to keep disassembler state */
xed_state_t dstate;
xed_decoded_inst_t xedd;

/* Flag to signal that no writing should be done */
int trace_do_not_write = 0;

/* XED2 initialization */
void xed2_init() {
  xed_tables_init ();
  xed_state_zero (&dstate);

  xed_state_init(&dstate,
    XED_MACHINE_MODE_LEGACY_32,
    XED_ADDRESS_WIDTH_32b,
    XED_ADDRESS_WIDTH_32b);

}

/* Get register number */
inline int get_regnum(OperandVal op) {
  if (op.type != TRegister) return -1;
  return regmapping[op.addr - 100];
}

/* Print the statistics variables */
void print_stats() {
  term_printf("Number of instructions decoded: %ld\n", insn_counter_decoded);
  term_printf("Number of operands decoded: %ld\n", operand_counter);
  term_printf("Number of instructions written to trace: %ld\n",
    insn_counter_traced);
  term_printf("Number of tainted instructions written to trace: %ld\n",
    insn_counter_traced_tainted);
}


/* Return the offset of the operand. Zero except for AH,BH,CH,DH that is one */
inline int getOperandOffset (OperandVal *op) {
  if ((op->type == TRegister) && ((op->addr >= ah_reg) && (op->addr <= bh_reg)))

    return 1;

  return 0;
}

/* This is the central function
  Given a memory address, reads a bunch of memory bytes and
    calls the disassembler to obtain the information
  Then it stores the information into the eh EntryHeader
*/
void decode_address(uint32_t address, EntryHeader *eh, int ignore_taint)
{
  unsigned char insn_buf[MAX_INSN_BYTES];
  unsigned int is_push = 0;

  /* Read memory from TEMU */
  TEMU_read_mem(address, MAX_INSN_BYTES, insn_buf);

  /* Disassemble instruction buffer */
  xed_decoded_inst_zero_set_mode(&xedd, &dstate);
  xed_error_enum_t xed_error =
    xed_decode(&xedd, STATIC_CAST(const xed_uint8_t*,insn_buf), MAX_INSN_BYTES);
  xed_bool_t okay = (xed_error == XED_ERROR_NONE);
  if (!okay) return;

  // Increase counters
  insn_counter_decoded++;

  int i;

  /* Clear out Entry header */
  memset(eh, 0, sizeof(EntryHeader));

  /* Copy the address and instruction size */
  eh->address = address;
  eh->inst_size = xed_decoded_inst_get_length(&xedd);
  if (eh->inst_size > MAX_INSN_BYTES) eh->inst_size = MAX_INSN_BYTES;

  /* Copy instruction rawbytes */
  memcpy(eh->rawbytes, insn_buf, eh->inst_size);
  
  /* is this insn rdtsc */
  if(iTracingRdtsc){
    if(eh->inst_size == 2){
      if(memcmp(eh->rawbytes, "\x0f\x31", 2) == 0){
        CanTraceRdtsc=1;
      }
    }
  }

  /* Get the number of XED operands */
  const xed_inst_t* xi = xed_decoded_inst_inst(&xedd);
  int xed_ops = xed_inst_noperands(xi);
  int op_idx = -1;
  
  if(xed_ops >= MAX_NUM_OPERANDS){
    term_printf("more OPERAND on 0x%0x\n", address);
  }
  /* Iterate over the XED operands */
  for(i = 0; i < xed_ops; i++) {
    assert(op_idx < MAX_NUM_OPERANDS);

    /* Get operand */
    const xed_operand_t* op = xed_inst_operand(xi,i);
    xed_operand_enum_t op_name = xed_operand_name(op);

    switch(op_name) {
      /* Register */
      case XED_OPERAND_REG0:
      case XED_OPERAND_REG1:
      case XED_OPERAND_REG2:
      case XED_OPERAND_REG3:
      case XED_OPERAND_REG4:
      case XED_OPERAND_REG5:
      case XED_OPERAND_REG6:
      case XED_OPERAND_REG7:
      case XED_OPERAND_REG8:
      case XED_OPERAND_REG9:
      case XED_OPERAND_REG10:
      case XED_OPERAND_REG11:
      case XED_OPERAND_REG12:
      case XED_OPERAND_REG13:
      case XED_OPERAND_REG14:
      case XED_OPERAND_REG15: {
        xed_reg_enum_t reg_id = xed_decoded_inst_get_reg(&xedd, op_name);
        int regnum = xed2chris_regmapping[reg_id][1];

        // Special handling for Push
        if (reg_id == XED_REG_STACKPUSH) is_push = 1;

        if (-1 == regnum)
          break;
        else {
	  op_idx++;
          eh->num_operands++;
          eh->operand[op_idx].type = TRegister;
          eh->operand[op_idx].addr = xed2chris_regmapping[reg_id][0];
	  eh->operand[op_idx].length = 
	    (uint8_t) xed_decoded_inst_operand_length (&xedd, i);
	  eh->operand[op_idx].access = (uint8_t) xed_operand_rw (op);
          eh->operand[op_idx].value = TEMU_cpu_regs[regnum];
          switch (eh->operand[op_idx].addr) {
            case ax_reg:
            case bx_reg:
            case cx_reg:
            case dx_reg:
            case bp_reg:
            case sp_reg:
            case si_reg:
            case di_reg:
              eh->operand[op_idx].value &= 0xFFFF;
              break;
            case al_reg:
            case bl_reg:
            case cl_reg:
            case dl_reg:
              eh->operand[op_idx].value &= 0xFF;
              break;
            case ah_reg:
            case bh_reg:
            case ch_reg:
            case dh_reg:
              eh->operand[op_idx].value = (eh->operand[i].value & 0xFF00) >> 8;
              break;
            default:
              break;
          }
        }
        if (ignore_taint == 0) set_operand_data(&(eh->operand[op_idx]));
        break;
      }

      /* Immediate */
      case XED_OPERAND_IMM0: {
        op_idx++;
        eh->num_operands++;
        eh->operand[op_idx].type = TImmediate;
	eh->operand[op_idx].length = 
	  (uint8_t) xed_decoded_inst_operand_length (&xedd, i);
	eh->operand[op_idx].access = (uint8_t) xed_operand_rw (op);
        //xed_uint_t width = xed_decoded_inst_get_immediate_width(x&edd);
        if (xed_decoded_inst_get_immediate_is_signed(&xedd)) {
          xed_int32_t signed_imm_val = 
	    xed_decoded_inst_get_signed_immediate(&xedd);
          eh->operand[op_idx].value = (uint32_t) signed_imm_val;
        }
        else {
          xed_uint64_t unsigned_imm_val =
            xed_decoded_inst_get_unsigned_immediate(&xedd);
          eh->operand[op_idx].value = (uint32_t) unsigned_imm_val;
        }
        break;
      break;
      }
      /* Special immediate only used in ENTER instruction */
      case XED_OPERAND_IMM1: {
        op_idx++;
        eh->num_operands++;
        eh->operand[op_idx].type = TImmediate;
	eh->operand[op_idx].length = 
	  (uint8_t) xed_decoded_inst_operand_length (&xedd, i);
	eh->operand[op_idx].access = (uint8_t) xed_operand_rw (op);
        xed_uint8_t unsigned_imm_val = 
	  xed_decoded_inst_get_second_immediate(&xedd);
        eh->operand[op_idx].value = (uint32_t) unsigned_imm_val;
        break;
      }


      /* Memory */
      case XED_OPERAND_AGEN:
      case XED_OPERAND_MEM0:
      case XED_OPERAND_MEM1: {
	unsigned long base = 0;
	unsigned long index = 0;
	unsigned long scale = 1;
	unsigned long segbase = 0;
	unsigned short segsel = 0;
	unsigned long displacement = 0;
	unsigned int j;
	size_t remaining = 0;

	/* Set memory index */
        int mem_idx = 0;
        if (op_name == XED_OPERAND_MEM1) mem_idx = 1;

	unsigned int memlen = xed_decoded_inst_operand_length (&xedd, i);

	for (j = 0; j < memlen; j+=4) {
	  /* Initialization */
	  base = 0;
	  index = 0;
	  scale = 1;
	  segbase = 0;
	  segsel = 0;
	  displacement = 0;
	  remaining = memlen - j;

	  op_idx++;
          if(!(op_idx < MAX_NUM_OPERANDS)){
            term_printf("more OPERANDS(xed_ops=%d,op_idx=%d,i=%d,num_operands=%d) on 0x%0x\n",xed_ops,op_idx,i,eh->num_operands , address);
            int x = 0;
            for(;x<eh->inst_size;x++){
              term_printf("%0x ", *(eh->rawbytes+x));
            }
            term_printf("\n");
          }
	  assert(op_idx < MAX_NUM_OPERANDS);
	  eh->num_operands++;
	  eh->operand[op_idx].type = TMemLoc;
	  eh->operand[op_idx].access = (uint8_t) xed_operand_rw (op);
	  eh->operand[op_idx].length = 
	    remaining > 4 ? 4 : (uint8_t) remaining;

	  // Get Segment register
	  xed_reg_enum_t seg_regid = 
	    xed_decoded_inst_get_seg_reg(&xedd,mem_idx);

	  if (seg_regid != XED_REG_INVALID) {
	    const xed_operand_values_t *xopv = 
	      xed_decoded_inst_operands_const(&xedd);
	    xed_bool_t default_segment = 
	      xed_operand_values_using_default_segment (xopv,mem_idx);

	    if (!default_segment) {
	      eh->num_operands++;
	      int segmentreg = xed2chris_regmapping[seg_regid][0] - 100;

	      segbase = TEMU_cpu_segs[segmentreg].base;
	      segsel = TEMU_cpu_segs[segmentreg].selector;

	      eh->memregs[op_idx][0].type = TRegister;
	      eh->memregs[op_idx][0].length = 4;
	      eh->memregs[op_idx][0].addr = xed2chris_regmapping[seg_regid][0];
	      eh->memregs[op_idx][0].access = (uint8_t) XED_OPERAND_ACTION_R;
	      eh->memregs[op_idx][0].value = segsel;
	      eh->memregs[op_idx][0].usage = memsegment;
	      if (ignore_taint == 0) 
		set_operand_data(&(eh->memregs[op_idx][0]));

	      int dt;
	      if (segsel & 0x4)       // ldt
		dt = TEMU_cpu_ldt->base;
	      else                    //gdt
		dt = TEMU_cpu_gdt->base;
	      segsel = segsel >> 3;

	      unsigned long segent = dt + 8 * segsel;
	      unsigned char segdes[8];
	      TEMU_read_mem(segent, 8, segdes);

#if 0
	      // debugging code to double check segbase value
	      unsigned long segbasenew = segdes[2] + segdes[3] * 256 +
	      segdes[4] * 256 * 256 + segdes[7] * 256 * 256 * 256;
	      if (segbase != segbasenew) {
		term_printf("segbase unexpected: 0x%08lX v.s 0x%08lX\n",
			segbase, segbasenew);
	      }
#endif
	      /* Segment descriptor is stored as a memory operand */
	      eh->num_operands+=2;
	      eh->memregs[op_idx][3].type = TMemLoc;
	      eh->memregs[op_idx][3].length = 4;
	      eh->memregs[op_idx][3].addr = segent;
	      eh->memregs[op_idx][3].access = 
		(uint8_t) XED_OPERAND_ACTION_INVALID;
	      eh->memregs[op_idx][3].value = *(uint32_t *) segdes;
	      eh->memregs[op_idx][3].tainted = 0;
	      eh->memregs[op_idx][3].usage = memsegent0;

	      eh->memregs[op_idx][4].type = TMemLoc;
	      eh->memregs[op_idx][4].length = 4;
	      eh->memregs[op_idx][4].addr = segent + 4;
	      eh->memregs[op_idx][4].access = 
		(uint8_t) XED_OPERAND_ACTION_INVALID;
	      eh->memregs[op_idx][4].value = *(uint32_t *) (segdes + 4);
	      eh->memregs[op_idx][4].tainted = 0;
	      eh->memregs[op_idx][4].usage = memsegent1;
	    }
	  }

	  // Get Base register
	  xed_reg_enum_t base_regid = 
	    xed_decoded_inst_get_base_reg(&xedd,mem_idx);
	  if (base_regid != XED_REG_INVALID) {
	    eh->num_operands++;
	    int basereg = xed2chris_regmapping[base_regid][1];
	    base = TEMU_cpu_regs[basereg];
	    eh->memregs[op_idx][1].type = TRegister;
	    eh->memregs[op_idx][1].addr = xed2chris_regmapping[base_regid][0];
	    eh->memregs[op_idx][1].length = 4;
	    eh->memregs[op_idx][1].access = (uint8_t) XED_OPERAND_ACTION_R;
	    eh->memregs[op_idx][1].value = base;
	    eh->memregs[op_idx][1].usage = membase;
	    if (ignore_taint == 0) set_operand_data(&(eh->memregs[op_idx][1]));
	  }
	  // Get Index register and Scale
	  xed_reg_enum_t index_regid = 
	    xed_decoded_inst_get_index_reg(&xedd,mem_idx);
	  if (mem_idx == 0 && index_regid != XED_REG_INVALID) {
	    eh->num_operands++;
	    int indexreg = xed2chris_regmapping[index_regid][1];
	    index = TEMU_cpu_regs[indexreg];
	    eh->memregs[op_idx][2].type = TRegister;
	    eh->memregs[op_idx][2].addr = xed2chris_regmapping[index_regid][0];
	    eh->memregs[op_idx][2].length = 4;
	    eh->memregs[op_idx][2].access = (uint8_t) XED_OPERAND_ACTION_R;
	    eh->memregs[op_idx][2].value = index;
	    eh->memregs[op_idx][2].usage = memindex;
	    if (ignore_taint == 0) set_operand_data(&(eh->memregs[op_idx][2]));

	    // Get Scale (AKA width) (only have a scale if the index exists)
	    if (xed_decoded_inst_get_scale(&xedd,i) != 0) {
	      scale = (unsigned long) xed_decoded_inst_get_scale(&xedd,mem_idx);
	    }
	  }
	  // Get displacement (AKA offset)
	  displacement = 
	    (unsigned long) xed_decoded_inst_get_memory_displacement
	    (&xedd,mem_idx);
	  if (is_push) {
	    displacement = displacement - eh->operand[op_idx].length;
	    is_push = 0;
	  }

	  // Calculate memory address accessed
	  eh->operand[op_idx].addr =
	    j + segbase + base + index * scale + displacement;

	  // Special handling for LEA instructions
	  if (op_name == XED_OPERAND_AGEN) {
	    eh->operand[op_idx].type = TMemAddress;
	    eh->operand[op_idx].length = 4;
	    has_page_fault = 0; // LEA won't trigger page fault
	  }
	  else {
	    has_page_fault = TEMU_read_mem(eh->operand[op_idx].addr,
	      (int)(eh->operand[op_idx].length), 
	      (uint8_t *)&(eh->operand[op_idx].value));
	  }

	  // Check if instruction accesses user memory
	  // kernel_mem_start defined in shared/read_linux.c
	  if ((eh->operand[op_idx].addr < kernel_mem_start) &&
	    (op_name != XED_OPERAND_AGEN))
	  {
	    access_user_mem = 1;
	  }
	  if (ignore_taint == 0) set_operand_data(&(eh->operand[op_idx]));
	}
	break;
      }

      /* Jumps */
      case XED_OPERAND_PTR:  // pointer (always in conjunction with a IMM0)
      case XED_OPERAND_RELBR: { // branch displacements
          xed_uint_t disp = xed_decoded_inst_get_branch_displacement(&xedd);
          if (disp) {
	    /* Displacement is from instruction end */
	    /* Adjust displacement with instruction size */
            disp = disp + eh->inst_size;
            op_idx++;
            eh->num_operands++;
            eh->operand[op_idx].type = TJump;
            eh->operand[op_idx].length = 4;
	    eh->operand[op_idx].access = (uint8_t) xed_operand_rw (op);
            eh->operand[op_idx].value = disp;
          }
          break;
      }

      /* Floating point registers */
      case XED_REG_X87CONTROL:
      case XED_REG_X87STATUS:
      case XED_REG_X87TOP:
      case XED_REG_X87TAG:
      case XED_REG_X87PUSH:
      case XED_REG_X87POP:
      case XED_REG_X87POP2:
          op_idx++;
          eh->num_operands++;
          eh->operand[op_idx].type = TFloatRegister;
          eh->operand[op_idx].length = 4;
	  eh->operand[op_idx].access = (uint8_t) xed_operand_rw (op);
      default:
        break;
     }
  }

  /* Increment the operand counter without including ESP */
  operand_counter += eh->num_operands;

  /* Remainig fields in EntryHeader */
  eh->eflags = 0; /* Gets updated at insn_end */
  eh->df = 0; /* Gets updated at insn_end */

  eh->cc_op = *TEMU_cc_op;

//  free(inst);

}

#ifdef INSN_INFO
long savedeip;
#endif

/* Output function
   Writes an operand structure to the given file
*/
unsigned int write_operand(FILE *stream, OperandVal op) {
  unsigned int i = 0;
  unsigned int num_elems_written = 0;

  if (stream == NULL) return 0;

  /* Write fixed part of operand */
  num_elems_written += fwrite(&op, OPERAND_VAL_FIXED_SIZE, 1, stream);

  /* Write enums */
  uint16_t enums = (((uint16_t) op.usage) << 8) | ((uint16_t) op.type);
  num_elems_written += fwrite(&enums, OPERAND_VAL_ENUMS_REAL_SIZE, 1, stream);

  /* For each byte in the operand, check if tainted.
      If tainted, write taint record */
  assert(op.length <= MAX_OPERAND_LEN);
  for (i = 0; i < op.length; i++) {
    if (op.tainted & (1 << i)) {
      /* Write fixed part of taint_record */
      num_elems_written += 
	fwrite(&(op.records[i]), TAINT_RECORD_FIXED_SIZE, 1, stream);

      /* Write only the non-empty taint_byte_record */
      //term_printf("num of Records: %d\n", op.records[i].numRecords);
      assert(op.records[i].numRecords <= MAX_NUM_TAINTBYTE_RECORDS);
      num_elems_written += fwrite(&(op.records[i].taintBytes), 
	sizeof(TaintByteRecord), op.records[i].numRecords, stream);
    }
  }
  return num_elems_written;
}

/* Output function
   Writes an EntryHeader to the given file
*/
unsigned int write_trace(FILE *stream, EntryHeader *eh)
{
  static int header_already_written = 0;
  unsigned int num_elems_written = 0;

  /* If trace_do_not_write is set, ignore write */
  if (trace_do_not_write) return 0;

  /* If no stream or no instruction, ignore write */
  if ((stream == NULL) || (eh == NULL)) return 0;

  /* If trace header still not written, write it 
   * Delaying writing the header till here allows to get more module
   * information when tracing a process by name */
  if (header_already_written == 0) {
    /* writing the trace header */
    TraceHeader th;
    th.magicnumber = MAGIC_NUMBER;
    th.version = VERSION_NUMBER;
    th.n_procs = 1;
    th.gdt_base = TEMU_cpu_gdt->base;
    th.idt_base = TEMU_cpu_idt->base;
    num_elems_written += fwrite(&th, sizeof(th), 1, stream);

    /* Set flag */
    header_already_written = 1;

    /* for each process */
    ProcRecord pr;
    memset(&pr,0,sizeof(ProcRecord));
    pr.n_mods = find_process(tracecr3, pr.name, &pr.pid);

    old_modinfo_t *pmr = 
      (old_modinfo_t *) malloc(pr.n_mods * sizeof(old_modinfo_t));
    if (pmr) {
      get_proc_modules(pr.pid, pmr, pr.n_mods);
    }
    else {
      pr.n_mods = -1;
    }
    pr.ldt_base = TEMU_cpu_ldt->base;

    num_elems_written += fwrite(&pr, sizeof(pr), 1, stream);

    if (pmr) {
      int i;
      ModuleRecord mr;
      for (i = 0; i < pr.n_mods; i++) {
        strncpy(mr.name, pmr[i].name, MAX_STRING_LEN);
        mr.base = pmr[i].base;
        mr.size = pmr[i].size;
        num_elems_written += fwrite(&mr, sizeof(mr), 1, stream);
      }
      free(pmr);
    }



    fflush(tracelog);
  }

  if (stream && (eh->inst_size > 0)) {
    //term_printf("address: 0x%0x\n", eh->address);
    /* Write fixed part of entry header */
    num_elems_written += fwrite(eh, ENTRY_HEADER_FIXED_SIZE, 1, stream);

    /* Write rawbytes */
    num_elems_written += fwrite(&(eh->rawbytes), eh->inst_size, 1, stream);

    /* Write remaining operands */
    int i = 0,j = 0;
    while ((eh->operand[i].type != TNone) && (i < MAX_NUM_OPERANDS)) {
      write_operand(stream, eh->operand[i]);

      /* For Memory operands, need to write memregs and segent's */
      if ((eh->operand[i].type == TMemLoc) ||
	(eh->operand[i].type == TMemAddress))
      {
	/* Write Memregs operands */
	for (j = 0; j < MAX_NUM_MEMREGS; j++) {
	  if (eh->memregs[i][j].type != TNone) {
	    write_operand(stream, eh->memregs[i][j]);
	  }
	}
      }
      i++;
    }

    insn_already_written = 1;
    insn_counter_traced++;
    if (insn_tainted) insn_counter_traced_tainted++;
    /* Avoid flushing to improve performance */
    //fflush(stream);

#ifdef INSN_INFO
    char name[MAX_STRING_LEN];
    uint32_t offset;
    char insninfostr[256];
    insninfostr[0] = 0;
    checkcr3(temu_plugin->monitored_cr3, savedeip, tracepid, name, 
      MAX_STRING_LEN, &offset);
    snprintf(insninfostr, sizeof(insninfostr),
      "EIP = 0x%08lx, Module = %s, Offset = %ud", savedeip, name, offset);
    fprintf(infolog, "%s\n", insninfostr);
#endif
  }

  return num_elems_written;
}

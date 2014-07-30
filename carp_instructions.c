#include "carp_machine.h"

definstr (HALT) {
  //printf("halting %d...\n", m->code[m->regs[CARP_EIP]]);
  carp_vm_exit(m, m->code[++m->regs[CARP_EIP]]);
}

definstr (NOP) {
  ;
}

definstr (LOADI) {
  long long reg = m->code[++m->regs[CARP_EIP]];
  long long val = m->code[++m->regs[CARP_EIP]];
  m->regs[reg] = val;
}

definstr (MOV) {
  long long dst = m->code[++m->regs[CARP_EIP]],
    src = m->code[++m->regs[CARP_EIP]];
  m->regs[dst] = m->regs[src];
}

definstr (ADDI) {
  long long b = carp_stack_pop(&m->stack),
    a = carp_stack_pop(&m->stack);
  carp_stack_push(&m->stack, a + b);
}

definstr (SUBI) {
  long long b = carp_stack_pop(&m->stack),
    a = carp_stack_pop(&m->stack);
  carp_stack_push(&m->stack, a - b);
}

definstr (MULI) {
  long long b = carp_stack_pop(&m->stack),
    a = carp_stack_pop(&m->stack);
  carp_stack_push(&m->stack, a * b);
}

definstr (INCR) {
  long long a = carp_stack_pop(&m->stack);
  carp_stack_push(&m->stack, a + 1);
}

definstr (DECR) {
  long long a = carp_stack_pop(&m->stack);
  carp_stack_push(&m->stack, a - 1);
}

definstr (PUSHR) {
  long long reg = m->code[++m->regs[CARP_EIP]];
  long long a = m->regs[reg];
  int status = carp_stack_push(&m->stack, a);
  if (status == -1) {
    fprintf(stderr, CARP_STACK_NO_MEM);
    carp_vm_exit(m, 1);
  }
}

definstr (PUSHI) {
  long long a = m->code[++m->regs[CARP_EIP]];
  int status = carp_stack_push(&m->stack, a);
  if (status == -1) {
    fprintf(stderr, CARP_STACK_NO_MEM);
    carp_vm_exit(m, 1);
  }
}

definstr (POPI) {
  long long status = carp_stack_pop(&m->stack);
  if ((void *) status == NULL) {
    fprintf(stderr, CARP_STACK_EMPTY);
    carp_vm_exit(m, 1);
  }

  long long val = status;
  m->regs[CARP_GBG] = val;
}

definstr (CMP) {
  carp_instr_POPI(m);
  long long b = m->regs[CARP_GBG];
  carp_instr_POPI(m);
  long long a = m->regs[CARP_GBG];

  m->regs[CARP_EAX] = a - b;
}

definstr (JZ) {
  // zero
  if (!m->regs[CARP_EAX])
    m->regs[CARP_EIP] = m->code[++m->regs[CARP_EIP]];
}

definstr (RJZ) {
  // zero
  if (!m->regs[CARP_EAX])
    m->regs[CARP_EIP] += m->code[m->regs[CARP_EIP] + 1];
}

definstr (JNZ) {
  // not zero
  if (m->regs[CARP_EAX])
    m->regs[CARP_EIP] = m->code[++m->regs[CARP_EIP]];
}

definstr (RJNZ) {
  // not zero
  if (m->regs[CARP_EAX])
    m->regs[CARP_EIP] += m->code[m->regs[CARP_EIP] + 1];
}

definstr (JMP) {
  m->regs[CARP_EIP] = m->code[++m->regs[CARP_EIP]];
}

definstr (RJMP) {
  m->regs[CARP_EIP] += m->code[m->regs[CARP_EIP] + 1];
}

definstr (DBS) {
  char *key = (char *) m->code[++m->regs[CARP_EIP]];
  long long val = m->code[++m->regs[CARP_EIP]];
  carp_ht *res = carp_ht_set(&m->vars, key, val);
  if (res == NULL) {
    fprintf(stderr, CARP_HT_NO_MEM);
    carp_vm_exit(m, 1);
  }
}

definstr (DBG) {
  char *key = (char *) m->code[++m->regs[CARP_EIP]];
  carp_ht *res = carp_ht_get(&m->vars, key);
  if (res == NULL) {
    fprintf(stderr, CARP_HT_DNE);
    carp_vm_exit(m, 1);
  }

  m->regs[CARP_EAX] = res->value;
}


definstr (LBL) {
  char *key = (char *) m->code[++m->regs[CARP_EIP]];
  long long val = ++m->regs[CARP_EIP];
  carp_ht *res = carp_ht_set(&m->labels, key, val);
  if (res == NULL) {
    fprintf(stderr, CARP_HT_NO_MEM);
    carp_vm_exit(m, 1);
  }
}

definstr (PREG) {
  int reg = m->code[++m->regs[CARP_EIP]];
  printf("%lld\n", m->regs[reg]);
}

definstr (PTOP) {
  long long status = carp_stack_peek(&m->stack);
  if ((void *) status == NULL) {
    fprintf(stderr, CARP_STACK_EMPTY);
    carp_vm_exit(m, 1);
  }

  long long val = status;
  printf("%lld\n", val);
}
#include "regs.h"
#include "regs_description.h"
#include "mirror_storage.h"
#include <string.h>

regs_main_t         regs_main;
main_vars_t * const regs_global       = &regs_main.regs_global;
semaphore_handle_t  regs_access_mutex = NULL;
queue_handle_t      regs_event_queue  = NULL;

u8 regs_size_in_byte(regs_flag_t type) {
   switch (type) {
   case U8_REGS_FLAG:
   case I8_REGS_FLAG:
      return 1;
   case U16_REGS_FLAG:
   case S16_REGS_FLAG:
      return 2;
   case U32_REGS_FLAG:
   case S32_REGS_FLAG:
   case FLOAT_REGS_FLAG:
   case INT_REGS_FLAG:
   case TIME_REGS_FLAG:
      return 4;
   case U64_REGS_FLAG:
   case S64_REGS_FLAG:
   case DOUBLE_REGS_FLAG:
      return 8;
   default:
      return 0;
   }
}

void regs_copy_safe(void *reg_to, void *reg_from, u32 size) {
   memcpy(reg_to, reg_from, size);
}

int mirror_access_write(regs_template_t *regs_template) {
   (void)regs_template;
   return 0;
}

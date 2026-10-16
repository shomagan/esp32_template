#include <string.h>
#include "unity.h"
#include "regs.h"
#include "regs_description.h"

extern const u32 def_table_version;

static int main_table_ind = -1;

static void register_main_table(void) {
   if (main_table_ind < 0) {
      if (regs_access_mutex == NULL) {
         regs_access_mutex = semaphore_create_binary();
         semaphore_release(regs_access_mutex);
      }
      static u8               saved_buf[4];
      regs_description_list_t table = {
         .description            = regs_description_global,
         .num_of_regs            = NUM_OF_MAIN_VARS,
         .table_version          = &def_table_version,
         .space_name             = "GLOBAL_SPACE",
         .saved_regs_buffer      = saved_buf,
         .saved_regs_buffer_size = sizeof(saved_buf),
      };
      main_table_ind = regs_description_list_add_new(table);
   }
}

TEST_CASE("main table registers as table 0", "[regs_description]") {
   register_main_table();
   TEST_ASSERT_EQUAL_INT(0, main_table_ind);
   TEST_ASSERT_EQUAL_UINT32(NUM_OF_MAIN_VARS, regs_description_list_get_num_of_regs(0));
}

TEST_CASE("register names are unique", "[regs_description]") {
   for (u32 i = 0; i < NUM_OF_MAIN_VARS; i++) {
      for (u32 j = i + 1; j < NUM_OF_MAIN_VARS; j++) {
         TEST_ASSERT_NOT_EQUAL_MESSAGE(0, strcmp(regs_description_global[i].name, regs_description_global[j].name),
                                       regs_description_global[i].name);
      }
   }
}

TEST_CASE("every register is found by name, guid and address", "[regs_description]") {
   register_main_table();
   for (u32 i = 0; i < NUM_OF_MAIN_VARS; i++) {
      const regs_description_t *expected      = &regs_description_global[i];
      regs_template_t           regs_template = { 0 };

      regs_template.name = expected->name;
      TEST_ASSERT_EQUAL_INT_MESSAGE(0, regs_description_get_by_name(&regs_template), expected->name);
      TEST_ASSERT_EQUAL_PTR_MESSAGE(expected->p_value, regs_template.p_value, expected->name);

      memset(&regs_template, 0, sizeof(regs_template));
      regs_template.guid = expected->guid;
      TEST_ASSERT_EQUAL_INT_MESSAGE(0, regs_description_get_by_guid(&regs_template), expected->name);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(expected->name, regs_template.name, expected->name);

      memset(&regs_template, 0, sizeof(regs_template));
      regs_template.p_value = expected->p_value;
      TEST_ASSERT_EQUAL_INT_MESSAGE(0, regs_description_get_by_address(&regs_template), expected->name);
      TEST_ASSERT_EQUAL_STRING_MESSAGE(expected->name, regs_template.name, expected->name);
   }
}

TEST_CASE("unknown name is not found", "[regs_description]") {
   register_main_table();
   regs_template_t regs_template = { .name = "no_such_register" };
   TEST_ASSERT_EQUAL_INT(-1, regs_description_get_by_name(&regs_template));
}

TEST_CASE("every register is found by modbus address", "[regs_description]") {
   register_main_table();
   for (u32 i = 0; i < NUM_OF_MAIN_VARS; i++) {
      const regs_description_t *expected = &regs_description_global[i];
      u16                       address  = RD_MDB_ADDRESS(expected->modbus_description);
      u8                        function = RD_MDB_FUNCTION(expected->modbus_description);
      TEST_ASSERT_EQUAL_PTR_MESSAGE(expected->p_value, regs_description_get_pointer_by_modbus(address, function), expected->name);
   }
}

TEST_CASE("set_regs_def_values applies defaults and clears the rest", "[regs_description]") {
   register_main_table();
   memset(regs_global, 0xA5, sizeof(*regs_global));
   TEST_ASSERT_EQUAL_INT(1, set_regs_def_values((u16)main_table_ind));
   for (u32 i = 0; i < NUM_OF_MAIN_VARS; i++) {
      regs_template_t regs_template = { 0 };
      TEST_ASSERT_EQUAL_INT(0, regs_description_get_by_index(&regs_template, i));
      if (regs_template.p_default != NULL) {
         TEST_ASSERT_EQUAL_MEMORY_MESSAGE(regs_template.p_default, regs_template.p_value, regs_template.size_in_bytes,
                                          regs_template.name);
      } else {
         static const u8 zeros[MAX_ARRAY_SIZE] = { 0 };
         TEST_ASSERT_EQUAL_MEMORY_MESSAGE(zeros, regs_template.p_value, regs_template.size_in_bytes, regs_template.name);
      }
   }
}

TEST_CASE("read only flag decides writability", "[regs_description]") {
   register_main_table();
   regs_template_t regs_template = { .name = "wifi_state" };
   TEST_ASSERT_EQUAL_INT(0, regs_description_get_by_name(&regs_template));
   TEST_ASSERT_EQUAL_UINT8(0, regs_description_is_writable(&regs_template));
   regs_template_t command = { .name = "command" };
   TEST_ASSERT_EQUAL_INT(0, regs_description_get_by_name(&command));
   TEST_ASSERT_EQUAL_UINT8(1, regs_description_is_writable(&command));
}

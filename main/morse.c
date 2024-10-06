/**
 * @file morse.c
 * @author Shoma Gane <shomagan@gmail.com>
 *         
 * @defgroup main
 * @ingroup main
 * @version 0.1 
 * @brief  TODO!!! write brief in 
 */
#ifndef MORSE_C
#define MORSE_C 1
 
#include "morse.h"
#include "pin_map.h"
#include "esp_log.h"
#include "esp_check.h"
#include "regs_description.h"
#include "driver/gpio.h"
#include <string.h>
#include "sleep_control.h"

task_handle_t morse_handle_id = NULL;
static const char *TAG = "morse";
#define MORSE_TASK_PERIOD (5u)

#if MORSE
#define MORSE_UNIT_TIME_MS morse_unit_time
#define DOT_LENGTH MORSE_UNIT_TIME_MS
#define DASH_LENGTH (3u*MORSE_UNIT_TIME_MS)
#define SPACE_SIGNALS MORSE_UNIT_TIME_MS
#define SPACE_LETTERS (3u*MORSE_UNIT_TIME_MS)
#define SPACE_WORDS (7u*MORSE_UNIT_TIME_MS)
#define TIME_BEFORE_SLEEP_MS (30*60*1000)
typedef enum{
   MORSE_LETTER,
   MORSE_DIGIT,
   MORSE_SIGN,
   MORSE_SPACE,
   MORSE_NEW_LINE,
   MORSE_COMMAND_ENABLE_BL,
   MORSE_COMMAND_DISABLE_BL,
   MORSE_COMMAND_SLEEP,
   MORSE_COMMAND_REMOVE_LAST,
   MORSE_COMMAND_BROADCAST,
   MORSE_END
}morse_type_t;
struct morse_node{
   const char * const letter;
   const void * const dit;
   const void * const dah;
   const morse_type_t type;
};
typedef struct{
   u8 * message[DISPLAY_LINES_NUM];
}morse_reseived_t;
morse_reseived_t morse_reseived;
const struct morse_node root_letter = {.letter = NULL, .dit = &e_letter, .dah = &t_letter};
const struct morse_node e_letter = {.letter ="E", .dit = &i_letter, .dah = &a_letter};
const struct morse_node i_letter = {.letter ="I", .dit = &s_letter, .dah = &u_letter};
const struct morse_node s_letter = {.letter ="S", .dit = &h_letter, .dah = &v_letter};
const struct morse_node h_letter = {.letter ="H", .dit = &d5_digit, .dah = &d4_digit};
const struct morse_node d5_digit = {.letter ="5", .dit = &bl_enable_command, .dah = &bl_disable_command};
/* . . . . . .*/
const struct morse_node bl_enable_command = {.letter ="!", .dit =&morse_sleep_command, .dah = NULL, .type = MORSE_COMMAND_ENABLE_BL};
/* . . . . . _*/
const struct morse_node bl_disable_command = {.letter ="!", .dit =NULL, .dah = NULL, .type = MORSE_COMMAND_DISABLE_BL};
/* . . . . . . .*/
const struct morse_node morse_sleep_command = {.letter ="!", .dit =NULL, .dah = NULL, .type = MORSE_COMMAND_SLEEP};
const struct morse_node d4_digit = {.letter ="4", .dit = NULL, .dah = NULL};
const struct morse_node v_letter = {.letter ="V", .dit = NULL, .dah = &d3_digit};
const struct morse_node d3_digit = {.letter ="3", .dit = NULL, .dah = NULL};
const struct morse_node u_letter = {.letter ="U", .dit = &f_letter, .dah = &utta_letter};
const struct morse_node f_letter = {.letter ="F", .dit = NULL, .dah = NULL};
const struct morse_node utta_letter = {.letter ="Ut", .dit = &detta_letter, .dah = &d2_digit};
const struct morse_node detta_letter = {.letter ="Dt", .dit = &question_sign, .dah = &underscore_sign};
const struct morse_node question_sign= {.letter ="?", .dit = NULL, .dah = NULL};
const struct morse_node underscore_sign= {.letter ="_", .dit = NULL, .dah = NULL};
const struct morse_node d2_digit = {.letter ="2", .dit = NULL, .dah = NULL};
const struct morse_node a_letter = {.letter ="A", .dit = &r_letter, .dah = &w_letter};
const struct morse_node r_letter = {.letter ="R", .dit = &l_letter, .dah = &atte_letter}; 
const struct morse_node l_letter = {.letter ="L", .dit = NULL, .dah = &etta_letter};
const struct morse_node etta_letter = {.letter ="E", .dit =&quotes_signs, .dah = NULL};
const struct morse_node quotes_signs = {.letter ="\'\'", .dit =NULL, .dah = NULL};
const struct morse_node atte_letter = {.letter ="At", .dit =&plus_sign, .dah = NULL};
const struct morse_node plus_sign = {.letter ="+", .dit =NULL, .dah = &dot_sign};
const struct morse_node dot_sign = {.letter =".", .dit =NULL, .dah = NULL};
const struct morse_node w_letter = {.letter ="W", .dit = &p_letter, .dah = &j_letter};
const struct morse_node p_letter = {.letter ="P", .dit = NULL, .dah = &amba_letter};
const struct morse_node amba_letter = {.letter ="Ab", .dit = &dog_sign, .dah = NULL};
const struct morse_node dog_sign = {.letter ="@", .dit = NULL, .dah = NULL};
const struct morse_node j_letter = {.letter ="J", .dit = &jetta_letter, .dah = &d1_digit};
const struct morse_node jetta_letter = {.letter ="Jt", .dit = NULL, .dah = NULL};

const struct morse_node d1_digit = {.letter ="1", .dit = &quote_sign, .dah = NULL};
const struct morse_node quote_sign = {.letter ="\'", .dit = NULL, .dah = NULL};
const struct morse_node t_letter = {.letter ="T", .dit = &n_letter, .dah = &m_letter};
const struct morse_node n_letter = {.letter ="N", .dit = &d_letter, .dah = &k_letter};
const struct morse_node d_letter = {.letter ="D", .dit = &b_letter, .dah = &x_letter};
const struct morse_node b_letter = {.letter ="B", .dit = &d6_digit, .dah = &equal_sign};
const struct morse_node d6_digit = {.letter ="6", .dit = NULL, .dah = &minus_sign};
const struct morse_node minus_sign = {.letter ="-", .dit = NULL, .dah = NULL};
const struct morse_node equal_sign = {.letter ="=", .dit = NULL, .dah = NULL};
const struct morse_node x_letter = {.letter ="X", .dit = &slash_sign, .dah = &space_sign};
const struct morse_node slash_sign = {.letter ="/", .dit = NULL, .dah = NULL};
const struct morse_node space_sign = {.letter =" ", .dit = NULL, .dah = NULL};
const struct morse_node k_letter = {.letter ="K", .dit = &c_letter, .dah = &y_letter};
const struct morse_node c_letter = {.letter ="C", .dit = NULL, .dah = &new_line};
const struct morse_node new_line = {.letter ="\n", .dit = &semicolon, .dah = &exclamation};
const struct morse_node semicolon = {.letter =";", .dit = NULL, .dah = NULL};
const struct morse_node exclamation = {.letter ="!", .dit = NULL, .dah = NULL};
const struct morse_node y_letter = {.letter ="Y", .dit = &open_bracket, .dah = NULL};
const struct morse_node open_bracket = {.letter ="(", .dit = NULL, .dah = &close_bracket};
const struct morse_node close_bracket = {.letter =")", .dit = NULL, .dah = NULL};
const struct morse_node m_letter = {.letter ="M", .dit = &g_letter, .dah = &o_letter};
const struct morse_node g_letter = {.letter ="G", .dit = &z_letter, .dah = &q_letter};
const struct morse_node z_letter = {.letter ="Z", .dit = &d7_digit, .dah = &zetta_letter};
const struct morse_node d7_digit = {.letter ="7", .dit = NULL, .dah = NULL};
const struct morse_node zetta_letter = {.letter ="Zt", .dit = NULL, .dah = &comma};
const struct morse_node comma = {.letter =",", .dit = NULL, .dah = NULL};
const struct morse_node q_letter = {.letter ="Q", .dit = NULL, .dah = NULL};
const struct morse_node o_letter = {.letter ="O", .dit = &otto_letter, .dah = &chapter};
/*_ _ _ . */
const struct morse_node otto_letter = {.letter ="Ot", .dit = &d8_digit, .dah = &remove_last_command};
/*_ _ _ . _*/
const struct morse_node remove_last_command = {.letter ="!", .dit = NULL, .dah = NULL, .type=MORSE_COMMAND_REMOVE_LAST};
const struct morse_node d8_digit = {.letter ="8", .dit = &colon, .dah = NULL};
const struct morse_node colon = {.letter =":", .dit = NULL, .dah = NULL};
const struct morse_node chapter = {.letter ="   ", .dit = &d9_digit, .dah = &d0_digit};
const struct morse_node d9_digit = {.letter ="9", .dit = NULL, .dah = NULL};
/*_ _ _ _ _*/
const struct morse_node d0_digit = {.letter ="0", .dit = NULL, .dah = &broadcast_the_message_command};
/*_ _ _ _ _ _*/
const struct morse_node broadcast_the_message_command = {.letter ="!", .dit = NULL, .dah = NULL, .type=MORSE_COMMAND_BROADCAST};

int morse_unit_time = 100; 
static int morse_init(void);
static int morse_deinit();
static u16 fill_morse_log_buffer(u8 * buffer, const char * letter, u16 log_pointer, u16 log_size);
static void handle_next_letter(const struct morse_node * node);

static int morse_init(){
   int result = 0;
   regs_global->vars.current_state[0] |= CS0_TASK_ACTIVE_MORSE;
   morse_reseived.message[0] = &morse_reg->vars.morse_line_1[0];
   morse_reseived.message[1] = &morse_reg->vars.morse_line_2[0];
   morse_reseived.message[2] = &morse_reg->vars.morse_line_3[0];
   morse_reseived.message[3] = &morse_reg->vars.morse_line_4[0];
   return result;
}  
static int morse_deinit(){
   int result = 0;
   regs_global->vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_MORSE);
   return result;
}

static u32 time_state(u32 pin_state_base){
   u32 start_time = task_get_time_ms();
   while(1){
      u32 pin_state = gpio_get_level(EXT_WAKEUP_PIN);
      u32 diff = task_get_time_ms() - start_time;
      if((pin_state != pin_state_base) || ((diff>SPACE_WORDS) && !pin_state)){
         return diff;
      }
      task_delay_ms(5);
   }
}

void morse_task(void *arg){
   (void)(*arg);
   uint32_t signal_value;
   morse_init();
   u64 task_counter = 0u;
   main_printf(TAG,"morse task ready to start");
   u32 last_button_press = task_get_tick_count();
   while(1){
      morse_unit_time = morse_reg->vars.morse_unit_time_ms;
      if(task_notify_wait(STOP_CHILD_PROCCES, &signal_value, MORSE_TASK_PERIOD)==pdTRUE){
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES){
            morse_deinit();
            task_delete(task_get_id());
         }
      }
      u32 pin_state = gpio_get_level(EXT_WAKEUP_PIN);
      if(pin_state){
         const struct morse_node * node = &root_letter; 
         last_button_press = task_get_tick_count();
         while(node){
            u64 diff = time_state(pin_state);
            if(diff > DASH_LENGTH){
               node = node->dah;
            }else{
               node = node->dit;
            }
            diff = time_state(0);
            if(diff > SPACE_WORDS){
               break;
            }else if (diff > SPACE_LETTERS){
               break;
            }
         }
         handle_next_letter(node);
      }
      if((task_get_tick_count() - last_button_press) > (TIME_BEFORE_SLEEP_MS/portTICK_PERIOD_MS)){
         u32 prev_value = 0;
         u32 signal = (u32)SLEEP_TASK_DEEP_SLEEP;
         task_notify_send(sleep_control_handle_id, signal, &prev_value);
      }
      task_counter++;
   }
}
static void handle_next_letter(const struct morse_node * node){
   u16 max_log_size = 0;
   static u16 log_pointer = 0u;
   max_log_size = morse_reg->vars.morse_message_len;
   if(node!= NULL && node->letter != NULL){
      if(node->type == MORSE_COMMAND_ENABLE_BL){
         gpio_set_level(NOKIA_PIN_BL, 1);
      }else if(node->type == MORSE_COMMAND_DISABLE_BL){
         gpio_set_level(NOKIA_PIN_BL, 0);
      }else if(node->type == MORSE_COMMAND_SLEEP){
         u32 prev_value = 0;
         u32 signal = (u32)SLEEP_TASK_DEEP_SLEEP;
         task_notify_send(sleep_control_handle_id, signal, &prev_value);
      }else if(node->type == MORSE_COMMAND_REMOVE_LAST){
         if(log_pointer>0){
            log_pointer--;
         }
         morse_reg->vars.morse_message[log_pointer] = 0;
      }else if(node->type == MORSE_COMMAND_BROADCAST){
         log_pointer = 0;
         regs_copy_safe(&morse_reg->vars.morse_send[0], &morse_reg->vars.morse_message[0], max_log_size);
         memset(&morse_reg->vars.morse_message[0], 0, max_log_size);
         semaphore_take(regs_access_mutex, portMAX_DELAY);{
            morse_reg->vars.morse_counter++;
         }semaphore_release(regs_access_mutex);
      } else {
         log_pointer = fill_morse_log_buffer(&morse_reg->vars.morse_message[0], node->letter, log_pointer, max_log_size);
      }
   }
   morse_reg->vars.morse_message_position = log_pointer;
}
static u16 fill_morse_log_buffer(u8 * buffer, const char * letter, u16 log_pointer, u16 max_log_size){
   for(int i = 0; i<strlen(letter); i++){
      buffer[log_pointer] = letter[i];
      if(log_pointer<max_log_size){
         log_pointer++;
      }else{
         log_pointer = 0;
      }
   }
   return log_pointer;
}
u8 * get_pointer_to_line(u8 line){
   u8 * result = NULL;
   if(line < DISPLAY_LINES_NUM){
      result = morse_reseived.message[line];
   }
   return result;
}
u8 message_is_new(u8 * message,u8 len){
   u8 result = 1;
   semaphore_take(regs_access_mutex, portMAX_DELAY);{
   if(message != NULL){
      for(u8 i = 0; i < DISPLAY_LINES_NUM ; i++){
         if(strncmp((char *)message,(char *)morse_reseived.message[i],len)==0){
            result = 0;
            break;
         }
      }
   }else{
      result = 0;
   }
   }semaphore_release(regs_access_mutex);
   return result;
}
#endif //MORSE
#endif /*MORSE_C*/

/**
 * @file morse.h
 * @author Shoma Gane <shomagan@gmail.com>
 *
 * @defgroup main
 * @ingroup main
 * @version 0.1
 * @brief  TODO!!! write brief in
 */
#ifndef MORSE_H
#define MORSE_H 1

/*add includes below */
#include "type_def.h"
#include "main_config.h"
#include "regs.h"
#include "common.h"
#include "pin_map.h"

/*add includes before */
#ifdef __cplusplus
   extern "C" {
#endif
/*add functions and variable declarations below */
/**
 * @brief struct for morse component
 * name variables uses for generate name in description file and then in get value by name
 * and therefore use max size len name is 16 charackter \n
 * coment style :   "" - description, \n
 *                  &ro  - read only, \n
 *                  &def -> have const varibale with struct like def_name, \n
 *                  &save- will have saved in bkram, \n
 *  @ingroup regs
 */
/** #generator_use_description {"space_name" :"morse_reg_t",  "address_space" :9, "modbus_type" :"server", "modbus_function" :"holding_registers", "register_start_address" :4500}*/
typedef union{
    struct MCU_PACK{
        // start regs struct
        u16 morse_unit_time_ms;         //!<"morse time ms" &save &def &min &max
        u16 morse_message_len;         //!<"max morse message len" &save &def &min &max
        u16 morse_settings;         //!<"morse settings bit0 - server" &save &def
        u16 morse_message_position;         //!<"morse message position" &ro
        u8 morse_message[32];         //!<"morse message to broadcast" &ro
        u8 morse_send[32];         //!<"morse message to broadcast" &ro
        u8 morse_line_1[32];         //!<"morse messages received " &ro
        u8 morse_line_2[32];         //!<"morse messages received " &ro
        u8 morse_line_3[32];         //!<"morse messages received " &ro
        u8 morse_line_4[32];         //!<"morse messages received " &ro
        u8 morse_line_5[32];         //!<"morse messages received " &ro
        u32 morse_counter;         //!<"morse messages sent and received counter" &ro
    }vars;
    u32 bytes[256]; //for full bksram copy
}morse_reg_t;// #generator_use_description {"message":"end_struct"}
extern morse_reg_t * const morse_reg;

extern task_handle_t morse_handle_id;

#if MORSE
#define MAX_MORSE_MESSAGE_LEN 32
#define DISPLAY_LINES_NUM 4

extern void morse_task(void *arg);
u8 * get_pointer_to_line(u8 line);
extern const struct morse_node root_letter ;
extern const struct morse_node e_letter ;
extern const struct morse_node i_letter ;
extern const struct morse_node s_letter ;
extern const struct morse_node h_letter ;
extern const struct morse_node d5_digit ;
extern const struct morse_node bl_enable_command;
extern const struct morse_node bl_disable_command;
extern const struct morse_node morse_sleep_command;

extern const struct morse_node d4_digit ;
extern const struct morse_node v_letter ;
extern const struct morse_node d3_digit ;
extern const struct morse_node u_letter ;
extern const struct morse_node f_letter ;
extern const struct morse_node utta_letter ;
extern const struct morse_node detta_letter ;
extern const struct morse_node question_sign;
extern const struct morse_node underscore_sign;
extern const struct morse_node d2_digit ;
extern const struct morse_node a_letter ;
extern const struct morse_node r_letter ;
extern const struct morse_node l_letter ;
extern const struct morse_node etta_letter ;
extern const struct morse_node quotes_signs ;
extern const struct morse_node atte_letter ;
extern const struct morse_node plus_sign ;
extern const struct morse_node dot_sign ;
extern const struct morse_node w_letter ;
extern const struct morse_node p_letter ;

extern const struct morse_node amba_letter ;
extern const struct morse_node dog_sign ;
extern const struct morse_node j_letter ;
extern const struct morse_node jetta_letter;
extern const struct morse_node d1_digit ;
extern const struct morse_node quote_sign ;
extern const struct morse_node t_letter ;
extern const struct morse_node n_letter ;
extern const struct morse_node d_letter ;
extern const struct morse_node b_letter ;
extern const struct morse_node d6_digit ;
extern const struct morse_node minus_sign ;
extern const struct morse_node equal_sign ;
extern const struct morse_node x_letter ;
extern const struct morse_node slash_sign ;
extern const struct morse_node space_sign;
extern const struct morse_node k_letter ;
extern const struct morse_node c_letter ;
extern const struct morse_node new_line ;
extern const struct morse_node semicolon ;
extern const struct morse_node exclamation ;
extern const struct morse_node y_letter ;
extern const struct morse_node open_bracket ;
extern const struct morse_node close_bracket ;
extern const struct morse_node m_letter ;
extern const struct morse_node g_letter ;
extern const struct morse_node z_letter ;
extern const struct morse_node d7_digit ;
extern const struct morse_node zetta_letter ;
extern const struct morse_node comma ;
extern const struct morse_node q_letter ;
extern const struct morse_node o_letter ;
/*_ _ _ . */
extern const struct morse_node otto_letter ;
/*_ _ _ . _*/
extern const struct morse_node remove_last_command;
/*_ _ _ . .*/
extern const struct morse_node d8_digit ;
extern const struct morse_node colon ;
extern const struct morse_node chapter ;
extern const struct morse_node d9_digit ;
/*_ _ _ _ _*/
extern const struct morse_node d0_digit ;
/*_ _ _ _ _ _*/
extern const struct morse_node broadcast_the_message_command ;

u8 message_is_new(u8 * message,u8 len);

#endif //MORSE


/*add functions and variable declarations before */
#ifdef __cplusplus
}
#endif
#endif /*MORSE_H*/

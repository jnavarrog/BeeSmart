/*
 * Copyright (c) 2018, George Oikonomou - http://www.spd.gr
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "dev/button-hal.h"
#include "orchestra.h"
#include "tsch.h"
#include <stdio.h>
#include <string.h>

//agregados
#include "net/ipv6/uip.h"
#include "net/ipv6/uiplib.h"
#include "net/ipv6/uip-icmp6.h"
#include "net/ipv6/uip-ds6.h"
#include "shell.h"
#include "serial-shell.h"
#include "net/routing/rpl-lite/rpl.h"

#define IPNODO "fd00::212:4b00:1376:3383"

/*---------------------------------------------------------------------------*/
PROCESS(button_hal_example, "Button HAL Example");
AUTOSTART_PROCESSES(&button_hal_example);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
static void
serial_shell_output(const char *str) {

  if(strstr(str,"Received ping reply from")!=NULL){
	printf("DODAG RESPONDE PING: SI\n");
  } else if(strstr(str,"Timeout")!=NULL){
	printf("DODAG RESPONDE PING: NO\n");
  }
  //printf("%s", str);
}
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(button_hal_example, ev, data)
{
 
  button_hal_button_t *btn;
  static struct pt shell_input_pt;
  char comando[UIPLIB_IPV6_MAX_STR_LEN+8] = "help";
  char ip_dodag[UIPLIB_IPV6_MAX_STR_LEN];

  PROCESS_BEGIN();

  button_hal_init();

  btn = button_hal_get_by_index(0);

  printf("Button HAL example.\n");
  printf("Device button count: %u.\n", button_hal_button_count);

  if(btn) {
    printf("%s on pin %u with ID=0, Logic=%s, Pull=%s\n",
           BUTTON_HAL_GET_DESCRIPTION(btn), btn->pin,
           btn->negative_logic ? "Negative" : "Positive",
           btn->pull == GPIO_HAL_PIN_CFG_PULL_UP ? "Pull Up" : "Pull Down");
  }

  while(1) {

    PROCESS_YIELD();

    if(ev == button_hal_press_event) {
      btn = (button_hal_button_t *)data;
      //printf("-->Apreta boton (%s)\n", BUTTON_HAL_GET_DESCRIPTION(btn));

      
    } else if(ev == button_hal_release_event) {
      btn = (button_hal_button_t *)data;
      //printf("-->Suelta boton (%s)\n", BUTTON_HAL_GET_DESCRIPTION(btn));

      if(tsch_is_associated){
	printf("ASOCIADO A SCHEDULE TSCH: SI\n");
      }else{ 
	printf("ASOCIADO A SCHEDULE TSCH: NO\n");
      }

      if(curr_instance.used){
	printf("INSTANCIA RPL: SI\n");
      }else{ 
	printf("INSTANCIA RPL: NO\n");
      }

      uiplib_ipaddr_snprint(ip_dodag, sizeof(ip_dodag), &curr_instance.dag.dag_id);
      strcpy(comando,"ping ");
      strcat(comando,ip_dodag);
      PROCESS_PT_SPAWN(&shell_input_pt, shell_input(&shell_input_pt, serial_shell_output, comando));

    }//fin if soltar boton 
  }//fin while

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/


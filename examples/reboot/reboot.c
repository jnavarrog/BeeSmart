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
/*---------------------------------------------------------------------------*/
PROCESS(button_hal_example, "Button HAL Example");
AUTOSTART_PROCESSES(&button_hal_example);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(button_hal_example, ev, data)
{
  button_hal_button_t *btn;

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
      printf("Reiniciando...\n");	
      watchdog_reboot(); //esta función reinicia el sistema

    } 
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

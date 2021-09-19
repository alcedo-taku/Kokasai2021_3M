#include "wrapper.hpp"

/* Include Begin */
#include "DFRobotDFPlayerMini.hpp"
#include "usart.h"
/* Include End */

/* Enum Begin */
/* Enum End */

/* Struct Begin */
/* Struct End */

/* Variable Begin */
/* Variable End */

/* Class Constructor Begin */
DFRobotDFPlayerMini DFPlayerMini;
/* Class Constructor End */

/* Function Prototype Begin */
/* Function Prototype End */

void init(void){
	DFPlayerMini.begin(&huart2, false, false);
	DFPlayerMini.next();
	DFPlayerMini.Send_cmd(0x01, 0x00, 0x00);
}

void loop(void){

}

/* Function Body Begin */
/* Function Body End */

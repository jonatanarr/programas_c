
#ifndef MMA8451_H_
#define MMA8451_H_

/*==================[inclusions]=============================================*/
#include "stdint.h"

/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions definition]==========================*/

/** \brief configura aceler�metro MMA8451
 **	
 **/
void mma8451_init(void);

/** \brief Lee lectura del aceler�metro en el eje Z
 **
 ** \return Lectura del aceler�metro en cent�cimas de g
 **/
int16_t iAcclReadX(void);
int16_t iAcclReadY(void);
int16_t iAcclReadZ(void);

/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* MMA8451_H_ */





#define RES_X               84      /**< horizontal screen resolution in pixel */
#define RES_Y               48      /**< vertical screen resolution in pixel */

#define COM_CMD             0       /**< we are sending a command */
#define COM_DTA             1       /**< we are sending some data */

#define CMD_DISABLE         0x24    /**< set LCD into power down mode */
#define CMD_ENABLE_H        0x20    /**< power up LCD and use horizontal addressing */
#define CMD_ENABLE_V        0x23    /**< power up LCD and use vertical addressing */
#define CMD_EXTENDED        0x21    /**< enter extended instruction set */

#define CMD_MODE_BLANK      0x08
#define CMD_MODE_NORMAL     0x0c
#define CMD_MODE_ALLON      0x09
#define CMD_MODE_INVERSE    0x0d

#define CMD_SET_Y           0x40    /**< set 3-bit y address, OR with address value */
#define CMD_SET_X           0x80    /**< set 7-bit x address, OR with address value */

#define CMD_EXT_TEMP0       0x04    /**< set temperature coefficient to 0 */
#define CMD_EXT_TEMP1       0x05    /**< set temperature coefficient to 1 */
#define CMD_EXT_TEMP2       0x06    /**< set temperature coefficient to 2 */
#define CMD_EXT_TEMP3       0x07    /**< set temperature coefficient to 3 */

#define CMD_EXT_BIAS0       0x10    /**< select n=7, recommended mux is 1:100 */
#define CMD_EXT_BIAS1       0x11    /**< select n=6, recommended mux is 1:80 */
#define CMD_EXT_BIAS2       0x12    /**< select n=5, recommended mux is 1:65 */
#define CMD_EXT_BIAS3       0x13    /**< select n=4, recommended mux is 1:48 recommended */
#define CMD_EXT_BIAS4       0x14    /**< select n=3, recommended mux is 1:34 */
#define CMD_EXT_BIAS5       0x15    /**< select n=2, recommended mux is 1:24 */
#define CMD_EXT_BIAS6       0x16    /**< select n=1. recommended mux is 1:16 */
#define CMD_EXT_BIAS7       0x17    /**< select n=0, recommended mux is 1:8 */

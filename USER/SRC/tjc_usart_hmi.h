/*

 */
 
#ifndef __TJCUSARTHMI_H_
#define __TJCUSARTHMI_H_

 

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
 
void uart0_send_char(char ch);
void uart0_send_string(char* str);
void tjc_send_string(char* str);
/**
	打印到屏幕串口
*/
void TJCPrintf(const char *cmd, ...);
void initRingBuff(void);
void writeRingBuff(uint8_t data);
void deleteRingBuff(uint16_t size);
uint16_t getRingBuffLength(void);
uint8_t read1BFromRingBuff(uint16_t position);



#define RINGBUFF_LEN	(500)     //定义最大接收字节数 500

#define     usize               getRingBuffLength()
#define     u_init()            initRingBuff()
#define     udelete(x)          deleteRingBuff(x)
#define     u(x)                read1BFromRingBuff(x)



#endif  /*  __TJCUSARTHMI_H_    */

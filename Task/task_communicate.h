#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include "sys.h"
#include "data_fifo.h"
#include "protocol.h"

#define UART_RX_DMA_SIZE      (1024)
#define DMA_BUFFER_SIZE       (1024)

#define UART_TX_SIGNAL      ( 1 << 2 )
#define UART_IDLE_SIGNAL    ( 1 << 1 )


typedef struct{
	USART_TypeDef *USARTx;
	
	DMA_TypeDef *rx_DMAx;
	uint32_t rx_Stream;
	DMA_Stream_TypeDef *rx_dma_stream;
	
	DMA_TypeDef *tx_DMAx;
	uint32_t tx_Stream;
	DMA_Stream_TypeDef *tx_dma_stream;
	
	u32 rx_usart_dma_memory_base_address_0,rx_usart_dma_memory_base_address_1,rx_usart_dma_buffer_size;
	
	u8 tx_finish_flag;
	osThreadId *task;
}usart_param_struct;

/*********************************************************************************	
 * 														协议部分——公用协议
 *********************************************************************************/
 
#define UP_REG_ID    0xA0  //上位机通信
#define DN_REG_ID    0xA5  //裁判系统通信
#define HEADER_LEN   sizeof(frame_header_t)
#define CMD_LEN      2    //命令帧
#define CRC_LEN      2    //CRC16校验

#define PROTOCAL_FRAME_MAX_SIZE  200


/** 
  * @brief  帧头定义
  */
typedef __packed struct
{
  uint8_t  sof;							//0xA0为与上位机通信，0xA5为与下位机通信
  uint16_t data_length;			//数据段长度
  uint8_t  seq;							//包序号
  uint8_t  crc8;						//帧头校验
} frame_header_t;

/** 
  * @brief  解包步骤定义
  */
typedef enum
{
  STEP_HEADER_SOF  = 0,
  STEP_LENGTH_LOW  = 1,
  STEP_LENGTH_HIGH = 2,
  STEP_FRAME_SEQ   = 3,
  STEP_HEADER_CRC8 = 4,
  STEP_DATA_CRC16  = 5,
} unpack_step_e;

/** 
  * @brief  串口中断类型
  */
typedef enum
{
  UART_IDLE_IT     = 0,
  UART_DMA_HALF_IT = 1,
  UART_DMA_FULL_IT = 2,
} uart_it_type_e;

typedef struct
{
  DMA_Stream_TypeDef *dma_stream;
  fifo_s_t           *data_fifo;
  uint16_t           buff_size;
  uint8_t            *buff[2];
  uint16_t           read_index;
  uint16_t           write_index;
} uart_dma_rxdata_t;

typedef struct
{
  fifo_s_t       *data_fifo;
  frame_header_t *p_header;
  uint16_t       data_len;
  uint8_t        protocol_packet[PROTOCAL_FRAME_MAX_SIZE];
  unpack_step_e  unpack_step;
  uint16_t       index;
} unpack_data_t;

/*********************************************************************************	
 * 														协议部分——与上位机通信
 *********************************************************************************/

#define COMPUTER_FIFO_BUFLEN 500


/** 
  * @brief  命令码定义  接收部分为0x00A-,发送部分为0x001-
  */
typedef enum
{
	
  CHASSIS_CTRL_ID     = 0x00A0,
  GIMBAL_CTRL_ID      = 0x00A1,
	
//------------------步兵字段：0x1??? ---------------------
	INFANTRY_BUFF_ID	        = 0x1000,
	INFANTRY_ENEMY_ID	        = 0x1001,
	
	//上发部分
	INFANTRY_SHOOT_MODE_ID		= 0x10A1,
//------------------英雄字段：0x2??? ---------------------
	HERO_AUTO_SHOOT_ID		= 0x2000,

//------------------工程字段：0x3??? ---------------------
	ENGINEER_ID		        = 0x3000,
	
//------------------哨兵字段：0x4??? ---------------------
	GUARD_AUTO_SHOOT_ID		= 0x4000,
	
//------------------飞机字段：0x5??? ---------------------
	DRONE_AUTO_SHOOT_ID		= 0x5000,

//------------------补给站字段：0x6??? ---------------------

	SUPPLY_ID		        = 0x6000,

} infantry_data_id_e;

/** 
  * @brief  gimbal control information
  */
typedef __packed struct
{
  uint8_t ctrl_mode;    /* gimbal control mode */
  float   pit_ref;      /* gimbal pitch reference angle(degree) */
  float   yaw_ref;      /* gimbal yaw reference angle(degree) */
  uint8_t visual_valid; /* visual information valid or not */
} gimbal_ctrl_t;

typedef __packed struct
{
  int16_t x_offset;   /* offset(mm) relative to the x-axis of the chassis center */
  int16_t y_offset;   /* offset(mm) relative to the y-axis of the chassis center */
  float   w_spd;    /* rotation speed(degree/s) of chassis */
} chassis_rotate_t;
/** 
  * @brief  chassis control information
  */
typedef __packed struct
{
  uint8_t          ctrl_mode; /* chassis control mode */
  int16_t          x_spd;   /* x-axis move speed(mm/s) of chassis */
  int16_t          y_spd;   /* y-axis move speed(mm/s) of chassis */
  chassis_rotate_t w_info;    /* rotation control of chassis */
} chassis_ctrl_t;

typedef __packed struct {
  int32_t enemy_dist;
  int32_t enemy_yaw;      /* gimbal pitch reference angle(degree) */
  int32_t enemy_pitch;      /* gimbal yaw reference angle(degree) */
  int32_t mode; /* visual information valid or not */
}GimbalShootControl;

typedef __packed struct {
  int32_t mode; /* visual information valid or not */
}GimbalMode;

typedef struct
{
  /* data receive */
//  gimbal_ctrl_t        gimbal_control_data;
	chassis_ctrl_t       chassis_control_data;
	GimbalShootControl   gimbal_enemy_data;
	GimbalShootControl   gimbal_buff_data;
	
	//增加协议在这里添加
	
	
} receive_pc_t;

/*********************************************************************************	
 * 														协议部分——与裁判系统通信
 *********************************************************************************/

#define JUDGE_FIFO_BUFLEN 500

/** 
  * @brief  裁判系统命令码
  */
typedef enum
{
//从裁判系统接收
  GAME_INFO_ID       = 0x0001,		//比赛信息，10Hz频率
  REAL_BLOOD_DATA_ID = 0x0002,		//伤害数据，受伤害发送
  REAL_SHOOT_DATA_ID = 0x0003,		//实时射击数据，发射弹丸发送
  REAL_POWER_DATA_ID = 0x0004,		//实时功率，热量数据，50Hz发送
  FIELD_RFID_DATA_ID = 0x0005,		//RFID数据，检测到RFID时10Hz发送
  GAME_RESULT_ID     = 0x0006,		//比赛结果数据，结束时发送一次
  GAIN_BUFF_ID       = 0x0007,		//Buff状态，Buff改变时发送一次
  ROBOT_POS_DATA_ID  = 0x0008,		//机器人位置及枪口朝向信息，50Hz
//向裁判系统发送 
  STU_CUSTOM_DATA_ID = 0x0100,		//自定义数据，频率10Hz
  ROBOT_TO_CLIENT_ID = 0x0101,
  CLIENT_TO_ROBOT_ID = 0x0102,
} judge_data_id_e;

/** 
  * @brief  game information structures definition(0x0001)
  */
typedef __packed struct
{
  uint16_t   stage_remain_time;
  uint8_t    game_process;
  /* current race stage
   0 not start
   1 preparation stage
   2 self-check stage
   3 5 seconds count down
   4 fighting stage
   5 result computing stage */
  uint8_t    robot_level;
  uint16_t   remain_hp;
  uint16_t   max_hp;
} game_robot_state_t;

/** 
  * @brief  real time blood volume change data(0x0002)
  */
typedef __packed struct
{
  uint8_t armor_type:4;
 /* 0-3bits: the attacked armor id:
    0x00: 0 front
    0x01：1 left
    0x02：2 behind
    0x03：3 right
    others reserved*/
  uint8_t hurt_type:4;
 /* 4-7bits: blood volume change type
    0x00: armor attacked
    0x01：module offline
    0x02: bullet over speed
    0x03: bullet over frequency */
} robot_hurt_data_t;

/** 
  * @brief  real time shooting data(0x0003)
  */
typedef __packed struct
{
  uint8_t bullet_type;
  uint8_t bullet_freq;
  float   bullet_spd;
} real_shoot_t;

/** 
  * @brief  real chassis power and shoot heat data(0x0004)
  *         icra need not this data
  */
typedef __packed struct
{
  float chassis_volt;
  float chassis_current;
  float chassis_power;
  float chassis_pwr_buf;
  uint16_t shooter1_heat;
  uint16_t shooter2_heat;
} real_power_data_t;

/** 
  * @brief  field rfid data(0x0005)
  */
typedef __packed struct
{
  uint8_t card_type;
  uint8_t card_idx;
} field_rfid_t;

/** 
  * @brief  game result data(0x0006)
  */
typedef __packed struct
{
  uint8_t winner;
} game_result_t;

/** 
  * @brief  the data of get field buff(0x0007)
  */
typedef __packed struct
{
  uint16_t buff_musk;
} get_buff_t;

/** 
  * @brief  field UWB data(0x0008)
  */
typedef __packed struct
{
  float x;
  float y;
  float z;
  float yaw;
} robot_position_t;

/** 
  * @brief  student custom data
  */
typedef __packed struct
{
  float data1;
  float data2;
  float data3;
	uint8_t data4;
} client_show_data_t;

typedef __packed struct
{
  uint8_t  data[64];
} user_to_server_t;

typedef __packed struct
{
  uint8_t  data[32];
} server_to_user_t;

/** 
  * @brief  the data structure receive from judgement
  */
typedef struct
{
  game_robot_state_t game_information;   //0x0001
  robot_hurt_data_t  blood_changed_data; //0x0002
  real_shoot_t       real_shoot_data;    //0x0003
  real_power_data_t  power_heat_data;    //0x0004
  field_rfid_t       rfid_data;          //0x0005
  game_result_t      game_result_data;   //0x0006
  get_buff_t         get_buff_data;      //0x0007
  robot_position_t   robot_pos_data;     //0x0008
} receive_judge_t;

void communicate_param_init(void);

void judge_unpack_task(void const *argu);
void pc_unpack_task(void const *argu);
void data_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof);


#endif

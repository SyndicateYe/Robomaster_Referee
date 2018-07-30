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
 * 														Э�鲿�֡�������Э��
 *********************************************************************************/
 
#define UP_REG_ID    0xA0  //��λ��ͨ��
#define DN_REG_ID    0xA5  //����ϵͳͨ��
#define HEADER_LEN   sizeof(frame_header_t)
#define CMD_LEN      2    //����֡
#define CRC_LEN      2    //CRC16У��

#define PROTOCAL_FRAME_MAX_SIZE  200


/** 
  * @brief  ֡ͷ����
  */
typedef __packed struct
{
  uint8_t  sof;							//0xA0Ϊ����λ��ͨ�ţ�0xA5Ϊ����λ��ͨ��
  uint16_t data_length;			//���ݶγ���
  uint8_t  seq;							//�����
  uint8_t  crc8;						//֡ͷУ��
} frame_header_t;

/** 
  * @brief  ������趨��
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
  * @brief  �����ж�����
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
 * 														Э�鲿�֡�������λ��ͨ��
 *********************************************************************************/

#define COMPUTER_FIFO_BUFLEN 500


/** 
  * @brief  �����붨��  ���ղ���Ϊ0x00A-,���Ͳ���Ϊ0x001-
  */
typedef enum
{
	
  CHASSIS_CTRL_ID     = 0x00A0,
  GIMBAL_CTRL_ID      = 0x00A1,
	
//------------------�����ֶΣ�0x1??? ---------------------
	INFANTRY_BUFF_ID	        = 0x1000,
	INFANTRY_ENEMY_ID	        = 0x1001,
	
	//�Ϸ�����
	INFANTRY_SHOOT_MODE_ID		= 0x10A1,
//------------------Ӣ���ֶΣ�0x2??? ---------------------
	HERO_AUTO_SHOOT_ID		= 0x2000,

//------------------�����ֶΣ�0x3??? ---------------------
	ENGINEER_ID		        = 0x3000,
	
//------------------�ڱ��ֶΣ�0x4??? ---------------------
	GUARD_AUTO_SHOOT_ID		= 0x4000,
	
//------------------�ɻ��ֶΣ�0x5??? ---------------------
	DRONE_AUTO_SHOOT_ID		= 0x5000,

//------------------����վ�ֶΣ�0x6??? ---------------------

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
	
	//����Э�����������
	
	
} receive_pc_t;

/*********************************************************************************	
 * 														Э�鲿�֡��������ϵͳͨ��
 *********************************************************************************/

#define JUDGE_FIFO_BUFLEN 500

/** 
  * @brief  ����ϵͳ������
  */
typedef enum
{
//�Ӳ���ϵͳ����
  GAME_INFO_ID       = 0x0001,		//������Ϣ��10HzƵ��
  REAL_BLOOD_DATA_ID = 0x0002,		//�˺����ݣ����˺�����
  REAL_SHOOT_DATA_ID = 0x0003,		//ʵʱ������ݣ����䵯�跢��
  REAL_POWER_DATA_ID = 0x0004,		//ʵʱ���ʣ��������ݣ�50Hz����
  FIELD_RFID_DATA_ID = 0x0005,		//RFID���ݣ���⵽RFIDʱ10Hz����
  GAME_RESULT_ID     = 0x0006,		//����������ݣ�����ʱ����һ��
  GAIN_BUFF_ID       = 0x0007,		//Buff״̬��Buff�ı�ʱ����һ��
  ROBOT_POS_DATA_ID  = 0x0008,		//������λ�ü�ǹ�ڳ�����Ϣ��50Hz
//�����ϵͳ���� 
  STU_CUSTOM_DATA_ID = 0x0100,		//�Զ������ݣ�Ƶ��10Hz
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
    0x01��1 left
    0x02��2 behind
    0x03��3 right
    others reserved*/
  uint8_t hurt_type:4;
 /* 4-7bits: blood volume change type
    0x00: armor attacked
    0x01��module offline
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

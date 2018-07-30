#include "task_communicate.h"
#include "cmsis_os.h"
#include "string.h"
#include "bsp_usart.h"
//变量定义
/* communicate task static parameter */
/* judge system receive data fifo and buffer*/
static osMutexId judge_rxdata_mutex;
static fifo_s_t  judge_rxdata_fifo;
static uint8_t   judge_rxdata_buf[JUDGE_FIFO_BUFLEN];
/* judge system send data fifo and buffer*/
static osMutexId judge_txdata_mutex;
static fifo_s_t  judge_txdata_fifo;
static uint8_t   judge_txdata_buf[JUDGE_FIFO_BUFLEN];
/* judge system dma receive data object */
static uart_dma_rxdata_t judge_rx_obj;
/* pc receive data fifo and buffer */
static osMutexId pc_rxdata_mutex;
static fifo_s_t  pc_rxdata_fifo;
static uint8_t   pc_rxdata_buf[COMPUTER_FIFO_BUFLEN];
/* pc send data fifo and buffer */
static osMutexId pc_txdata_mutex;
static fifo_s_t  pc_txdata_fifo;
static uint8_t   pc_txdata_buf[COMPUTER_FIFO_BUFLEN];

/* pc system dma receive data object */
static uart_dma_rxdata_t pc_rx_obj;
/* unpack object */
static unpack_data_t judge_unpack_obj;
static unpack_data_t pc_unpack_obj;


uint8_t pc_dma_rxbuff[2][UART_RX_DMA_SIZE];
uint8_t judge_dma_rxbuff[2][UART_RX_DMA_SIZE];

extern TaskHandle_t judge_unpack_task_t;
extern TaskHandle_t pc_unpack_task_t;

receive_pc_t pc_recv_mesg;

//----------------------------------------------外设初始化-----------------------------------------------------------


usart_param_struct judgement_usart;
usart_param_struct computer_usart;

void communicate_param_init()
{
	//judgement_param_init
	judgement_usart.USARTx = USART2;
	
	judgement_usart.rx_DMAx = DMA1;
	judgement_usart.rx_Stream = LL_DMA_STREAM_5;
	judgement_usart.rx_dma_stream = DMA1_Stream5;

	judgement_usart.tx_DMAx = DMA1;
	judgement_usart.tx_Stream = LL_DMA_STREAM_6;
	judgement_usart.tx_dma_stream = DMA1_Stream6;
	
	judgement_usart.rx_usart_dma_memory_base_address_0 = (u32)judge_dma_rxbuff[0];
	judgement_usart.rx_usart_dma_memory_base_address_1 = (u32)judge_dma_rxbuff[1];
	judgement_usart.rx_usart_dma_buffer_size = DMA_BUFFER_SIZE;
	judgement_usart.task = &judge_unpack_task_t;
	judgement_usart.tx_finish_flag = 1;
	
        //computer_param_init
	computer_usart.USARTx = USART6;
	
	computer_usart.rx_DMAx = DMA2;
	computer_usart.rx_Stream = LL_DMA_STREAM_1;
	computer_usart.rx_dma_stream=DMA2_Stream1;

	computer_usart.tx_DMAx = DMA2;
	computer_usart.tx_Stream = LL_DMA_STREAM_6;
	computer_usart.tx_dma_stream=DMA2_Stream6;
	computer_usart.rx_usart_dma_memory_base_address_0 = (u32)pc_dma_rxbuff[0];
	computer_usart.rx_usart_dma_memory_base_address_1 = (u32)pc_dma_rxbuff[1];	
	computer_usart.rx_usart_dma_buffer_size = DMA_BUFFER_SIZE;
	computer_usart.task = &pc_unpack_task_t;
	computer_usart.tx_finish_flag = 1;
	

}



void computer_uart_init(void)
{
			/* create the pc_rxdata_mutex mutex  */
  osMutexDef(pc_rxdata_mutex);
  pc_rxdata_mutex = osMutexCreate(osMutex(pc_rxdata_mutex));
  
  /* create the pc_txdata_mutex mutex  */
  osMutexDef(pc_txdata_mutex);
  pc_txdata_mutex = osMutexCreate(osMutex(pc_txdata_mutex));
	
	/* pc data fifo init */
  fifo_s_init(&pc_rxdata_fifo, pc_rxdata_buf, COMPUTER_FIFO_BUFLEN, pc_rxdata_mutex);
  fifo_s_init(&pc_txdata_fifo, pc_txdata_buf, COMPUTER_FIFO_BUFLEN, pc_txdata_mutex);
	
	/* initial pc data dma receiver object */
  pc_rx_obj.dma_stream = computer_usart.rx_dma_stream;
  pc_rx_obj.data_fifo = &pc_rxdata_fifo;
  pc_rx_obj.buff_size = UART_RX_DMA_SIZE;
  pc_rx_obj.buff[0] = pc_dma_rxbuff[0];
  pc_rx_obj.buff[1] = pc_dma_rxbuff[1];
	
  /* initial pc data unpack object */ 
  pc_unpack_obj.data_fifo = &pc_rxdata_fifo;
  pc_unpack_obj.p_header = (frame_header_t *)pc_unpack_obj.protocol_packet;
  pc_unpack_obj.index = 0;
  pc_unpack_obj.data_len = 0;
  pc_unpack_obj.unpack_step = STEP_HEADER_SOF;
	usart_communicate_config(computer_usart);
}

void judgement_uart_init(void)
{
		/* create the judge_rxdata_mutex mutex  */
  osMutexDef(judge_rxdata_mutex);
  judge_rxdata_mutex = osMutexCreate(osMutex(judge_rxdata_mutex));
  
  /* create the judge_txdata_mutex mutex  */
  osMutexDef(judge_txdata_mutex);
  judge_txdata_mutex = osMutexCreate(osMutex(judge_txdata_mutex));
	
	/* judge data fifo init */
  fifo_s_init(&judge_rxdata_fifo, judge_rxdata_buf, JUDGE_FIFO_BUFLEN, judge_rxdata_mutex);
  fifo_s_init(&judge_txdata_fifo, judge_txdata_buf, JUDGE_FIFO_BUFLEN, judge_txdata_mutex);
	
	/* initial judge data dma receiver object */
  judge_rx_obj.dma_stream = judgement_usart.rx_dma_stream;
  judge_rx_obj.data_fifo = &judge_rxdata_fifo;
  judge_rx_obj.buff_size = UART_RX_DMA_SIZE;
  judge_rx_obj.buff[0] = judge_dma_rxbuff[0];
  judge_rx_obj.buff[1] = judge_dma_rxbuff[1];
	
  /* initial judge data unpack object */
  judge_unpack_obj.data_fifo = &judge_rxdata_fifo;
  judge_unpack_obj.p_header = (frame_header_t *)judge_unpack_obj.protocol_packet;
  judge_unpack_obj.index = 0;
  judge_unpack_obj.data_len = 0;
  judge_unpack_obj.unpack_step = STEP_HEADER_SOF;
	
	usart_communicate_config(judgement_usart);

}



//----------------------------------------------协议处理-----------------------------------------------------------


/** 
  * @brief  分数据，增加协议包改这里
  */
void pc_data_handler(uint8_t *p_frame)
{
  frame_header_t *p_header = (frame_header_t*)p_frame;
  memcpy(p_header, p_frame, HEADER_LEN);

  uint16_t data_length = p_header->data_length;
  uint16_t cmd_id      = *(uint16_t *)(p_frame + HEADER_LEN);
  uint8_t *data_addr   = p_frame + HEADER_LEN + CMD_LEN;

  taskENTER_CRITICAL();
  
  switch (cmd_id)
  {
		
		case CHASSIS_CTRL_ID:
      memcpy(&pc_recv_mesg.chassis_control_data, data_addr, data_length);
    break;
		
		case INFANTRY_BUFF_ID:
			memcpy(&pc_recv_mesg.gimbal_buff_data, data_addr, data_length);
			
		break;
		case INFANTRY_ENEMY_ID:
			memcpy(&pc_recv_mesg.gimbal_enemy_data, data_addr, data_length);

		break;
		case HERO_AUTO_SHOOT_ID:
			
		break;
		case ENGINEER_ID:
			
		break;
		case GUARD_AUTO_SHOOT_ID:
			
		break;
		case DRONE_AUTO_SHOOT_ID:
			
		break;
		case SUPPLY_ID:
			
		break;
		
		
  }
  
  taskEXIT_CRITICAL();
  
}




//裁判系统数据接收
/* data send (forward) */
/* data receive */
receive_judge_t judge_rece_mesg;

/**
  * @brief    get judgement system message
  */
extern TaskHandle_t pc_unpack_task_t;
void judgement_data_handler(uint8_t *p_frame)
{
  frame_header_t *p_header = (frame_header_t*)p_frame;
  memcpy(p_header, p_frame, HEADER_LEN);
	
  uint16_t data_length = p_header->data_length;
  uint16_t cmd_id      = *(uint16_t *)(p_frame + HEADER_LEN);
  uint8_t *data_addr   = p_frame + HEADER_LEN + CMD_LEN;
  
  switch (cmd_id)
  {
    case GAME_INFO_ID:
      memcpy(&judge_rece_mesg.game_information, data_addr, data_length);
    break;

    case REAL_BLOOD_DATA_ID:
      memcpy(&judge_rece_mesg.blood_changed_data, data_addr, data_length);
    break;

    case REAL_SHOOT_DATA_ID:
      memcpy(&judge_rece_mesg.real_shoot_data, data_addr, data_length);
    break;
		case REAL_POWER_DATA_ID:
			memcpy(&judge_rece_mesg.power_heat_data, data_addr, data_length);
		break;
    case FIELD_RFID_DATA_ID:
      memcpy(&judge_rece_mesg.rfid_data, data_addr, data_length);
    break;

    case GAME_RESULT_ID:
      memcpy(&judge_rece_mesg.game_result_data, data_addr, data_length);
    break;

    case GAIN_BUFF_ID:
      memcpy(&judge_rece_mesg.get_buff_data, data_addr, data_length);
    break;
    
    case ROBOT_POS_DATA_ID:
      memcpy(&judge_rece_mesg.robot_pos_data, data_addr, data_length);
    break;
    
    default:
    break;
  }
}


//----------------------------------------------打包与解包-----------------------------------------------------------
/** 
  * @brief  解包
  */
	
void unpack_fifo_data(unpack_data_t *p_obj, uint8_t sof)
{
  uint8_t byte = 0;
  
  while ( fifo_used_count(p_obj->data_fifo) )
  {
    byte = fifo_s_get(p_obj->data_fifo);
    switch(p_obj->unpack_step)
    {
      case STEP_HEADER_SOF:
      {
        if(byte == sof)
        {
          p_obj->unpack_step = STEP_LENGTH_LOW;
          p_obj->protocol_packet[p_obj->index++] = byte;
        }
        else
        {
          p_obj->index = 0;
        }
      }break;
      
      case STEP_LENGTH_LOW:
      {
        p_obj->data_len = byte;
        p_obj->protocol_packet[p_obj->index++] = byte;
        p_obj->unpack_step = STEP_LENGTH_HIGH;
      }break;
      
      case STEP_LENGTH_HIGH:
      {
        p_obj->data_len |= (byte << 8);
        p_obj->protocol_packet[p_obj->index++] = byte;

        if(p_obj->data_len < (PROTOCAL_FRAME_MAX_SIZE - HEADER_LEN - CRC_LEN))
        {
          p_obj->unpack_step = STEP_FRAME_SEQ;
        }
        else
        {
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;
        }
      }break;
    
      case STEP_FRAME_SEQ:
      {
        p_obj->protocol_packet[p_obj->index++] = byte;
        p_obj->unpack_step = STEP_HEADER_CRC8;
      }break;

      case STEP_HEADER_CRC8:
      {
        p_obj->protocol_packet[p_obj->index++] = byte;

        if (p_obj->index == HEADER_LEN)
        {
          if ( verify_crc8_check_sum(p_obj->protocol_packet, HEADER_LEN) )
          {
            p_obj->unpack_step = STEP_DATA_CRC16;
          }
          else
          {
            p_obj->unpack_step = STEP_HEADER_SOF;
            p_obj->index = 0;
          }
        }
      }break;  

      case STEP_DATA_CRC16:
      {
        if (p_obj->index < (HEADER_LEN + CMD_LEN + p_obj->data_len + CRC_LEN))
        {
           p_obj->protocol_packet[p_obj->index++] = byte;  
        }
        if (p_obj->index >= (HEADER_LEN + CMD_LEN + p_obj->data_len + CRC_LEN))
        {
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;

          if ( verify_crc16_check_sum(p_obj->protocol_packet, HEADER_LEN + CMD_LEN + p_obj->data_len + CRC_LEN) )
          {
            if (sof == UP_REG_ID)
            {
              pc_data_handler(p_obj->protocol_packet);
            }
            else  //DN_REG_ID
            {
              judgement_data_handler(p_obj->protocol_packet);
            }
          }
        }
      }break;

      default:
      {
        p_obj->unpack_step = STEP_HEADER_SOF;
        p_obj->index = 0;
      }break;
    }
  }
}

//for debug
int dma_write_len = 0;
int fifo_overflow = 0;

void dma_buffer_to_unpack_buffer(uart_dma_rxdata_t *dma_obj, uart_it_type_e it_type)
{
  int16_t  tmp_len;
  uint8_t  current_memory_id;
  uint16_t remain_data_counter;
  uint8_t  *pdata = dma_obj->buff[0];
  
  get_dma_memory_msg(dma_obj->dma_stream, &current_memory_id, &remain_data_counter);
  
  if (UART_IDLE_IT == it_type)
  {
    if (current_memory_id)
    {
      dma_obj->write_index = dma_obj->buff_size*2 - remain_data_counter;
    }
    else
    {
      dma_obj->write_index = dma_obj->buff_size - remain_data_counter;
    }
  }
  else if (UART_DMA_FULL_IT == it_type)
  {
#if 0
    if (current_memory_id)
    {
      dma_obj->write_index = dma_obj->buff_size;
    }
    else
    {
      dma_obj->write_index = dma_obj->buff_size*2;
    }
#endif
  }
  
  if (dma_obj->write_index < dma_obj->read_index)
  {
    dma_write_len = dma_obj->buff_size*2 - dma_obj->read_index + dma_obj->write_index;
    
    tmp_len = dma_obj->buff_size*2 - dma_obj->read_index;
    if (tmp_len != fifo_s_puts(dma_obj->data_fifo, &pdata[dma_obj->read_index], tmp_len))
      fifo_overflow = 1;
    else
      fifo_overflow = 0;
    dma_obj->read_index = 0;
    
    tmp_len = dma_obj->write_index;
    if (tmp_len != fifo_s_puts(dma_obj->data_fifo, &pdata[dma_obj->read_index], tmp_len))
      fifo_overflow = 1;
    else
      fifo_overflow = 0;
    dma_obj->read_index = dma_obj->write_index;
  }
  else
  {
    dma_write_len = dma_obj->write_index - dma_obj->read_index;
    
    tmp_len = dma_obj->write_index - dma_obj->read_index;
    if (tmp_len != fifo_s_puts(dma_obj->data_fifo, &pdata[dma_obj->read_index], tmp_len))
      fifo_overflow = 1;
    else
      fifo_overflow = 0;
    dma_obj->read_index = (dma_obj->write_index) % (dma_obj->buff_size*2);
  }
}


/** 
  * @brief  打包
  */

uint8_t* protocol_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof, uint8_t *tx_buf)
{
  //memset(tx_buf, 0, 100);
  //static uint8_t seq;
  
  uint16_t frame_length = HEADER_LEN + CMD_LEN + len + CRC_LEN;
  frame_header_t *p_header = (frame_header_t*)tx_buf;
  
  p_header->sof          = sof;
  p_header->data_length  = len;
  p_header->seq          = 0;
  //p_header->seq          = seq++;
  
  memcpy(&tx_buf[HEADER_LEN], (uint8_t*)&cmd_id, CMD_LEN);
  append_crc8_check_sum(tx_buf, HEADER_LEN);
  memcpy(&tx_buf[HEADER_LEN + CMD_LEN], p_data, len);
  append_crc16_check_sum(tx_buf, frame_length);
  
  return tx_buf;
}

uint32_t send_packed_fifo_data(fifo_s_t *pfifo, uint8_t sof)
{

#if (JUDGE_FIFO_BUFLEN > COMPUTER_FIFO_BUFLEN)
  uint8_t  tx_buf[JUDGE_FIFO_BUFLEN];
#else
  uint8_t  tx_buf[COMPUTER_FIFO_BUFLEN];
#endif
  uint32_t fifo_count = fifo_used_count(pfifo);
  
  if (fifo_count)
  {
    fifo_s_gets(pfifo, tx_buf, fifo_count);
    
    if (sof == UP_REG_ID)
			usart_dma_send(&computer_usart, (u32)tx_buf, fifo_count);
    else if (sof == DN_REG_ID)
			usart_dma_send(&judgement_usart, (u32)tx_buf, fifo_count);
    else
      return 0;
  }
  
  return fifo_count;
}

  uint8_t tx_buf[PROTOCAL_FRAME_MAX_SIZE];

void data_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof)
{
  
  uint16_t frame_length = HEADER_LEN + CMD_LEN + len + CRC_LEN;
  
  protocol_packet_pack(cmd_id, p_data, len, sof, tx_buf);
  
  /* use mutex operation */
  if (sof == UP_REG_ID)
    fifo_s_puts(&pc_txdata_fifo, tx_buf, frame_length);
  else if (sof == DN_REG_ID)
    fifo_s_puts(&judge_txdata_fifo, tx_buf, frame_length);
  else
    return ;
}




//----------------------------------------------Task-----------------------------------------------------------

void pc_unpack_task(void const *argu)
{
  osEvent event;
  
//  taskENTER_CRITICAL();
  /* open pc uart receive it */
  computer_uart_init();
  /* create periodic information task */
//  taskEXIT_CRITICAL();
  
  while (1)
  {
    event = osSignalWait(UART_IDLE_SIGNAL|UART_TX_SIGNAL , osWaitForever);
    		
    if (event.status == osEventSignal)
    {
      //receive pc data puts fifo
      if (event.value.signals & UART_IDLE_SIGNAL)
      {
        dma_buffer_to_unpack_buffer(&pc_rx_obj, UART_IDLE_IT);
        unpack_fifo_data(&pc_unpack_obj, UP_REG_ID);
      }
			
			if (event.value.signals & UART_TX_SIGNAL)
      {
        send_packed_fifo_data(&pc_txdata_fifo, UP_REG_ID);
      }
    }
    
  }
}

void judge_unpack_task(void const *argu)
{
  osEvent event;
  
  /* open judge uart receive it */
	judgement_uart_init();
  
  while (1)
  {
    event = osSignalWait(UART_TX_SIGNAL | UART_IDLE_SIGNAL, osWaitForever);
    
    if (event.status == osEventSignal)
    {
      //receive judge data puts fifo
      if (event.value.signals & UART_IDLE_SIGNAL)
      {
        dma_buffer_to_unpack_buffer(&judge_rx_obj, UART_IDLE_IT);
        unpack_fifo_data(&judge_unpack_obj, DN_REG_ID);
      }
      
      //send data to judge system
      if (event.value.signals & UART_TX_SIGNAL)
      {
        send_packed_fifo_data(&judge_txdata_fifo, DN_REG_ID);
      }
      
    }
  }
}

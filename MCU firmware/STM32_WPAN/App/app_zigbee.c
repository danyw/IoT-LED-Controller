
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    App/app_zigbee.c
  * @author  MCD Application Team
  * @brief   Zigbee Application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "app_entry.h"
#include "dbg_trace.h"
#include "app_zigbee.h"
#include "zigbee_interface.h"
#include "shci.h"
#include "stm_logging.h"
#include "app_conf.h"
#include "stm32wbxx_core_interface_def.h"
#include "zigbee_types.h"
#include "stm32_seq.h"

/* Private includes -----------------------------------------------------------*/
#include <assert.h>
#include "zcl/zcl.h"
#include "zcl/general/zcl.identify.h"
#include "zcl/general/zcl.onoff.h"
#include "zcl/general/zcl.level.h"

/* USER CODE BEGIN Includes */
#include "main.h"
#include "led_control.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_STARTUP_FAIL_DELAY               500U
#define CHANNEL                                     12

#define SW1_ENDPOINT                                1
#define SW2_ENDPOINT                                2
#define SW3_ENDPOINT                                3
#define SW4_ENDPOINT                                4
#define SW5_ENDPOINT                                5
#define SW6_ENDPOINT                                6
#define SW7_ENDPOINT                                7

/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* External definition -------------------------------------------------------*/
enum ZbStatusCodeT ZbStartupWait(struct ZigBeeT *zb, struct ZbStartupT *config);

/* USER CODE BEGIN ED */
/* USER CODE END ED */

/* Private function prototypes -----------------------------------------------*/
static void APP_ZIGBEE_StackLayersInit(void);
static void APP_ZIGBEE_ConfigEndpoints(void);
static void APP_ZIGBEE_NwkForm(void);

static void APP_ZIGBEE_TraceError(const char *pMess, uint32_t ErrCode);
static void APP_ZIGBEE_CheckWirelessFirmwareInfo(void);

static void Wait_Getting_Ack_From_M0(void);
static void Receive_Ack_From_M0(void);
static void Receive_Notification_From_M0(void);

static void APP_ZIGBEE_ProcessNotifyM0ToM4(void);
static void APP_ZIGBEE_ProcessRequestM0ToM4(void);

/* USER CODE BEGIN PFP */
static uint16_t scaledBrightness(uint16_t x) {
	uint16_t scaledBrightness = (uint16_t)(((float) x / 254.0) * 1000.0);
	return scaledBrightness;
}
/* USER CODE END PFP */

/* Private variables ---------------------------------------------------------*/
static TL_CmdPacket_t   *p_ZIGBEE_otcmdbuffer;
static TL_EvtPacket_t   *p_ZIGBEE_notif_M0_to_M4;
static TL_EvtPacket_t   *p_ZIGBEE_request_M0_to_M4;
static __IO uint32_t    CptReceiveNotifyFromM0 = 0;
static __IO uint32_t    CptReceiveRequestFromM0 = 0;

PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static TL_ZIGBEE_Config_t ZigbeeConfigBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t ZigbeeOtCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ZigbeeNotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ZigbeeNotifRequestBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];

struct zigbee_app_info
{
  bool has_init;
  struct ZigBeeT *zb;
  enum ZbStartType startupControl;
  enum ZbStatusCodeT join_status;
  uint32_t join_delay;
  bool init_after_join;

  struct ZbZclClusterT *identify_server_1;
  struct ZbZclClusterT *onOff_server_1;
  struct ZbZclClusterT *onOff_server_2;
  struct ZbZclClusterT *levelControl_server_2;
  struct ZbZclClusterT *onOff_server_3;
  struct ZbZclClusterT *levelControl_server_3;
  struct ZbZclClusterT *onOff_server_4;
  struct ZbZclClusterT *levelControl_server_4;
  struct ZbZclClusterT *onOff_server_5;
  struct ZbZclClusterT *levelControl_server_5;
  struct ZbZclClusterT *onOff_server_6;
  struct ZbZclClusterT *levelControl_server_6;
  struct ZbZclClusterT *onOff_server_7;
  struct ZbZclClusterT *levelControl_server_7;
};
static struct zigbee_app_info zigbee_app_info;

/* OnOff server 1 custom callbacks */
static enum ZclStatusCodeT onOff_server_1_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_1_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclOnOffServerCallbacksT OnOffServerCallbacks_1 =
{
  .off = onOff_server_1_off,
  .on = onOff_server_1_on,
};

/* OnOff server 2 custom callbacks */
static enum ZclStatusCodeT onOff_server_2_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_2_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclOnOffServerCallbacksT OnOffServerCallbacks_2 =
{
  .off = onOff_server_2_off,
  .on = onOff_server_2_on,
};

/* LevelControl server 2 custom callbacks */
static enum ZclStatusCodeT levelControl_server_2_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_2_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_2_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_2_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclLevelServerCallbacksT LevelServerCallbacks_2 =
{
  .move_to_level = levelControl_server_2_move_to_level,
  .move = levelControl_server_2_move,
  .step = levelControl_server_2_step,
  .stop = levelControl_server_2_stop,
};

/* OnOff server 3 custom callbacks */
static enum ZclStatusCodeT onOff_server_3_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_3_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclOnOffServerCallbacksT OnOffServerCallbacks_3 =
{
  .off = onOff_server_3_off,
  .on = onOff_server_3_on,
};

/* LevelControl server 3 custom callbacks */
static enum ZclStatusCodeT levelControl_server_3_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_3_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_3_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_3_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclLevelServerCallbacksT LevelServerCallbacks_3 =
{
  .move_to_level = levelControl_server_3_move_to_level,
  .move = levelControl_server_3_move,
  .step = levelControl_server_3_step,
  .stop = levelControl_server_3_stop,
};

/* OnOff server 4 custom callbacks */
static enum ZclStatusCodeT onOff_server_4_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_4_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclOnOffServerCallbacksT OnOffServerCallbacks_4 =
{
  .off = onOff_server_4_off,
  .on = onOff_server_4_on,
};

/* LevelControl server 4 custom callbacks */
static enum ZclStatusCodeT levelControl_server_4_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_4_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_4_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_4_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclLevelServerCallbacksT LevelServerCallbacks_4 =
{
  .move_to_level = levelControl_server_4_move_to_level,
  .move = levelControl_server_4_move,
  .step = levelControl_server_4_step,
  .stop = levelControl_server_4_stop,
};

/* OnOff server 5 custom callbacks */
static enum ZclStatusCodeT onOff_server_5_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_5_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclOnOffServerCallbacksT OnOffServerCallbacks_5 =
{
  .off = onOff_server_5_off,
  .on = onOff_server_5_on,
};

/* LevelControl server 5 custom callbacks */
static enum ZclStatusCodeT levelControl_server_5_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_5_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_5_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_5_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclLevelServerCallbacksT LevelServerCallbacks_5 =
{
  .move_to_level = levelControl_server_5_move_to_level,
  .move = levelControl_server_5_move,
  .step = levelControl_server_5_step,
  .stop = levelControl_server_5_stop,
};

/* OnOff server 6 custom callbacks */
static enum ZclStatusCodeT onOff_server_6_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_6_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_6_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclOnOffServerCallbacksT OnOffServerCallbacks_6 =
{
  .off = onOff_server_6_off,
  .on = onOff_server_6_on,
  .toggle = onOff_server_6_toggle,
};

/* LevelControl server 6 custom callbacks */
static enum ZclStatusCodeT levelControl_server_6_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_6_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_6_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_6_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclLevelServerCallbacksT LevelServerCallbacks_6 =
{
  .move_to_level = levelControl_server_6_move_to_level,
  .move = levelControl_server_6_move,
  .step = levelControl_server_6_step,
  .stop = levelControl_server_6_stop,
};

/* OnOff server 7 custom callbacks */
static enum ZclStatusCodeT onOff_server_7_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_7_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclOnOffServerCallbacksT OnOffServerCallbacks_7 =
{
  .off = onOff_server_7_off,
  .on = onOff_server_7_on,
};

/* LevelControl server 7 custom callbacks */
static enum ZclStatusCodeT levelControl_server_7_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_7_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_7_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_7_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclLevelServerCallbacksT LevelServerCallbacks_7 =
{
  .move_to_level = levelControl_server_7_move_to_level,
  .move = levelControl_server_7_move,
  .step = levelControl_server_7_step,
  .stop = levelControl_server_7_stop,
};

/* USER CODE BEGIN PV */
/* USER CODE END PV */
/* Functions Definition ------------------------------------------------------*/

/* OnOff server off 1 command callback */
static enum ZclStatusCodeT onOff_server_1_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 0 OnOff server 1 off 1 */
	led_enable(GPIO_PIN_RESET);
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 0 OnOff server 1 off 1 */
}

/* OnOff server on 1 command callback */
static enum ZclStatusCodeT onOff_server_1_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 1 OnOff server 1 on 1 */
	led_enable(GPIO_PIN_SET);

  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 1 OnOff server 1 on 1 */
}

/* OnOff server off 2 command callback */
static enum ZclStatusCodeT onOff_server_2_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 2 OnOff server 2 off 2 */
	 set_brightness_single(1, 0);
	 SystemSettings.Pwm_on[0] = false;
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 2 OnOff server 2 off 2 */
}

/* OnOff server on 2 command callback */
static enum ZclStatusCodeT onOff_server_2_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 3 OnOff server 2 on 2 */
	turn_on_single(0);

  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 3 OnOff server 2 on 2 */
}

/* LevelControl server move_to_level 2 command callback */
static enum ZclStatusCodeT levelControl_server_2_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 4 LevelControl server 2 move_to_level 2 */
//	  uint16_t scaledBrightness = (uint16_t)(((float)req->level / 254.0) * 1000.0);


	set_brightness_single(1, scaledBrightness(req->level));
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 4 LevelControl server 2 move_to_level 2 */
}

/* LevelControl server move 2 command callback */
static enum ZclStatusCodeT levelControl_server_2_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 5 LevelControl server 2 move 2 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 5 LevelControl server 2 move 2 */
}

/* LevelControl server step 2 command callback */
static enum ZclStatusCodeT levelControl_server_2_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 6 LevelControl server 2 step 2 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 6 LevelControl server 2 step 2 */
}

/* LevelControl server stop 2 command callback */
static enum ZclStatusCodeT levelControl_server_2_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 7 LevelControl server 2 stop 2 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 7 LevelControl server 2 stop 2 */
}

/* OnOff server off 3 command callback */
static enum ZclStatusCodeT onOff_server_3_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 8 OnOff server 3 off 3 */
	 set_brightness_single(2, 0);
	 SystemSettings.Pwm_on[1] = false;
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 8 OnOff server 3 off 3 */
}

/* OnOff server on 3 command callback */
static enum ZclStatusCodeT onOff_server_3_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 9 OnOff server 3 on 3 */
	turn_on_single(1);
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 9 OnOff server 3 on 3 */
}

/* LevelControl server move_to_level 3 command callback */
static enum ZclStatusCodeT levelControl_server_3_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 10 LevelControl server 3 move_to_level 3 */
	set_brightness_single(2, scaledBrightness(req->level));
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 10 LevelControl server 3 move_to_level 3 */
}

/* LevelControl server move 3 command callback */
static enum ZclStatusCodeT levelControl_server_3_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 11 LevelControl server 3 move 3 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 11 LevelControl server 3 move 3 */
}

/* LevelControl server step 3 command callback */
static enum ZclStatusCodeT levelControl_server_3_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 12 LevelControl server 3 step 3 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 12 LevelControl server 3 step 3 */
}

/* LevelControl server stop 3 command callback */
static enum ZclStatusCodeT levelControl_server_3_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 13 LevelControl server 3 stop 3 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 13 LevelControl server 3 stop 3 */
}

/* OnOff server off 4 command callback */
static enum ZclStatusCodeT onOff_server_4_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 14 OnOff server 4 off 4 */
	 set_brightness_single(3, 0);
	 SystemSettings.Pwm_on[2] = false;
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 14 OnOff server 4 off 4 */
}

/* OnOff server on 4 command callback */
static enum ZclStatusCodeT onOff_server_4_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 15 OnOff server 4 on 4 */
	turn_on_single(2);
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 15 OnOff server 4 on 4 */
}

/* LevelControl server move_to_level 4 command callback */
static enum ZclStatusCodeT levelControl_server_4_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 16 LevelControl server 4 move_to_level 4 */
	set_brightness_single(3, scaledBrightness(req->level));
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 16 LevelControl server 4 move_to_level 4 */
}

/* LevelControl server move 4 command callback */
static enum ZclStatusCodeT levelControl_server_4_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 17 LevelControl server 4 move 4 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 17 LevelControl server 4 move 4 */
}

/* LevelControl server step 4 command callback */
static enum ZclStatusCodeT levelControl_server_4_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 18 LevelControl server 4 step 4 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 18 LevelControl server 4 step 4 */
}

/* LevelControl server stop 4 command callback */
static enum ZclStatusCodeT levelControl_server_4_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 19 LevelControl server 4 stop 4 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 19 LevelControl server 4 stop 4 */
}

/* OnOff server off 5 command callback */
static enum ZclStatusCodeT onOff_server_5_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 20 OnOff server 5 off 5 */
	 set_brightness_single(4, 0);
	 SystemSettings.Pwm_on[3] = false;
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 20 OnOff server 5 off 5 */
}

/* OnOff server on 5 command callback */
static enum ZclStatusCodeT onOff_server_5_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 21 OnOff server 5 on 5 */
	turn_on_single(3);
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 21 OnOff server 5 on 5 */
}

/* LevelControl server move_to_level 5 command callback */
static enum ZclStatusCodeT levelControl_server_5_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 22 LevelControl server 5 move_to_level 5 */
	set_brightness_single(4, scaledBrightness(req->level));
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 22 LevelControl server 5 move_to_level 5 */
}

/* LevelControl server move 5 command callback */
static enum ZclStatusCodeT levelControl_server_5_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 23 LevelControl server 5 move 5 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 23 LevelControl server 5 move 5 */
}

/* LevelControl server step 5 command callback */
static enum ZclStatusCodeT levelControl_server_5_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 24 LevelControl server 5 step 5 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 24 LevelControl server 5 step 5 */
}

/* LevelControl server stop 5 command callback */
static enum ZclStatusCodeT levelControl_server_5_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 25 LevelControl server 5 stop 5 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 25 LevelControl server 5 stop 5 */
}

/* OnOff server off 6 command callback */
static enum ZclStatusCodeT onOff_server_6_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 26 OnOff server 6 off 6 */
	 set_brightness_single(5, 0);
	 SystemSettings.Pwm_on[4] = false;
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 26 OnOff server 6 off 6 */
}

/* OnOff server on 6 command callback */
static enum ZclStatusCodeT onOff_server_6_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 27 OnOff server 6 on 6 */
	turn_on_single(4);
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 27 OnOff server 6 on 6 */
}

/* OnOff server toggle 6 command callback */
static enum ZclStatusCodeT onOff_server_6_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 28 OnOff server 6 toggle 6 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 28 OnOff server 6 toggle 6 */
}

/* LevelControl server move_to_level 6 command callback */
static enum ZclStatusCodeT levelControl_server_6_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 29 LevelControl server 6 move_to_level 6 */
	set_brightness_single(5, scaledBrightness(req->level));
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 29 LevelControl server 6 move_to_level 6 */
}

/* LevelControl server move 6 command callback */
static enum ZclStatusCodeT levelControl_server_6_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 30 LevelControl server 6 move 6 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 30 LevelControl server 6 move 6 */
}

/* LevelControl server step 6 command callback */
static enum ZclStatusCodeT levelControl_server_6_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 31 LevelControl server 6 step 6 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 31 LevelControl server 6 step 6 */
}

/* LevelControl server stop 6 command callback */
static enum ZclStatusCodeT levelControl_server_6_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 32 LevelControl server 6 stop 6 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 32 LevelControl server 6 stop 6 */
}

/* OnOff server off 7 command callback */
static enum ZclStatusCodeT onOff_server_7_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 33 OnOff server 7 off 7 */
	 set_brightness_single(6, 0);
	 SystemSettings.Pwm_on[5] = false;
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 33 OnOff server 7 off 7 */
}

/* OnOff server on 7 command callback */
static enum ZclStatusCodeT onOff_server_7_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 34 OnOff server 7 on 7 */
	turn_on_single(5);
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 34 OnOff server 7 on 7 */
}

/* LevelControl server move_to_level 7 command callback */
static enum ZclStatusCodeT levelControl_server_7_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 35 LevelControl server 7 move_to_level 7 */
	set_brightness_single(6, scaledBrightness(req->level));
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 35 LevelControl server 7 move_to_level 7 */
}

/* LevelControl server move 7 command callback */
static enum ZclStatusCodeT levelControl_server_7_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 36 LevelControl server 7 move 7 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 36 LevelControl server 7 move 7 */
}

/* LevelControl server step 7 command callback */
static enum ZclStatusCodeT levelControl_server_7_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 37 LevelControl server 7 step 7 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 37 LevelControl server 7 step 7 */
}

/* LevelControl server stop 7 command callback */
static enum ZclStatusCodeT levelControl_server_7_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 38 LevelControl server 7 stop 7 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 38 LevelControl server 7 stop 7 */
}

/**
 * @brief  Zigbee application initialization
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_Init(void)
{
  SHCI_CmdStatus_t ZigbeeInitStatus;

  APP_DBG("APP_ZIGBEE_Init");

  /* Check the compatibility with the Coprocessor Wireless Firmware loaded */
  APP_ZIGBEE_CheckWirelessFirmwareInfo();

  /* Register cmdbuffer */
  APP_ZIGBEE_RegisterCmdBuffer(&ZigbeeOtCmdBuffer);

  /* Init config buffer and call TL_ZIGBEE_Init */
  APP_ZIGBEE_TL_INIT();

  /* Register task */
  /* Create the different tasks */
  UTIL_SEQ_RegTask(1U << (uint32_t)CFG_TASK_NOTIFY_FROM_M0_TO_M4, UTIL_SEQ_RFU, APP_ZIGBEE_ProcessNotifyM0ToM4);
  UTIL_SEQ_RegTask(1U << (uint32_t)CFG_TASK_REQUEST_FROM_M0_TO_M4, UTIL_SEQ_RFU, APP_ZIGBEE_ProcessRequestM0ToM4);

  /* Task associated with network creation process */
  UTIL_SEQ_RegTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, UTIL_SEQ_RFU, APP_ZIGBEE_NwkForm);

  /* USER CODE BEGIN APP_ZIGBEE_INIT */
  /* USER CODE END APP_ZIGBEE_INIT */

  /* Start the Zigbee on the CPU2 side */
  ZigbeeInitStatus = SHCI_C2_ZIGBEE_Init();
  /* Prevent unused argument(s) compilation warning */
  UNUSED(ZigbeeInitStatus);

  /* Initialize Zigbee stack layers */
  APP_ZIGBEE_StackLayersInit();

}

/**
 * @brief  Initialize Zigbee stack layers
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_StackLayersInit(void)
{
  APP_DBG("APP_ZIGBEE_StackLayersInit");

  zigbee_app_info.zb = ZbInit(0U, NULL, NULL);
  assert(zigbee_app_info.zb != NULL);

  /* Create the endpoint and cluster(s) */
  APP_ZIGBEE_ConfigEndpoints();

  /* USER CODE BEGIN APP_ZIGBEE_StackLayersInit */
  /* USER CODE END APP_ZIGBEE_StackLayersInit */

  /* Configure the joining parameters */
  zigbee_app_info.join_status = (enum ZbStatusCodeT) 0x01; /* init to error status */
  zigbee_app_info.join_delay = HAL_GetTick(); /* now */
  zigbee_app_info.startupControl = ZbStartTypeJoin;

  /* Initialization Complete */
  zigbee_app_info.has_init = true;

  /* run the task */
  UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, CFG_SCH_PRIO_0);
}

/**
 * @brief  Configure Zigbee application endpoints
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ConfigEndpoints(void)
{
  struct ZbApsmeAddEndpointReqT req;
  struct ZbApsmeAddEndpointConfT conf;

  memset(&req, 0, sizeof(req));

  /* Endpoint: SW1_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_CONFIG_TOOL;
  req.endpoint = SW1_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* Identify server */
  zigbee_app_info.identify_server_1 = ZbZclIdentifyServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, NULL);
  assert(zigbee_app_info.identify_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.identify_server_1);
  /* OnOff server */
  zigbee_app_info.onOff_server_1 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, &OnOffServerCallbacks_1, NULL);
  assert(zigbee_app_info.onOff_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_1);
  /* Endpoint: SW2_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_DIMMABLE_LIGHT;
  req.endpoint = SW2_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onOff_server_2 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW2_ENDPOINT, &OnOffServerCallbacks_2, NULL);
  assert(zigbee_app_info.onOff_server_2 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_2);
  /* LevelControl server */
  zigbee_app_info.levelControl_server_2 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW2_ENDPOINT, zigbee_app_info.onOff_server_2, &LevelServerCallbacks_2, NULL);
  assert(zigbee_app_info.levelControl_server_2 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_2);
  /* Endpoint: SW3_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_DIMMABLE_LIGHT;
  req.endpoint = SW3_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onOff_server_3 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW3_ENDPOINT, &OnOffServerCallbacks_3, NULL);
  assert(zigbee_app_info.onOff_server_3 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_3);
  /* LevelControl server */
  zigbee_app_info.levelControl_server_3 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW3_ENDPOINT, zigbee_app_info.onOff_server_3, &LevelServerCallbacks_3, NULL);
  assert(zigbee_app_info.levelControl_server_3 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_3);
  /* Endpoint: SW4_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_DIMMABLE_LIGHT;
  req.endpoint = SW4_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onOff_server_4 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW4_ENDPOINT, &OnOffServerCallbacks_4, NULL);
  assert(zigbee_app_info.onOff_server_4 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_4);
  /* LevelControl server */
  zigbee_app_info.levelControl_server_4 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW4_ENDPOINT, zigbee_app_info.onOff_server_4, &LevelServerCallbacks_4, NULL);
  assert(zigbee_app_info.levelControl_server_4 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_4);
  /* Endpoint: SW5_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_DIMMABLE_LIGHT;
  req.endpoint = SW5_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onOff_server_5 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW5_ENDPOINT, &OnOffServerCallbacks_5, NULL);
  assert(zigbee_app_info.onOff_server_5 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_5);
  /* LevelControl server */
  zigbee_app_info.levelControl_server_5 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW5_ENDPOINT, zigbee_app_info.onOff_server_5, &LevelServerCallbacks_5, NULL);
  assert(zigbee_app_info.levelControl_server_5 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_5);
  /* Endpoint: SW6_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_DIMMABLE_LIGHT;
  req.endpoint = SW6_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onOff_server_6 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW6_ENDPOINT, &OnOffServerCallbacks_6, NULL);
  assert(zigbee_app_info.onOff_server_6 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_6);
  /* LevelControl server */
  zigbee_app_info.levelControl_server_6 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW6_ENDPOINT, zigbee_app_info.onOff_server_6, &LevelServerCallbacks_6, NULL);
  assert(zigbee_app_info.levelControl_server_6 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_6);
  /* Endpoint: SW7_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_DIMMABLE_LIGHT;
  req.endpoint = SW7_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* OnOff server */
  zigbee_app_info.onOff_server_7 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW7_ENDPOINT, &OnOffServerCallbacks_7, NULL);
  assert(zigbee_app_info.onOff_server_7 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_7);
  /* LevelControl server */
  zigbee_app_info.levelControl_server_7 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW7_ENDPOINT, zigbee_app_info.onOff_server_7, &LevelServerCallbacks_7, NULL);
  assert(zigbee_app_info.levelControl_server_7 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_7);

  /* USER CODE BEGIN CONFIG_ENDPOINT */
//  // Configure endpoints from SW2_ENDPOINT to SW7_ENDPOINT
//  for (uint8_t endpoint = SW2_ENDPOINT; endpoint <= SW7_ENDPOINT; endpoint++) {
//	// Allocate OnOff server
//	struct ZbZclClusterT *onOffServer = ZbZclOnOffServerAlloc(zigbee_app_info.zb, endpoint, NULL, NULL);
//	assert(onOffServer != NULL);
//	ZbZclClusterEndpointRegister(onOffServer);
//
//	// Allocate LevelControl server
//	struct ZbZclClusterT *levelControlServer = ZbZclLevelServerAlloc(zigbee_app_info.zb, endpoint, onOffServer, NULL, NULL);
//	assert(levelControlServer != NULL);
//	ZbZclClusterEndpointRegister(levelControlServer);
//
//	// Set minimum and maximum level for Level Control server
//	ZbZclAttrIntegerWrite(levelControlServer, ZCL_LEVEL_MINIMUM_LEVEL, 0);    // Minimum level
//	ZbZclAttrIntegerWrite(levelControlServer, ZCL_LEVEL_MAXIMUM_LEVEL, 1000); // Maximum level
//  }
  /* USER CODE END CONFIG_ENDPOINT */
}

/**
 * @brief  Handle Zigbee network forming and joining
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_NwkForm(void)
{
  if ((zigbee_app_info.join_status != ZB_STATUS_SUCCESS) && (HAL_GetTick() >= zigbee_app_info.join_delay))
  {
    struct ZbStartupT config;
    enum ZbStatusCodeT status;

    /* Configure Zigbee Logging */
    ZbSetLogging(zigbee_app_info.zb, ZB_LOG_MASK_LEVEL_5, NULL);

    /* Attempt to join a zigbee network */
    ZbStartupConfigGetProDefaults(&config);

    /* Set the centralized network */
    APP_DBG("Network config : APP_STARTUP_CENTRALIZED_ROUTER");
    config.startupControl = zigbee_app_info.startupControl;

    /* Using the default HA preconfigured Link Key */
    memcpy(config.security.preconfiguredLinkKey, sec_key_ha, ZB_SEC_KEYSIZE);

    config.channelList.count = 1;
    config.channelList.list[0].page = 0;
    config.channelList.list[0].channelMask = 1 << CHANNEL; /*Channel in use */

    /* Using ZbStartupWait (blocking) */
    status = ZbStartupWait(zigbee_app_info.zb, &config);

    APP_DBG("ZbStartup Callback (status = 0x%02x)", status);
    zigbee_app_info.join_status = status;

    if (status == ZB_STATUS_SUCCESS)
    {
      zigbee_app_info.join_delay = 0U;
      zigbee_app_info.init_after_join = true;
      APP_DBG("Startup done !\n");
      /* USER CODE BEGIN 39 */

      /* USER CODE END 39 */
    }
    else
    {
      APP_DBG("Startup failed, attempting again after a short delay (%d ms)", APP_ZIGBEE_STARTUP_FAIL_DELAY);
      zigbee_app_info.join_delay = HAL_GetTick() + APP_ZIGBEE_STARTUP_FAIL_DELAY;
      /* USER CODE BEGIN 40 */

      /* USER CODE END 40 */
    }
  }

  /* If Network forming/joining was not successful reschedule the current task to retry the process */
  if (zigbee_app_info.join_status != ZB_STATUS_SUCCESS)
  {
    UTIL_SEQ_SetTask(1U << CFG_TASK_ZIGBEE_NETWORK_FORM, CFG_SCH_PRIO_0);
  }
  /* USER CODE BEGIN NW_FORM */
  /* USER CODE END NW_FORM */
}

/*************************************************************
 * ZbStartupWait Blocking Call
 *************************************************************/
struct ZbStartupWaitInfo
{
  bool active;
  enum ZbStatusCodeT status;
};

static void ZbStartupWaitCb(enum ZbStatusCodeT status, void *cb_arg)
{
  struct ZbStartupWaitInfo *info = cb_arg;

  info->status = status;
  info->active = false;
  UTIL_SEQ_SetEvt(EVENT_ZIGBEE_STARTUP_ENDED);
}

enum ZbStatusCodeT ZbStartupWait(struct ZigBeeT *zb, struct ZbStartupT *config)
{
  struct ZbStartupWaitInfo *info;
  enum ZbStatusCodeT status;

  info = malloc(sizeof(struct ZbStartupWaitInfo));
  if (info == NULL)
  {
    return ZB_STATUS_ALLOC_FAIL;
  }
  memset(info, 0, sizeof(struct ZbStartupWaitInfo));

  info->active = true;
  status = ZbStartup(zb, config, ZbStartupWaitCb, info);
  if (status != ZB_STATUS_SUCCESS)
  {
    free(info);
    return status;
  }

  UTIL_SEQ_WaitEvt(EVENT_ZIGBEE_STARTUP_ENDED);
  status = info->status;
  free(info);
  return status;
}

/**
 * @brief  Trace the error or the warning reported.
 * @param  ErrId :
 * @param  ErrCode
 * @retval None
 */
void APP_ZIGBEE_Error(uint32_t ErrId, uint32_t ErrCode)
{
  switch (ErrId)
  {
    default:
      APP_ZIGBEE_TraceError("ERROR Unknown ", 0);
      break;
  }
}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/**
 * @brief  Warn the user that an error has occurred.
 *
 * @param  pMess  : Message associated to the error.
 * @param  ErrCode: Error code associated to the module (Zigbee or other module if any)
 * @retval None
 */
static void APP_ZIGBEE_TraceError(const char *pMess, uint32_t ErrCode)
{
  APP_DBG("**** Fatal error = %s (Err = %d)", pMess, ErrCode);
  /* USER CODE BEGIN TRACE_ERROR */
  /* USER CODE END TRACE_ERROR */

}

/**
 * @brief Check if the Coprocessor Wireless Firmware loaded supports Zigbee
 *        and display associated information
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_CheckWirelessFirmwareInfo(void)
{
  WirelessFwInfo_t wireless_info_instance;
  WirelessFwInfo_t *p_wireless_info = &wireless_info_instance;

  if (SHCI_GetWirelessFwInfo(p_wireless_info) != SHCI_Success)
  {
    APP_ZIGBEE_Error((uint32_t)ERR_ZIGBEE_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
  }
  else
  {
    APP_DBG("**********************************************************");
    APP_DBG("WIRELESS COPROCESSOR FW:");
    /* Print version */
    APP_DBG("VERSION ID = %d.%d.%d", p_wireless_info->VersionMajor, p_wireless_info->VersionMinor, p_wireless_info->VersionSub);

    switch (p_wireless_info->StackType)
    {
      case INFO_STACK_TYPE_ZIGBEE_FFD:
        APP_DBG("FW Type : FFD Zigbee stack");
        break;

      case INFO_STACK_TYPE_ZIGBEE_RFD:
        APP_DBG("FW Type : RFD Zigbee stack");
        break;

      default:
        /* No Zigbee device supported ! */
        APP_ZIGBEE_Error((uint32_t)ERR_ZIGBEE_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
        break;
    }

    /* print the application name */
    char *__PathProject__ = (strstr(__FILE__, "Zigbee") ? strstr(__FILE__, "Zigbee") + 7 : __FILE__);
    char *pdel = NULL;
    if((strchr(__FILE__, '/')) == NULL)
    {
      pdel = strchr(__PathProject__, '\\');
    }
    else
    {
      pdel = strchr(__PathProject__, '/');
    }

    int index = (int)(pdel - __PathProject__);
    APP_DBG("Application flashed: %*.*s", index, index, __PathProject__);

    /* print channel */
    APP_DBG("Channel used: %d", CHANNEL);
    /* print Link Key */
    APP_DBG("Link Key: %.16s", sec_key_ha);
    /* print Link Key value hex */
    char Z09_LL_string[ZB_SEC_KEYSIZE*3+1];
    Z09_LL_string[0] = 0;
    for (int str_index = 0; str_index < ZB_SEC_KEYSIZE; str_index++)
    {
      sprintf(&Z09_LL_string[str_index*3], "%02x ", sec_key_ha[str_index]);
    }

    APP_DBG("Link Key value: %s", Z09_LL_string);
    /* print clusters allocated */
    APP_DBG("Clusters allocated are:");
    APP_DBG("identify Server on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("onOff Server on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("onOff Server on Endpoint %d", SW2_ENDPOINT);
    APP_DBG("levelControl Server on Endpoint %d", SW2_ENDPOINT);
    APP_DBG("onOff Server on Endpoint %d", SW3_ENDPOINT);
    APP_DBG("levelControl Server on Endpoint %d", SW3_ENDPOINT);
    APP_DBG("onOff Server on Endpoint %d", SW4_ENDPOINT);
    APP_DBG("levelControl Server on Endpoint %d", SW4_ENDPOINT);
    APP_DBG("onOff Server on Endpoint %d", SW5_ENDPOINT);
    APP_DBG("levelControl Server on Endpoint %d", SW5_ENDPOINT);
    APP_DBG("onOff Server on Endpoint %d", SW6_ENDPOINT);
    APP_DBG("levelControl Server on Endpoint %d", SW6_ENDPOINT);
    APP_DBG("onOff Server on Endpoint %d", SW7_ENDPOINT);
    APP_DBG("levelControl Server on Endpoint %d", SW7_ENDPOINT);
    APP_DBG("**********************************************************");
  }
}

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

void APP_ZIGBEE_RegisterCmdBuffer(TL_CmdPacket_t *p_buffer)
{
  p_ZIGBEE_otcmdbuffer = p_buffer;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_OTCmdPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)p_ZIGBEE_otcmdbuffer->cmdserial.cmd.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_OTCmdRspPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)((TL_EvtPacket_t *)p_ZIGBEE_otcmdbuffer)->evtserial.evt.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_NotificationPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)(p_ZIGBEE_notif_M0_to_M4)->evtserial.evt.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_M0RequestPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)(p_ZIGBEE_request_M0_to_M4)->evtserial.evt.payload;
}

/**
 * @brief  This function is used to transfer the commands from the M4 to the M0.
 *
 * @param   None
 * @return  None
 */
void ZIGBEE_CmdTransfer(void)
{
  Zigbee_Cmd_Request_t *cmd_req = (Zigbee_Cmd_Request_t *)p_ZIGBEE_otcmdbuffer->cmdserial.cmd.payload;

  /* Zigbee OT command cmdcode range 0x280 .. 0x3DF = 352 */
  p_ZIGBEE_otcmdbuffer->cmdserial.cmd.cmdcode = 0x280U;
  /* Size = otCmdBuffer->Size (Number of OT cmd arguments : 1 arg = 32bits so multiply by 4 to get size in bytes)
   * + ID (4 bytes) + Size (4 bytes) */
  p_ZIGBEE_otcmdbuffer->cmdserial.cmd.plen = 8U + (cmd_req->Size * 4U);

  TL_ZIGBEE_SendM4RequestToM0();

  /* Wait completion of cmd */
  Wait_Getting_Ack_From_M0();
}

/**
 * @brief  This function is called when the M0+ acknowledge the fact that it has received a Cmd
 *
 *
 * @param   Otbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_CmdEvtReceived(TL_EvtPacket_t *Otbuffer)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Otbuffer);

  Receive_Ack_From_M0();
}

/**
 * @brief  This function is called when notification from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_NotReceived(TL_EvtPacket_t *Notbuffer)
{
  p_ZIGBEE_notif_M0_to_M4 = Notbuffer;

  Receive_Notification_From_M0();
}

/**
 * @brief  This function is called before sending any ot command to the M0
 *         core. The purpose of this function is to be able to check if
 *         there are no notifications coming from the M0 core which are
 *         pending before sending a new ot command.
 * @param  None
 * @retval None
 */
void Pre_ZigbeeCmdProcessing(void)
{
  UTIL_SEQ_WaitEvt(EVENT_SYNCHRO_BYPASS_IDLE);
}

/**
 * @brief  This function waits for getting an acknowledgment from the M0.
 *
 * @param  None
 * @retval None
 */
static void Wait_Getting_Ack_From_M0(void)
{
  UTIL_SEQ_WaitEvt(EVENT_ACK_FROM_M0_EVT);
}

/**
 * @brief  Receive an acknowledgment from the M0+ core.
 *         Each command send by the M4 to the M0 are acknowledged.
 *         This function is called under interrupt.
 * @param  None
 * @retval None
 */
static void Receive_Ack_From_M0(void)
{
  UTIL_SEQ_SetEvt(EVENT_ACK_FROM_M0_EVT);
}

/**
 * @brief  Receive a notification from the M0+ through the IPCC.
 *         This function is called under interrupt.
 * @param  None
 * @retval None
 */
static void Receive_Notification_From_M0(void)
{
  CptReceiveNotifyFromM0++;
  UTIL_SEQ_SetTask(1U << (uint32_t)CFG_TASK_NOTIFY_FROM_M0_TO_M4, CFG_SCH_PRIO_0);
}

/**
 * @brief  This function is called when a request from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_M0RequestReceived(TL_EvtPacket_t *Reqbuffer)
{
  p_ZIGBEE_request_M0_to_M4 = Reqbuffer;

  CptReceiveRequestFromM0++;
  UTIL_SEQ_SetTask(1U << (uint32_t)CFG_TASK_REQUEST_FROM_M0_TO_M4, CFG_SCH_PRIO_0);
}

/**
 * @brief Perform initialization of TL for Zigbee.
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_TL_INIT(void)
{
  ZigbeeConfigBuffer.p_ZigbeeOtCmdRspBuffer = (uint8_t *)&ZigbeeOtCmdBuffer;
  ZigbeeConfigBuffer.p_ZigbeeNotAckBuffer = (uint8_t *)ZigbeeNotifRspEvtBuffer;
  ZigbeeConfigBuffer.p_ZigbeeNotifRequestBuffer = (uint8_t *)ZigbeeNotifRequestBuffer;
  TL_ZIGBEE_Init(&ZigbeeConfigBuffer);
}

/**
 * @brief Process the messages coming from the M0.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ProcessNotifyM0ToM4(void)
{
  if (CptReceiveNotifyFromM0 != 0)
  {
    /* Reset counter */
    CptReceiveNotifyFromM0 = 0;
    Zigbee_CallBackProcessing();
  }
}

/**
 * @brief Process the requests coming from the M0.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ProcessRequestM0ToM4(void)
{
  if (CptReceiveRequestFromM0 != 0)
  {
    CptReceiveRequestFromM0 = 0;
    Zigbee_M0RequestProcessing();
  }
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* USER CODE END FD_LOCAL_FUNCTIONS */

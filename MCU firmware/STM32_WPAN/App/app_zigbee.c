
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
#include "zcl/general/zcl.alarm.h"
#include "zcl/general/zcl.time.h"
#include "zcl/general/zcl.commission.h"
#include "zcl/general/zcl.temp.meas.h"
#include "zcl/general/zcl.fan.h"
#include "zcl/general/zcl.onoff.h"
#include "zcl/general/zcl.color.h"
#include "zcl/general/zcl.level.h"

/* USER CODE BEGIN Includes */
#include "main.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_STARTUP_FAIL_DELAY               500U
#define CHANNEL                                     13
#define ZED_SLEEP_TIME_30S                           1 /* 30s sleep time unit */

#define SW1_ENDPOINT                                1

/* Commissioning (endpoint 1) specific defines ------------------------------------------------*/
#define APS_SECURED_1                      false
#define COMMISSIONING_DEST_ENDPOINT_1                      ZB_ENDPOINT_BCAST
/* USER CODE BEGIN Commissioning (endpoint 1) defines */
/* USER CODE END Commissioning (endpoint 1) defines */

/* Temperature_meas (endpoint 1) specific defines ------------------------------------------------*/
#define TEMP_MIN_1                      -27315
#define TEMP_MAX_1                      32767
#define TEMP_TOLERANCE_1                      2048
/* USER CODE BEGIN Temperature_meas (endpoint 1) defines */
/* USER CODE END Temperature_meas (endpoint 1) defines */

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

  struct ZbZclClusterT *identify_client_1;
  struct ZbZclClusterT *alarm_client_1;
  struct ZbZclClusterT *time_client_1;
  struct ZbZclClusterT *commissioning_server_1;
  struct ZbZclClusterT *temperature_meas_server_1;
  struct ZbZclClusterT *fan_server_1;
  struct ZbZclClusterT *onOff_server_1;
  struct ZbZclClusterT *colorControl_server_1;
  struct ZbZclClusterT *levelControl_server_1;
};
static struct zigbee_app_info zigbee_app_info;

/* Alarm client 1 custom callbacks */
static ZbZclAlarmClientCallbackT AlarmClientCallbacks_1;

/* Commissioning server 1 custom callbacks */
static enum ZclStatusCodeT commission_server_1_restart_device(struct ZbZclClusterT *cluster, struct ZbZclCommissionClientRestartDev *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT commission_server_1_save_startup(struct ZbZclClusterT *cluster, struct ZbZclCommissionClientSaveStartup *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT commission_server_1_restore_startup(struct ZbZclClusterT *cluster, struct ZbZclCommissionClientRestoreStartup *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT commission_server_1_reset_startup(struct ZbZclClusterT *cluster, struct ZbZclCommissionClientResetStartup *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclCommissionServerCallbacksT CommissionServerCallbacks_1 =
{
  .restart_device = commission_server_1_restart_device,
  .save_startup = commission_server_1_save_startup,
  .restore_startup = commission_server_1_restore_startup,
  .reset_startup = commission_server_1_reset_startup,
};

/* OnOff server 1 custom callbacks */
static enum ZclStatusCodeT onOff_server_1_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_1_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_1_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclOnOffServerCallbacksT OnOffServerCallbacks_1 =
{
  .off = onOff_server_1_off,
  .on = onOff_server_1_on,
  .toggle = onOff_server_1_toggle,
};

/* ColorControl server 1 custom callbacks */
static enum ZclStatusCodeT colorControl_server_1_move_to_hue(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToHueReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_hue(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveHueReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_step_hue(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepHueReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_to_sat(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToSatReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_sat(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveSatReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_step_sat(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepSatReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_to_hue_sat(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToHueSatReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_to_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_step_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_to_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_to_hue_enh(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToHueEnhReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_hue_enh(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveHueEnhReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_step_hue_enh(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepHueEnhReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_to_hue_sat_enh(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToHueSatEnhReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_color_loop_set(struct ZbZclClusterT *cluster, struct ZbZclColorClientColorLoopSetReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_stop_move_step(struct ZbZclClusterT *cluster, struct ZbZclColorClientStopMoveStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_step_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclColorServerCallbacksT ColorServerCallbacks_1 =
{
  .move_to_hue = colorControl_server_1_move_to_hue,
  .move_hue = colorControl_server_1_move_hue,
  .step_hue = colorControl_server_1_step_hue,
  .move_to_sat = colorControl_server_1_move_to_sat,
  .move_sat = colorControl_server_1_move_sat,
  .step_sat = colorControl_server_1_step_sat,
  .move_to_hue_sat = colorControl_server_1_move_to_hue_sat,
  .move_to_color_xy = colorControl_server_1_move_to_color_xy,
  .move_color_xy = colorControl_server_1_move_color_xy,
  .step_color_xy = colorControl_server_1_step_color_xy,
  .move_to_color_temp = colorControl_server_1_move_to_color_temp,
  .move_to_hue_enh = colorControl_server_1_move_to_hue_enh,
  .move_hue_enh = colorControl_server_1_move_hue_enh,
  .step_hue_enh = colorControl_server_1_step_hue_enh,
  .move_to_hue_sat_enh = colorControl_server_1_move_to_hue_sat_enh,
  .color_loop_set = colorControl_server_1_color_loop_set,
  .stop_move_step = colorControl_server_1_stop_move_step,
  .move_color_temp = colorControl_server_1_move_color_temp,
  .step_color_temp = colorControl_server_1_step_color_temp,
};

/* LevelControl server 1 custom callbacks */
static enum ZclStatusCodeT levelControl_server_1_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_1_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_1_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_1_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclLevelServerCallbacksT LevelServerCallbacks_1 =
{
  .move_to_level = levelControl_server_1_move_to_level,
  .move = levelControl_server_1_move,
  .step = levelControl_server_1_step,
  .stop = levelControl_server_1_stop,
};

/* USER CODE BEGIN PV */
/* USER CODE END PV */
/* Functions Definition ------------------------------------------------------*/

/* Commissioning server restart_device 1 command callback */
static enum ZclStatusCodeT commission_server_1_restart_device(struct ZbZclClusterT *cluster, struct ZbZclCommissionClientRestartDev *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 0 Commission server 1 restart_device 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 0 Commission server 1 restart_device 1 */
}

/* Commissioning server save_startup 1 command callback */
static enum ZclStatusCodeT commission_server_1_save_startup(struct ZbZclClusterT *cluster, struct ZbZclCommissionClientSaveStartup *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 1 Commission server 1 save_startup 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 1 Commission server 1 save_startup 1 */
}

/* Commissioning server restore_startup 1 command callback */
static enum ZclStatusCodeT commission_server_1_restore_startup(struct ZbZclClusterT *cluster, struct ZbZclCommissionClientRestoreStartup *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 2 Commission server 1 restore_startup 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 2 Commission server 1 restore_startup 1 */
}

/* Commissioning server reset_startup 1 command callback */
static enum ZclStatusCodeT commission_server_1_reset_startup(struct ZbZclClusterT *cluster, struct ZbZclCommissionClientResetStartup *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 3 Commission server 1 reset_startup 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 3 Commission server 1 reset_startup 1 */
}

/* OnOff server off 1 command callback */
static enum ZclStatusCodeT onOff_server_1_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 4 OnOff server 1 off 1 */
	stop_led_control();
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 4 OnOff server 1 off 1 */
}

/* OnOff server on 1 command callback */
static enum ZclStatusCodeT onOff_server_1_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 5 OnOff server 1 on 1 */
	start_led_control(&SystemSettings);
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);

  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 5 OnOff server 1 on 1 */
}

/* OnOff server toggle 1 command callback */
static enum ZclStatusCodeT onOff_server_1_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 6 OnOff server 1 toggle 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 6 OnOff server 1 toggle 1 */
}

/* ColorControl server move_to_hue 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_to_hue(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToHueReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 7 ColorControl server 1 move_to_hue 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 7 ColorControl server 1 move_to_hue 1 */
}

/* ColorControl server move_hue 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_hue(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveHueReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 8 ColorControl server 1 move_hue 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 8 ColorControl server 1 move_hue 1 */
}

/* ColorControl server step_hue 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_step_hue(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepHueReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 9 ColorControl server 1 step_hue 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 9 ColorControl server 1 step_hue 1 */
}

/* ColorControl server move_to_sat 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_to_sat(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToSatReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 10 ColorControl server 1 move_to_sat 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 10 ColorControl server 1 move_to_sat 1 */
}

/* ColorControl server move_sat 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_sat(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveSatReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 11 ColorControl server 1 move_sat 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 11 ColorControl server 1 move_sat 1 */
}

/* ColorControl server step_sat 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_step_sat(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepSatReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 12 ColorControl server 1 step_sat 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 12 ColorControl server 1 step_sat 1 */
}

/* ColorControl server move_to_hue_sat 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_to_hue_sat(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToHueSatReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 13 ColorControl server 1 move_to_hue_sat 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 13 ColorControl server 1 move_to_hue_sat 1 */
}

/* ColorControl server move_to_color_xy 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_to_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 14 ColorControl server 1 move_to_color_xy 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 14 ColorControl server 1 move_to_color_xy 1 */
}

/* ColorControl server move_color_xy 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 15 ColorControl server 1 move_color_xy 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 15 ColorControl server 1 move_color_xy 1 */
}

/* ColorControl server step_color_xy 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_step_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 16 ColorControl server 1 step_color_xy 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 16 ColorControl server 1 step_color_xy 1 */
}

/* ColorControl server move_to_color_temp 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_to_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 17 ColorControl server 1 move_to_color_temp 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 17 ColorControl server 1 move_to_color_temp 1 */
}

/* ColorControl server move_to_hue_enh 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_to_hue_enh(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToHueEnhReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 18 ColorControl server 1 move_to_hue_enh 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 18 ColorControl server 1 move_to_hue_enh 1 */
}

/* ColorControl server move_hue_enh 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_hue_enh(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveHueEnhReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 19 ColorControl server 1 move_hue_enh 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 19 ColorControl server 1 move_hue_enh 1 */
}

/* ColorControl server step_hue_enh 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_step_hue_enh(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepHueEnhReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 20 ColorControl server 1 step_hue_enh 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 20 ColorControl server 1 step_hue_enh 1 */
}

/* ColorControl server move_to_hue_sat_enh 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_to_hue_sat_enh(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToHueSatEnhReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 21 ColorControl server 1 move_to_hue_sat_enh 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 21 ColorControl server 1 move_to_hue_sat_enh 1 */
}

/* ColorControl server color_loop_set 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_color_loop_set(struct ZbZclClusterT *cluster, struct ZbZclColorClientColorLoopSetReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 22 ColorControl server 1 color_loop_set 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 22 ColorControl server 1 color_loop_set 1 */
}

/* ColorControl server stop_move_step 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_stop_move_step(struct ZbZclClusterT *cluster, struct ZbZclColorClientStopMoveStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 23 ColorControl server 1 stop_move_step 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 23 ColorControl server 1 stop_move_step 1 */
}

/* ColorControl server move_color_temp 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 24 ColorControl server 1 move_color_temp 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 24 ColorControl server 1 move_color_temp 1 */
}

/* ColorControl server step_color_temp 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_step_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 25 ColorControl server 1 step_color_temp 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 25 ColorControl server 1 step_color_temp 1 */
}

/* LevelControl server move_to_level 1 command callback */
static enum ZclStatusCodeT levelControl_server_1_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 26 LevelControl server 1 move_to_level 1 */
    uint8_t channel_id;
    uint8_t desired_brightness;

   // Determine which channel the command is for
   
    // Extract the desired brightness level from the request
        desired_brightness = req->level;
//        determine_channel_id(srcInfo);
//        channel_id = determine_channel_id(srcInfo);
//        set_brightness_single(channel_id, desired_brightness);
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 26 LevelControl server 1 move_to_level 1 */
}

/* LevelControl server move 1 command callback */
static enum ZclStatusCodeT levelControl_server_1_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 27 LevelControl server 1 move 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 27 LevelControl server 1 move 1 */
}

/* LevelControl server step 1 command callback */
static enum ZclStatusCodeT levelControl_server_1_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 28 LevelControl server 1 step 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 28 LevelControl server 1 step 1 */
}

/* LevelControl server stop 1 command callback */
static enum ZclStatusCodeT levelControl_server_1_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 29 LevelControl server 1 stop 1 */
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 29 LevelControl server 1 stop 1 */
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
  req.deviceId = ZCL_DEVICE_COMBINED_INTERFACE;
  req.endpoint = SW1_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* Identify client */
  zigbee_app_info.identify_client_1 = ZbZclIdentifyClientAlloc(zigbee_app_info.zb, SW1_ENDPOINT);
  assert(zigbee_app_info.identify_client_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.identify_client_1);
  /* Alarm client */
  zigbee_app_info.alarm_client_1 = ZbZclAlarmClientAlloc(zigbee_app_info.zb, SW1_ENDPOINT, AlarmClientCallbacks_1, NULL);
  assert(zigbee_app_info.alarm_client_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.alarm_client_1);
  /* Time client */
  zigbee_app_info.time_client_1 = ZbZclTimeClientAlloc(zigbee_app_info.zb, SW1_ENDPOINT);
  assert(zigbee_app_info.time_client_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.time_client_1);
  /* Commissioning server */
  zigbee_app_info.commissioning_server_1 = ZbZclCommissionServerAlloc(zigbee_app_info.zb, COMMISSIONING_DEST_ENDPOINT_1, ZCL_PROFILE_HOME_AUTOMATION, APS_SECURED_1, &CommissionServerCallbacks_1, NULL);
  assert(zigbee_app_info.commissioning_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.commissioning_server_1);
  /* Temperature meas server */
  zigbee_app_info.temperature_meas_server_1 = ZbZclTempMeasServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, TEMP_MIN_1, TEMP_MAX_1, TEMP_TOLERANCE_1);
  assert(zigbee_app_info.temperature_meas_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.temperature_meas_server_1);
  /* Fan server */
  zigbee_app_info.fan_server_1 = ZbZclFanServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT);
  assert(zigbee_app_info.fan_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.fan_server_1);
  /* OnOff server */
  zigbee_app_info.onOff_server_1 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, &OnOffServerCallbacks_1, NULL);
  assert(zigbee_app_info.onOff_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_1);
  /* ColorControl server */
  struct ZbColorClusterConfig colorServerConfig_1 = {
    .callbacks = ColorServerCallbacks_1,
    /* Please complete the other attributes according to your application:
     *          .capabilities           //uint8_t (e.g. ZCL_COLOR_CAP_HS)
     *          .enhanced_supported     //bool
     */
    /* USER CODE BEGIN Color Server Config (endpoint1) */
    /* USER CODE END Color Server Config (endpoint1) */
  };
  zigbee_app_info.colorControl_server_1 = ZbZclColorServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, zigbee_app_info.onOff_server_1, NULL, 0, &colorServerConfig_1, NULL);
  assert(zigbee_app_info.colorControl_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.colorControl_server_1);
  /* LevelControl server */
  zigbee_app_info.levelControl_server_1 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, zigbee_app_info.onOff_server_1, &LevelServerCallbacks_1, NULL);
  assert(zigbee_app_info.levelControl_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_1);

  /* USER CODE BEGIN CONFIG_ENDPOINT */
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
    APP_DBG("Network config : APP_STARTUP_CENTRALIZED_END_DEVICE");
    config.startupControl = zigbee_app_info.startupControl;

    /* Using the default HA preconfigured Link Key */
    memcpy(config.security.preconfiguredLinkKey, sec_key_ha, ZB_SEC_KEYSIZE);

    config.channelList.count = 1;
    config.channelList.list[0].page = 0;
    config.channelList.list[0].channelMask = 1 << CHANNEL; /*Channel in use */

    /* Add End device configuration */
    config.capability &= ~(MCP_ASSOC_CAP_RXONIDLE | MCP_ASSOC_CAP_DEV_TYPE | MCP_ASSOC_CAP_ALT_COORD);
    config.endDeviceTimeout=ZED_SLEEP_TIME_30S;

    /* Using ZbStartupWait (blocking) */
    status = ZbStartupWait(zigbee_app_info.zb, &config);

    APP_DBG("ZbStartup Callback (status = 0x%02x)", status);
    zigbee_app_info.join_status = status;

    if (status == ZB_STATUS_SUCCESS)
    {
      zigbee_app_info.join_delay = 0U;
      zigbee_app_info.init_after_join = true;
      APP_DBG("Startup done !\n");
      /* USER CODE BEGIN 30 */

      /* USER CODE END 30 */
    }
    else
    {
      APP_DBG("Startup failed, attempting again after a short delay (%d ms)", APP_ZIGBEE_STARTUP_FAIL_DELAY);
      zigbee_app_info.join_delay = HAL_GetTick() + APP_ZIGBEE_STARTUP_FAIL_DELAY;
      /* USER CODE BEGIN 31 */

      /* USER CODE END 31 */
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
    APP_DBG("identify Client on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("alarm Client on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("time Client on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("commissioning Server on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("temperature_meas Server on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("fan Server on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("onOff Server on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("colorControl Server on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("levelControl Server on Endpoint %d", SW1_ENDPOINT);
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

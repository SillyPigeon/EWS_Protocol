#ifndef EWS_PORTS_H
#define EWS_PORTS_H


/*
*********************************************************************************************************
*                                           INCLUDE HEADER FILES
*********************************************************************************************************
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ews.h"

/*
*********************************************************************************************************
*                                             MARCO CONFIG 
*********************************************************************************************************
*/

#define TRUE   1
#define FALSE  0

#define EWS_UI_TYPE_MSTAR                   0
#define EWS_UI_TYPE_CUS                     1

#define EWS_UI_TYPE                         EWS_UI_TYPE_CUS
#define EN_EWS_INPUT_FOR_CUSTOMER       FALSE
#define EN_EWS_CONTENT_FOCUS            FALSE 
#define EN_EWS_EXIT_WASPADA_BY_ANYKEY   TRUE

#define CONDITION(x)                        1
#define FUNCTION_0(x)                       0
#define FUNCTION_1(x)                       1
#define FUNCTION_ON(x)                      _EWS_Sys_Void_Func()
#define FUNCTION_OFF(x)                     _EWS_Sys_Void_Func()
#define FUNCTION_ADJUST(x)                  _EWS_Sys_Void_Func()

/*
*********************************************************************************************************
*                                              SYS FUNC
*********************************************************************************************************
*/

#define  EWS_STRCPY(pDest, pSrc, DestSize)                     memcpy(pDest, pSrc, DestSize)
#define  EWS_STRCPY_8to16(pU16Dest, pU8Src, DestSize)          memcpy(pU16Dest, pU8Src, DestSize)
#define  EWS_MEMSET(ptr, value, p_size)                        memset(ptr, value, p_size) 
#define  EWS_MALLOC(p_size, buff_id)                           malloc(p_size);
#define  EWS_FREE(p_buff, buff_id)                             free(p_buff);   

/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/

#define EWS_DESC_BUF_SIZE                       4096
#define EWS_NORMAL_PERIOD                       1000//ms
#define EWS_URGENT_PERIOD                       200//ms 
#define EWS_VOLUME_VALUE                        100u
#define EWS_TEXT_SCROLL_LINE_BUF_SIZE           48


#define  EWS_OSD                                   0xFF
#define  EWS_AWAS_HWND                             0xFF
#define  EWS_WASPADA_HWND                          0xFF                       
#define  EWS_SIAGA_HWND                            0xFF
#define  EWS_CODE_SET_HWND                         0xFF

                
#define EWS_DISASTER_INVALID_HWND                  0xFF
#define EWS_DISASTER_TYPE_HWND                     0xFF
#define EWS_DISASTER_DATE_AND_TIME_HWND            0xFF
#define EWS_DISASTER_LONGITUDE_HWND                0xFF
#define EWS_DISASTER_CHARACTER_HWND                0xFF

#define EWS_AWAS_SLIDE_SCROL_HWND(x)               0xFF

#define EWS_SIAGA_SLIDE_SCROL_HWND(x)              0xFF

#define EWS_WASPADA_SLIDE_SCROL_HWND(x)            0xFF

#define EWS_BMP_gempa                              0xFF                       
#define EWS_BMP_tsunami                            0xFF
#define EWS_BMP_gunung_meletus                     0xFF
#define EWS_BMP_gerakan_tanah                      0xFF
#define EWS_BMP_banjir                             0xFF
#define EWS_BMP_kekeringan                         0xFF                     
#define EWS_BMP_kebakaran_hutan                    0xFF
#define EWS_BMP_erosi                              0xFF
#define EWS_BMP_kebakaran_Gedung                   0xFF
#define EWS_BMP_gelombang_ekstrin_dan_abrasi       0xFF
#define EWS_BMP_cuaca_eksrim                       0xFF
#define EWS_BMP_kegagalan_teknologi                0xFF
#define EWS_BMP_Epidemi                            0xFF
#define EWS_BMP_konflik_sosial                     0xFF
#define EWS_BMP_cadangan                           0xFF

#define EWS_BMP_Logo_BMKG                          0xFF
#define EWS_BMP_Logo_BNPB                          0xFF

/*
*********************************************************************************************************
*                                       EWS RELATED STATUS TYPES
*********************************************************************************************************
*/

typedef enum
{
    EWS_INPUT_CONTROL_INVALID                      = 0,
    EWS_INPUT_CONTROL_STATUS,
  #if EN_EWS_INPUT_FOR_CUSTOMER
    EWS_INPUT_CONTROL_FOR_CUSTOMER,
  #endif
}EWS_USER_INPUT_TYPE;

typedef enum
{
    EWS_SYS_EVENT_NULL                             = 0, 
    EWS_SYS_EVENT_TIMESHIFT,                                               
}E_EWS_SYS_EVENT;

typedef struct ews_pre_setting{
    U8      b_PreConnectedARC             :6;
    U8      b_PreLockByParent             :1;
    U8      b_PreMuteByUser               :1;
}S_EWS_SYS_SETTING;

typedef enum
{
    INPUT_NULL                                   = 1,
    POWER_KEY                                    = 2, /*meas EWS sys is in running or not*/
    EXIT_KEY                                     = 3, 
    UP_KEY                                       = 4, /*meas EWS sys is in running or not*/
    DOWN_KEY                                     = 5,
    MUTE_KEY                                     = 6,
}EWS_INPUT;

typedef enum _EWS_SCROLL_TYPE
{
    EN_EWS_SCROLL_RESET,
    EN_EWS_SCROLL_UP,
    EN_EWS_SCROLL_DOWN,
}EWS_SCROLL_TYPE;

typedef struct ews_scroll_setting{
    U8 u8ForwardStep;
    U8 u8EwsScrollLineCount;
}S_EWS_SCROLL_SETTING;

void _EWS_Sys_Void_Func(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                           SI LIB BLOCK
*[Description]:
*                 In this model, user need realize the method to get EWS info from SI/driver 
*********************************************************************************************************
*/

/* EWS  OrginalNetworkID */
#define ONID_INDONESIA_EWS 0x2168
#define SI_EWS_NUMBER_CHAR_LOCTION              256
#define SI_EWS_NUMBER_CHAR_DISASTER_TYPE        256
#define SI_EWS_NUMBER_CHAR_DISASTER_POSITION    256
#define SI_EWS_NUMBER_CHAR_DISASTER_DATE        256
#define SI_EWS_NUMBER_CHAR_DISASTER_CHARACTER   256
#define SI_EWS_NUMBER_CHAR_MESSAGE              3201

typedef struct
{
    U8 u8_EwsDisasterSymbol;
    U8 u8_EwsAuthor;
    U8 u8_EwsDisasterLoctionStatus;                                      //EWS warnning level : 1 -high, 2 -middle, 3-low, 0xFF is invalid
    U8 u8_EwsLocationDisaster[SI_EWS_NUMBER_CHAR_LOCTION];               //from TRDW
    U8 u8_EwsDisasterType[SI_EWS_NUMBER_CHAR_DISASTER_TYPE];             //from TCDW
    U8 u8_EwsDisasterDate[SI_EWS_NUMBER_CHAR_DISASTER_DATE];             //from TCDW
    U8 u8_EwsDisasterLongitude[SI_EWS_NUMBER_CHAR_DISASTER_POSITION];    //from TCDW
    U8 u8_EwsDisasterCharacter[SI_EWS_NUMBER_CHAR_DISASTER_CHARACTER];   //from TCDW
    U8 u8_EwsDisasterRemark[SI_EWS_NUMBER_CHAR_MESSAGE];                 //from TMDW
    BOOLEAN bisValid;
} SI_EWSSETTING;

void  SI_GetEwsData(SI_EWSSETTING* ptr_setting);

void  SI_ExitEWS(void);

U8 SI_GetEWSRank(SI_EWSSETTING si_setting);



/*$PAGE*/
/*
*********************************************************************************************************
*                                          CHECK BLOCK
*[Description]:
*                 In this model, user need realize the basic method to check TV status
*                 make sure the function can be useful independent and acquire TV status correct
*********************************************************************************************************
*/

BOOLEAN _EWS_Sys_Has_UIShown(EWS_RANK rank);

BOOLEAN _EWS_Sys_PannelMuteChecker(void);

BOOLEAN _EWS_Sys_AlarmDoneChecker(void);

BOOLEAN _EWS_Sys_DtvStatusCheck(void);

BOOLEAN _EWS_Sys_ScanStatusCheck(void);

BOOLEAN _EWS_Sys_StandbyCheck(void);

BOOLEAN _EWS_Sys_InstallStatusCheck(void);

BOOLEAN _EWS_Sys_CodeSetStatusCheck(void);

BOOLEAN _EWS_Sys_BadSignalCheck(void);

BOOLEAN _EWS_Sys_OSDActiveCheck(void);

BOOLEAN _EWS_Sys_AudioMuteCheck(void);

BOOLEAN _EWS_Sys_ARCConnectCheck(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                               MANAGE   BLOCK
*[Description]:
*                 In this model, user need realize the basic action to adjust TV status
*                 make sure the function can be useful independent and actual to charge TV correct
*********************************************************************************************************
*/

void _EWS_Sys_PannelMuteManager(BOOLEAN setting);                                                                                    

void _EWS_Sys_DisasterPageManager(BOOLEAN setting);

void _EWS_Sys_AudioMuteManager(BOOLEAN setting);

void _EWS_Sys_AlarmManager(BOOLEAN setting);

void _EWS_Sys_ARCDeviceManager(BOOLEAN setting);

U8 _EWS_Sys_VolumeManager(U8 volume ,BOOLEAN setting);

void _EWS_Sys_RestoreVolumeManager(BOOLEAN setting);


/*$PAGE*/
/*
*********************************************************************************************************
*                                              USER RELATED
*[Description]:
*                In this model, user need to realize the events handle method witch would be conflict with EWS
*                make sure the function can be useful independent and actual to check and deal with conflct event
*********************************************************************************************************
*/

BOOLEAN _EWS_User_TimeShiftEventCheck(void);

void _EWS_User_StopTimeShift(void);

#if EN_EWS_INPUT_FOR_CUSTOMER
void _EWS_User_ScrollText(EWS_SCROLL_TYPE uTextScroll);

U16  _EWS_User_GetScrollTextOffset(void);

void _EWS_User_MoveScrollBar(U8 line, BOOLEAN option);
#if (EN_EWS_CONTENT_FOCUS)
void EWS_Customer_UpKeyFocusAction(EWS_RANK);
void EWS_Customer_DownKeyFocusAction(EWS_RANK);
#endif
#endif

void EWS_User_Clear_HwndData(void);
/*$PAGE*/
/*
*********************************************************************************************************
*                                              SIMPLE BASIC FUNC
*[Description]:
*                In this model, user shall define the basic func for EWS function use
*                if the described func is complex, please separate to USER RELATED block
*********************************************************************************************************
*/

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_ClearOSDPage()
//  [Description]:
//                 to clear desk, make TV ready to show EWS info page
//  [Arguments]:
//                 NONE
//  [Return]:
//                 NONE
//**************************************************************************
#define _EWS_Sys_ClearOSDPage()        _EWS_Sys_Void_Func()

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_DiffTimeFromNow()
//  [Description]:
//                 to get a difference from NOW timestamp with one timestamp by setting
//  [Arguments]:
//                 t: a arbitrary timestamp by user put into
//  [Return]:
//                 time: the U32 time count from t to NOW
//**************************************************************************
#define _EWS_Sys_DiffTimeFromNow(t)         t

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_TimerGetCurTime()
//  [Description]:
//                 to get current timestamp by millisecond
//  [Arguments]:
//                 NONE
//  [Return]:
//                 time: current timestamp
//**************************************************************************
#define _EWS_Sys_TimerGetCurTime()  	     0

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_FocusHwndSet()
//  [Description]:
//                 to set hwnd focus 
//  [Arguments]:
//                 HWND index
//  [Return]:
//                 NONE
//**************************************************************************
#define _EWS_Sys_FocusHwndSet(h)            

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_HwndDataGet()
//  [Description]:
//                 to get hwnd DATA
//  [Arguments]:
//                 HWND index
//  [Return]:
//                 U32
//**************************************************************************
#define _EWS_Sys_HwndDataGet(h)              0

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_ParentLockCheck()
//  [Description]:
//                to check the system has locked by parent or not
//  [Arguments]:
//                 NONE
//  [Return]:
//            TRUE:system has been lock  FALSE: audio is normal before
//**************************************************************************
#define _EWS_Sys_ParentalBlockCheck()          0

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_VchipMuteManager()
//  [Description]:
//                 manage the TV audio status in Vchip
//  [Arguments]:
//                 TRUE: to force mute auido for Vchip
//                 FALSE: to release auido for Vchip
//  [Return]:
//                 NONE
//**************************************************************************
#define _EWS_Sys_VchipMuteManager(setting)  

#define _EWS_Sys_ParentalBlockCheck()           0

#define _EWS_Sys_RestoreVolumeCheck()           0

/*$PAGE*/
/*
*********************************************************************************************************
*                                              CONFIG ERROR WARNING
*[Description]:
*                In this model, user may add some config error tip to make sure code running is OK
*********************************************************************************************************
*/
#if (EN_EWS_CONTENT_FOCUS)&&(!EN_EWS_INPUT_FOR_CUSTOMER)
#error "EN_EWS_CONTENT_FOCUS is useless now, you must enable EN_EWS_INPUT_FOR_CUSTOMER to support it !"
#endif

#if (EN_EWS_EXIT_WASPADA_BY_ANYKEY)&&(EN_EWS_INPUT_FOR_CUSTOMER)
#error "Marcos defined error!!! if you want to use EN_EWS_EXIT_WASPADA_BY_ANYKEY , you must disable EN_EWS_INPUT_FOR_CUSTOMER !"
#endif
#endif
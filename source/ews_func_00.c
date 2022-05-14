/*
*********************************************************************************************************
*                                                
*
*                          
*                                          
*
* File    : ews_ports.c
* By      :  
* Version : 
*
* 
* ---------------

*********************************************************************************************************
*/

#include "ews_func.h"

/*
*********************************************************************************************************
*                                   EWS USER DEFINED FUNC BLOCK
*********************************************************************************************************
*/
#define  FOCUS_UP_STEP                                         1
#define  INVALID_MARQUEE_FOCUS                                 0
#define  MAX_NUM_OF_MARQUEE_LIST                               5
#define  MAX_NUM_OF_ALL_TIME_MARQUEE_LIST                      6
#define  E_SCREEN_MUTE_BY_EWS                                  0
#define  VOLUME_GET                                            FALSE
#define  VOLUME_SET                                            TRUE
#define  SCROLL_TEXT_OFFSET_LINE                               3

/*
*********************************************************************************************************
*                                   EWS USER DEFINED VARIABLE
*********************************************************************************************************
*/

#if EN_EWS_INPUT_FOR_CUSTOMER
static   S_EWS_SCROLL_SETTING                                  s_Scroll;
#if (EN_EWS_CONTENT_FOCUS)
static   U8                                                    MarqueeFocusHwnd;
#endif
#endif
U16      au16EwsTextScrollLineInx[EWS_TEXT_SCROLL_LINE_BUF_SIZE] = {0};

#if (EN_EWS_CONTENT_FOCUS)
static const HWND EWSMarqueeHwndList[MAX_NUM_OF_MARQUEE_LIST]=
{
    EWS_DISASTER_INVALID_HWND,
    EWS_DISASTER_TYPE_HWND,
    EWS_DISASTER_DATE_AND_TIME_HWND,
    EWS_DISASTER_LONGITUDE_HWND,
    EWS_DISASTER_CHARACTER_HWND
};
#endif

static const HWND EWSAllTimeMarqueeHwndList[MAX_NUM_OF_ALL_TIME_MARQUEE_LIST]=
{
    EWS_AWAS_SLIDE_SCROL_HWND(LOCATION_OF_DISASTER),
    EWS_AWAS_SLIDE_SCROL_HWND(REMARK_DESCRIPTION),
    EWS_SIAGA_SLIDE_SCROL_HWND(LOCATION_OF_DISASTER),
    EWS_SIAGA_SLIDE_SCROL_HWND(REMARK_DESCRIPTION),
    EWS_WASPADA_SLIDE_SCROL_HWND(LOCATION_OF_DISASTER),
  #if (FALSE == EN_EWS_INPUT_FOR_CUSTOMER)
    EWS_WASPADA_SLIDE_SCROL_HWND(REMARK_DESCRIPTION),
  #endif
};

void _EWS_Sys_Void_Func(void)
{

}


/*
*********************************************************************************************************
*                                           SI LIB BLOCK
*[Description]:
*                 In this model, user need realize the method to get EWS info from SI/driver 
*********************************************************************************************************
*/

void  SI_GetEwsData(SI_EWSSETTING* setting)
{
    if((SI_EWSSETTING*)0 == setting)
    {
        return;
    }
    FUNCTION_ADJUST(setting);
}  

void  SI_ExitEWS(void)
{
    FUNCTION_ADJUST();
}    

U8 SI_GetEWSRank(SI_EWSSETTING setting)
{
    return ((setting.bisValid == TRUE) ? setting.u8_EwsDisasterLoctionStatus : 0xFF);
}


/*
*********************************************************************************************************
*                                          CHECK BLOCK
*[Description]:
*                 In this model, user need realize the basic method to check TV status
*                 make sure the function can be useful independent and acquire TV status correct
*********************************************************************************************************
*/

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_Has_UIShown()
//  [Description]:
//                 It used to check the EWS UI has been shown or not
//  [Arguments]:
//               rank: the current EWS rank being monitored
//  [Return]:
//              TRUE: shown  FALSE: do not show yet
//**************************************************************************

BOOLEAN _EWS_Sys_Has_UIShown(EWS_RANK rank)
{
    BOOLEAN result = FALSE;
    switch (rank)
    {
    case E_EWS_RANK_AWAS:
        if (CONDITION(x))
        {
            result = TRUE;
        }
        break;
    case E_EWS_RANK_SIAGA:
        if (CONDITION(x))
        {
            result = TRUE;
        }
        break;
    case E_EWS_RANK_WASPADA:
        if (CONDITION(x))
        {
            result = TRUE;
        }
        break;
    default:
        break;
    }
    return result;
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_PannelMuteChecker()
//  [Description]:
//                 It used to check pannel has been mute or not, which means no active picture
//  [Arguments]:
//               
//  [Return]:
//              TRUE: being MUTE  FALSE: do not MUTE yet
//**************************************************************************

BOOLEAN _EWS_Sys_PannelMuteChecker(void)
{                 
    return CONDITION(x) ? FUNCTION_0(x) : FUNCTION_1(x);
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_AlarmDoneChecker()
//  [Description]:
//                It used to check alarm music has played done or not, if done, it need to restart 
//  [Arguments]:
//               
//  [Return]:
//              TRUE:play done  FALSE: is in playing
//**************************************************************************

BOOLEAN _EWS_Sys_AlarmDoneChecker(void)
{
    return CONDITION() ? FUNCTION_0() : FUNCTION_1();
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_DtvStatusCheck()
//  [Description]:
//                to check the TV channel status for EWS monitor
//  [Arguments]:
//               
//  [Return]:
//              TRUE:in DTV  FALSE: DTV is not use,means do not receive PSI/SI info
//**************************************************************************

BOOLEAN _EWS_Sys_DtvStatusCheck(void)
{
    return CONDITION() ? FUNCTION_0() : FUNCTION_1();
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_ScanStatusCheck()
//  [Description]:
//                to check the TV status is in scanning or not, means do not prepare for EWS monitor
//  [Arguments]:
//               
//  [Return]:
//              TRUE:in scaning  FALSE: DTV is not want to scan
//**************************************************************************

BOOLEAN _EWS_Sys_ScanStatusCheck(void)
{
    return CONDITION() ? FUNCTION_0() : FUNCTION_1();
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_StandbyCheck()
//  [Description]:
//                to check the TV status is in standby or not, means do not receive EWS info
//  [Arguments]:
//               
//  [Return]:
//              TRUE:in standby flow, prepare to power down FALSE: DTV is active
//**************************************************************************

BOOLEAN _EWS_Sys_StandbyCheck(void)
{
    return CONDITION() ? FUNCTION_0() : FUNCTION_1();
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_InstallStatusCheck()
//  [Description]:
//                to check is in FTI or not, means TV has not ready to use
//  [Arguments]:
//               
//  [Return]:
//              TRUE:in FTI  FALSE: TV has been setting
//**************************************************************************

BOOLEAN _EWS_Sys_InstallStatusCheck(void)
{
    return CONDITION() ? FUNCTION_0() : FUNCTION_1();
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_CodeSetStatusCheck()
//  [Description]:
//                to check is setting postcode or not,means user do not comfirm the area
//  [Arguments]:
//               
//  [Return]:
//              TRUE:in setting postcode  FALSE: a valid postcode has been set
//**************************************************************************

BOOLEAN _EWS_Sys_CodeSetStatusCheck(void)
{
    return CONDITION() ? FUNCTION_0() : FUNCTION_1();
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_BadSignalCheck()
//  [Description]:
//                to check vedio signal quality
//  [Arguments]:
//               
//  [Return]:
//            TRUE:a good sinal,mean a vedio can play normally FALSE: 
//**************************************************************************

BOOLEAN _EWS_Sys_BadSignalCheck(void)
{
    return CONDITION() ? FUNCTION_0() : FUNCTION_1();
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_OSDActiveCheck()
//  [Description]:
//                to check UI status,try to show EWS info page
//  [Arguments]:
//               
//  [Return]:
//            TRUE:sth has been shown, means need to clear desk FALSE: it's ready to show EWS
//**************************************************************************

BOOLEAN _EWS_Sys_OSDActiveCheck(void)
{
    return CONDITION() ? FUNCTION_0() : FUNCTION_1();
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_AudioMuteCheck()
//  [Description]:
//                to check audio has been user mute before or not, which need restore after alarm
//  [Arguments]:
//               
//  [Return]:
//            TRUE:user mute before FALSE: audio is normal before
//**************************************************************************

BOOLEAN _EWS_Sys_AudioMuteCheck(void)
{
    return CONDITION() ? FUNCTION_0() : FUNCTION_1();
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_ARCConnectCheck()
//  [Description]:
//                to check ARC device connect or not
//  [Arguments]:
//               
//  [Return]:
//            TRUE:user mute before FALSE: audio is normal before
//**************************************************************************

BOOLEAN _EWS_Sys_ARCConnectCheck(void)
{
    return CONDITION() ? FUNCTION_0() : FUNCTION_1();
}


/*
*********************************************************************************************************
*                                               MANAGE   BLOCK
*[Description]:
*                 In this model, user need realize the basic action to adjust TV status
*                 make sure the function can be useful independent and actual to charge TV correct
*********************************************************************************************************
*/

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_PannelMuteManager()
//  [Description]:
//                 to manage pannel status, MUTE with setting flag
//  [Arguments]:
//              TRUE:set pannel mute and no active picture FALSE: release pannel
//  [Return]:
//            
//**************************************************************************

void _EWS_Sys_PannelMuteManager(BOOLEAN setting)
{
    if (TRUE == setting)//OPEN
    {
        FUNCTION_ON();
    }
    else//CLOSE
    {
        FUNCTION_OFF();
    }
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_DisasterPageManager()
//  [Description]:
//                 to manage EWS disaster info page ON or OFF
//  [Arguments]:
//              TRUE:to show EWS info page FALSE: close info page
//  [Return]:
//            
//**************************************************************************

void _EWS_Sys_DisasterPageManager(BOOLEAN setting)
{
    if (TRUE == setting)//OPEN
    {
        FUNCTION_ON();
    }
    else//CLOSE
    {
        FUNCTION_OFF();
    }
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_AudioMuteManager()
//  [Description]:
//                 to manage system MUTE status, to MUTE or to MUTE off
//  [Arguments]:
//              TRUE:MUTE TV audio and show ICON  FALSE: MUTE off TV and let ICON disappear
//  [Return]:
//            
//**************************************************************************

void _EWS_Sys_AudioMuteManager(BOOLEAN setting)
{
    if(TRUE == setting)             /*to mute system*/
    {
        FUNCTION_ON();
    }
    else                             /*to unmute system*/
    {
        FUNCTION_OFF();
    }
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_AlarmManager()
//  [Description]:
//                 to manage alarm status in EWS 
//  [Arguments]:
//              TRUE: to start play EWS alarm music once or func can make it alarm continued
//              FALSE: make alarm stop
//  [Return]:
//            
//**************************************************************************

void _EWS_Sys_AlarmManager(BOOLEAN setting)
{
    if(TRUE == setting) //play       
    {
        FUNCTION_ON();
    }
    else //stop                            
    {
        FUNCTION_OFF();
    }
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_VolumeManager()
//  [Description]:
//                 to manage sys volume 
//  [Arguments]:
//              volume: the volume wtich would set into sys
//              setting: if it equal VOLUME_SET ,the func will to adjust system volume and return
//  [Return]:
//            
//**************************************************************************

U8 _EWS_Sys_VolumeManager(U8 volume ,BOOLEAN setting)
{
    if(VOLUME_SET == setting) //need to change volume     
    {
        FUNCTION_ADJUST(volume);
    }
    
    return volume;
}

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_ARCDeviceManager()
//  [Description]
//                 to switch audio devie from ARC to system device on board
//  [Arguments]:
//               TRUE: switch to use ARC device FALSE: switch off
//  [Return]:
//
//**************************************************************************

void _EWS_Sys_ARCDeviceManager(BOOLEAN setting)
{
    if(TRUE == setting)             /*swtich to use ARC audio device*/
    {
        FUNCTION_ON();
    }
    else                             /*swtich to use system audio device*/
    {
        FUNCTION_OFF();
    }
}

//****PS: NOS need move the inner method like " msAPI_CEC_ARC_AmpControl " 
//        to cec_new.c which realizes can use func in below******//

/*void msAPI_CEC_ARC_AmpControl(BOOLEAN b_status)
{
    if (TRUE == b_status)
    {
        MApp_Audio_Set_AmpMute(AUDIO_AMP_MUTE_ON);
        msAPI_CEC_SetSystemMute();
        msAPI_CEC_ARC_TX_Control(TRUE);
    }
    else
    {
        msAPI_CEC_ARC_TX_Control(FALSE);
        MApp_Audio_Set_AmpMute(AUDIO_AMP_MUTE_OFF);
        msAPI_CEC_SetSystemUnMute();
    }
}*/

//**************************************************************************
//  [Function Name]:
//                   _EWS_Sys_RestoreVolumeManager()
//  [Description]
//                 to Restore Volume value in system
//  [Arguments]:
//               TRUE: store current volume FALSE: restore and clear
//  [Return]:
//
//**************************************************************************
void _EWS_Sys_RestoreVolumeManager(BOOLEAN setting)
{
    if(TRUE == setting)             /*swtich to use ARC audio device*/
    {
        FUNCTION_ON();
    }
    else                             /*swtich to use system audio device*/
    {
        FUNCTION_OFF();
    }
}


/*
*********************************************************************************************************
*                                              USER RELATED
*[Description]:
*                In this model, user need to realize the events handle method witch would be conflict with EWS
*                make sure the function can be useful independent and actual to check and deal with conflct event
*********************************************************************************************************
*/


BOOLEAN _EWS_User_TimeShiftEventCheck(void)
{
    return CONDITION() ? FUNCTION_0() : FUNCTION_1();
}


void _EWS_User_StopTimeShift(void)
{
    FUNCTION_OFF();
}

#if EN_EWS_INPUT_FOR_CUSTOMER
U16 _EWS_User_GetScrollTextOffset(void)
{
    return FUNCTION_0();
}

#if (EN_EWS_CONTENT_FOCUS)
void EWS_Customer_UpKeyFocusAction(EWS_RANK rank)
{
    if (E_EWS_RANK_WASPADA == rank)
    {
        return;
    }

//     printf("SUCCESS focus_hwnd = %d", p_EWSSysContext->EWSStoreSetting.MarqueeFocusHwnd);
}

void EWS_Customer_DownKeyFocusAction(EWS_RANK rank)
{
    if (E_EWS_RANK_WASPADA == rank)
    {
        return;
    }

    // printf("SUCCESS focus_hwnd = %d", p_EWSSysContext->EWSStoreSetting.MarqueeFocusHwnd);
}
#endif

void _EWS_User_ScrollText(EWS_SCROLL_TYPE uTextScroll)
{
    U8 u8EwsScrollLineNum = 0;

    switch(uTextScroll)
    {
        case EN_EWS_SCROLL_RESET:
        {
            FUNCTION_ADJUST(0);
        }
            break;

        case EN_EWS_SCROLL_DOWN:
        {
            FUNCTION_ADJUST(1)
        }
            break;

        case EN_EWS_SCROLL_UP:
        {
            FUNCTION_ADJUST(2)
        }
            break;
    }
}

void _EWS_User_MoveScrollBar(U8 line, BOOLEAN option)
{
    //
}
#endif

void EWS_User_Clear_HwndData(void)
{
    //
}



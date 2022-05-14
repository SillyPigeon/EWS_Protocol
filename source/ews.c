/*
*********************************************************************************************************
*                                                
*
*                          
*                                          
*
* File    : ews.c
* By      : 
* Version : 
*
* 
* ---------------

*********************************************************************************************************
*/

#include "ews_func.h"


/*$PAGE*/
/*
*********************************************************************************************************
*                                         INNER DEFINE
*********************************************************************************************************
*/
#define EWS_DEBUG(x)                                //x
#define EWS_TYPE2BIT(x)                             (0x0001 << x)
#define EWS_PeriodTouch_CurTime()                   (p_EWSSysContext->u32EwsPeriodTime = _EWS_Sys_TimerGetCurTime())
#define EWS_Sys_TipAppend(p_str, p_source, count)   {\
                                                        if ((sizeof(p_source)+count) < EWS_DESC_BUF_SIZE)\
                                                        {\
                                                            snprintf((char*)(p_str+count), sizeof(p_source), p_source);\
                                                            count += (sizeof(p_source) - sizeof(char));\
                                                        }\
                                                    }

#define EWS_Sys_StringAppend(p_str, p_source, count) {\
                                                        EWS_STRCPY((char*)(p_str + count),\
                                                                   (char*)p_source,\
                                                                   (EWS_DESC_BUF_SIZE - sizeof(char)) - count);\
                                                        count = strlen((char*)p_str);\
                                                     }

/*$PAGE*/
/*
*********************************************************************************************************
*                                       STATIC VARIABLE
*********************************************************************************************************
*/

typedef struct _S_EWS_SYS_CONTEXT
{
    U32                      u32EwsPeriodTime;                         //Monitor period
    U32                      u8EwsCurrentRank      :8;
    U32                      u8EwsPreviousRank     :8;
    U32                      EWSSysStatusInfo      :16;                /* the main EWS related flag status      */
    S_EWS_SYS_SETTING        EWSStoreSetting;
    SI_EWSSETTING            stEwsSettingData;
    BOOLEAN                  bEwsAudioMuteControl  :1;
#if EN_EWS_EXIT_WASPADA_BY_ANYKEY
    BOOLEAN                  bEwsInfoPageHold      :1;
#endif
}S_EWS_SYS_CONTEXT;

static        S_EWS_SYS_CONTEXT                 s_EWSSysContext;

S_EWS_SYS_CONTEXT  * const  p_EWSSysContext  = (S_EWS_SYS_CONTEXT  *)&s_EWSSysContext;

/*
*********************************************************************************************************
*                                          FUNC DECLARE
*********************************************************************************************************
*/

//base
static void  EWS_Sys_Start(void);
static void  EWS_Sys_ForceSetting(void);
static void  EWS_Sys_ReleaseSetting(void);
static void  EWSStatusInfoSet (EWS_SYS_INFO_TAG);

//input execute
static EWS_INPUT  EWS_InputExec_ForSystem(EWS_INPUT);
static void EWS_Sys_EventHandle(U8);
static EWS_USER_INPUT_TYPE EWS_Sys_InputParse(EWS_INPUT);

//check
static BOOLEAN EWS_Sys_Is_NeedReset(void);
static BOOLEAN EWS_Sys_VolumeCheck(void);

//handler and manager
static BOOLEAN EWS_Sys_PeriodHandler(void);
static BOOLEAN EWS_Sys_AccessionHandler(void);
static BOOLEAN EWS_Sys_StatusHandler(void);
static BOOLEAN EWS_Sys_BehaviorHandler(EWS_RANK);

//user func
static U8 EWS_Sys_EventConflictCheck(void);
static void EWS_Sys_EventForceStop(U8);
#if EN_EWS_INPUT_FOR_CUSTOMER
static BOOLEAN EWS_InputChecker_ForCustomer(EWS_INPUT);
static void EWS_InputExec_ForCustomer(EWS_INPUT, EWS_RANK);
static void EWS_Customer_UpKeyDefaultAction(EWS_RANK);
static void EWS_Customer_DownKeyDefaultAction(EWS_RANK);
#endif

//Message func
static BOOLEAN EWS_Sys_Get_Description(U16*, U16);
static U16 EWS_Sys_Get_AuthorBitmap(void);
static U16 EWS_Sys_Get_DisasterBitmap(void);
/*$PAGE*/
/*
*********************************************************************************************************
*                                        BASE FUNCTION 
*********************************************************************************************************
*/


BOOLEAN  EWSStatusInfoGet (EWS_SYS_INFO_TAG sysinfotag)
{
    if((p_EWSSysContext->EWSSysStatusInfo & EWS_TYPE2BIT(sysinfotag)) > 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void  EWSStatusInfoSet (EWS_SYS_INFO_TAG sysinfotag)
{
    p_EWSSysContext->EWSSysStatusInfo &= ~(EWS_TYPE2BIT(sysinfotag));
    p_EWSSysContext->EWSSysStatusInfo |=  (EWS_TYPE2BIT(sysinfotag));
}

/*  do not use yet, reserve it
static void  EWSStatusInfoReset (EWS_SYS_INFO_TAG sysinfotag)
{
    p_EWSSysContext->EWSSysStatusInfo &= ~(EWS_TYPE2BIT(sysinfotag));
}*/

EWS_RANK  EWS_Sys_Get_CurRank (void)
{
    return p_EWSSysContext->u8EwsCurrentRank;
}

void  EWS_Sys_Set_PrevRank (U8 rank)
{
    p_EWSSysContext->u8EwsPreviousRank = rank;
}

EWS_RANK  EWS_Sys_Get_PrevsRank (void)
{
    return p_EWSSysContext->u8EwsPreviousRank;
}

SI_EWSSETTING* EWS_Sys_Get_EWSSettingData (void)
{
    return &p_EWSSysContext->stEwsSettingData;
}

/*
*********************************************************************************************************
*                                             MAIN FLOW
*********************************************************************************************************
*/

void EWS_init(void)
{
    EWS_MEMSET(p_EWSSysContext, 0, sizeof(S_EWS_SYS_CONTEXT));
}


static void EWS_Sys_Start(void)
{
    EWS_Sys_ForceSetting();
    EWSStatusInfoSet(E_EWS_SYSTEM_RUN);
}


void _EWS_Sys_Exit(void)
{
    if (FALSE == EWSStatusInfoGet(E_EWS_SYSTEM_RUN))
    {
        EWS_DEBUG(printf("WARNING !!! NOT RUNNING EXIT \n"));
        return;
    }
    SI_ExitEWS();
    EWS_Sys_ReleaseSetting();
    EWS_MEMSET(p_EWSSysContext, 0, sizeof(S_EWS_SYS_CONTEXT));
}

void EWS_TASK(void)
{   

    //Period Handler:Now is set one second period running
    if (FALSE == EWS_Sys_PeriodHandler())
    {
        return;
    }

    //Accession Handler:apply for runing EWS which may check conflict
    if (FALSE == EWS_Sys_AccessionHandler())
    {
        return;
    }

    //Status Handler:maintain the EWS status
    if(FALSE ==  EWS_Sys_StatusHandler())
    {
        return;
    }

    //Behavior Handler:adjust the EWS behavoir
    if(FALSE ==  EWS_Sys_BehaviorHandler(EWS_Sys_Get_CurRank()))
    {
        return;
    }
    EWS_DEBUG(printf("main p_EWSSysContext->EWSSysStatusInfo = %04x ",p_EWSSysContext->EWSSysStatusInfo));
    EWS_DEBUG(printf("currank = %d\n",p_EWSSysContext->u8EwsCurrentRank));
}

/*
*********************************************************************************************************
*                                             MAIN HANDLER
*********************************************************************************************************
*/

static BOOLEAN EWS_Sys_PeriodHandler(void)
{
    U32 diff_time = _EWS_Sys_DiffTimeFromNow(p_EWSSysContext->u32EwsPeriodTime);
    U32 period = (FALSE == EWSStatusInfoGet(E_EWS_ALARM_START)) ? EWS_NORMAL_PERIOD : EWS_URGENT_PERIOD;

    if (diff_time < period)
    {
        return FALSE;
        
    }
    
    EWS_PeriodTouch_CurTime();
    return TRUE;
}


static BOOLEAN EWS_Sys_AccessionHandler(void)
{
    U8 conflict_event = EWS_SYS_EVENT_NULL;

    //check system is accessible or not
    if (TRUE == EWS_Sys_Is_NeedReset())
    {
        EWS_Sys_Exit();
        return FALSE;
    }

    SI_GetEwsData(&p_EWSSysContext->stEwsSettingData);
    p_EWSSysContext->u8EwsCurrentRank = SI_GetEWSRank(p_EWSSysContext->stEwsSettingData);

    //check conflict which need depend on rank
    conflict_event = EWS_Sys_EventConflictCheck();
    if ((U8)EWS_SYS_EVENT_NULL != conflict_event)
    {
        EWS_Sys_EventHandle(conflict_event);
        return FALSE;
    }

    return TRUE;
}

static BOOLEAN EWS_Sys_StatusHandler(void)
{
    // EWS_FLAGS  result = (EWS_FLAGS)0;

    EWS_RANK currank   = EWS_Sys_Get_CurRank();
    EWS_RANK prevrank  = EWS_Sys_Get_PrevsRank();

    if (currank == prevrank)
    {
        return TRUE;
    }

    if((E_EWS_RANK_NONE == prevrank) || (E_EWS_RANK_INVALID == prevrank))
    {
        if ((E_EWS_RANK_NONE == currank) || (E_EWS_RANK_INVALID == currank))
        {
            return FALSE;
        }
        //RANK VALID
        EWS_Sys_Start();

        switch (currank)
        {
        case E_EWS_RANK_AWAS:
        {
            //visual
            _EWS_Sys_PannelMuteManager(TRUE); 
            _EWS_Sys_DisasterPageManager(TRUE);
            //audio
            _EWS_Sys_AlarmManager(TRUE);

            EWSStatusInfoSet(E_EWS_PANNEL_MUTE);
            EWSStatusInfoSet(E_EWS_ALARM_START);
        }
            break;
        case E_EWS_RANK_SIAGA:
        {
            _EWS_Sys_DisasterPageManager(TRUE);
            //audio
            _EWS_Sys_AlarmManager(TRUE);

            EWSStatusInfoSet(E_EWS_ALARM_START);
        }
            break;
        case E_EWS_RANK_WASPADA:
        {
          #if EN_EWS_EXIT_WASPADA_BY_ANYKEY
            p_EWSSysContext->bEwsInfoPageHold = TRUE;
          #endif
            _EWS_Sys_DisasterPageManager(TRUE);
        }
            break;
        default:
            break;
        }
        EWS_DEBUG(printf("SUCCESS TO SET currank = %d\n", currank));
    }
    else//Reset First
    {
        EWS_Sys_Exit();
    }

    EWS_Sys_Set_PrevRank(p_EWSSysContext->u8EwsCurrentRank);
    return FALSE;
}


static BOOLEAN EWS_Sys_BehaviorHandler(EWS_RANK rank)
{
    if((E_EWS_RANK_INVALID == rank) || (E_EWS_RANK_NONE == rank))
    {
        return FALSE;
    }

    if(FALSE == EWSStatusInfoGet(E_EWS_SYSTEM_RUN))
    {
        EWS_DEBUG(printf("\n//****WARNING !!! EWS does not run yet ****//\n"));
        return FALSE;
    }

    //step 1. check for UI display
    if(FALSE == _EWS_Sys_Has_UIShown(rank))
    {
        if (FALSE == EWSStatusInfoGet(E_EWS_CANCEL_BY_USER))
        {
            _EWS_Sys_ClearOSDPage();
            _EWS_Sys_DisasterPageManager(TRUE);
        }
    }
    else
    {
        if (TRUE == EWSStatusInfoGet(E_EWS_CANCEL_BY_USER))
        {
            _EWS_Sys_DisasterPageManager(FALSE);
        }
    }

    //step 2. check for pannel forbidden
    if (E_EWS_RANK_AWAS == rank)
    {
        if(FALSE == _EWS_Sys_PannelMuteChecker())
        {
            _EWS_Sys_PannelMuteManager(TRUE);
            _EWS_Sys_DisasterPageManager(TRUE);
        }
    }

    #if EN_EWS_EXIT_WASPADA_BY_ANYKEY
    if (E_EWS_RANK_WASPADA == rank)
    {
        if (p_EWSSysContext->bEwsInfoPageHold == TRUE)
        {
            p_EWSSysContext->bEwsInfoPageHold = FALSE;
        }
    }
    #endif

    //step 2. check for alarm
    if (E_EWS_RANK_WASPADA != rank)
    {
        BOOLEAN fin_check = _EWS_Sys_AlarmDoneChecker();
        if (TRUE == fin_check)
        {
            _EWS_Sys_AlarmManager(TRUE);
        }
        EWS_DEBUG(printf("check to MApi_AUDIO_CheckPlayDone() = %d\n", fin_check));

        if (FALSE == EWS_Sys_VolumeCheck())
        {
            _EWS_Sys_VolumeManager(EWS_VOLUME_VALUE, TRUE);
        }   
    }

    return TRUE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           CRITICAL   FUNCTIONG
*********************************************************************************************************
*/

static U8 EWS_Sys_EventConflictCheck(void)
{
    U8 event = (U8)EWS_SYS_EVENT_NULL;

    if(TRUE == _EWS_User_TimeShiftEventCheck())
    {
        event = (U8)EWS_SYS_EVENT_TIMESHIFT;
    }

    return event;
}


static void EWS_Sys_EventForceStop(U8 _event)
{
    switch(_event)
    {
    case EWS_SYS_EVENT_TIMESHIFT:
    {
        _EWS_User_StopTimeShift();
    }
        break;
    default:
        break;
    }

}


static void EWS_Sys_ForceSetting(void)
{
    S_EWS_SYS_SETTING* setting = &p_EWSSysContext->EWSStoreSetting;

    //UI check 
    if(_EWS_Sys_OSDActiveCheck())
    {
        _EWS_Sys_ClearOSDPage();
    }

    //Audio check
    if (EWS_Sys_Get_CurRank() != E_EWS_RANK_WASPADA)
    {
        if (_EWS_Sys_AudioMuteCheck())
        {
            setting->b_PreMuteByUser = TRUE;
            _EWS_Sys_AudioMuteManager(FALSE);
        }

        if (TRUE == _EWS_Sys_ParentalBlockCheck())
        {
            setting->b_PreLockByParent = TRUE;
            _EWS_Sys_VchipMuteManager(FALSE);
        }

        if (_EWS_Sys_ARCConnectCheck())
        {
            setting->b_PreConnectedARC = TRUE;
            _EWS_Sys_ARCDeviceManager(FALSE);
        }

        _EWS_Sys_RestoreVolumeManager(TRUE);
        _EWS_Sys_VolumeManager(EWS_VOLUME_VALUE,TRUE);
    }

    //user default
  #if EN_EWS_INPUT_FOR_CUSTOMER
    _EWS_User_ScrollText(EN_EWS_SCROLL_RESET);
  #endif
    // p_EWSSysContext->EWSStoreSetting.MarqueeFocusHwnd = MAX_NUM_OF_MARQUEE_LIST - 1;
}

static void EWS_Sys_ReleaseSetting(void)
{

    S_EWS_SYS_SETTING* setting = &p_EWSSysContext->EWSStoreSetting;
    if (setting == 0)
    {
        return;
    }

    if (TRUE == _EWS_Sys_Has_UIShown(EWS_Sys_Get_PrevsRank()))
    {
        EWS_User_Clear_HwndData();
        _EWS_Sys_DisasterPageManager(FALSE);
    }
    if (TRUE == EWSStatusInfoGet(E_EWS_PANNEL_MUTE))
    {   
        _EWS_Sys_PannelMuteManager(FALSE);
    }
    
    if (TRUE == EWSStatusInfoGet(E_EWS_ALARM_START))
    {
        _EWS_Sys_AlarmManager(FALSE);
    }

    //Audio check
    if (EWS_Sys_Get_CurRank() != E_EWS_RANK_WASPADA)
    {
        if (setting->b_PreConnectedARC)
        {
            _EWS_Sys_ARCDeviceManager(TRUE);
        }

        if(TRUE == _EWS_Sys_RestoreVolumeCheck())
        {
            _EWS_Sys_RestoreVolumeManager(FALSE);
        }

        if (setting->b_PreLockByParent)
        {
            _EWS_Sys_VchipMuteManager(TRUE);
        }

        if (setting->b_PreMuteByUser)
        {
            _EWS_Sys_AudioMuteManager(TRUE);
        }

    }
}

/*
*********************************************************************************************************
*                                          MAIN CHECK BLOCK
*********************************************************************************************************
*/

//需要重置状态
static BOOLEAN EWS_Sys_Is_NeedReset(void)
{
    if(FALSE == _EWS_Sys_DtvStatusCheck())
    {
        EWS_DEBUG(printf("It is not in DTV\n"));
        return TRUE;
    }

    if(_EWS_Sys_ScanStatusCheck())
    {
        EWS_DEBUG(printf("It is in Scan\n"));
        return TRUE;
    }

    if(_EWS_Sys_StandbyCheck())
    {
        EWS_DEBUG(printf("It is in Standby\n"));
        return TRUE;
    }

    if(_EWS_Sys_InstallStatusCheck())
    {
        EWS_DEBUG(printf("It is in Install\n"));
        return TRUE;
    }

//    if(MApp_GetSignalStatus()!=SIGNAL_LOCK) // audio only channel can't display
    if(_EWS_Sys_BadSignalCheck())
    {
        EWS_DEBUG(printf("It is BadSignal\n"));
        return TRUE;
    }

    if(_EWS_Sys_CodeSetStatusCheck())
    {
        EWS_DEBUG(printf("It is in code set\n"));
        //bypass: cancel WASPASA msg status
        return TRUE;
    }

    return FALSE;
}

static BOOLEAN EWS_Sys_VolumeCheck(void)
{
    BOOLEAN result = FALSE;
    if(_EWS_Sys_VolumeManager(EWS_VOLUME_VALUE,FALSE) >= EWS_VOLUME_VALUE)
    {
        result = TRUE;
    }
    return result;
}


/*
*********************************************************************************************************
*                                        INPUT AND EVENT HANDLER BLOCK
*********************************************************************************************************
*/

EWS_KEY EWS_UserInput_Handler(EWS_KEY key_input)
{
    EWS_USER_INPUT_TYPE type = EWS_INPUT_CONTROL_INVALID;
    EWS_RANK rank = EWS_Sys_Get_CurRank();
    EWS_KEY result = (EWS_KEY)INPUT_NULL;

    if(FALSE == _EWS_Sys_Has_UIShown(rank))
    {
        return key_input;
    }

    type = EWS_Sys_InputParse((EWS_INPUT)key_input);

  #if EN_EWS_INPUT_FOR_CUSTOMER
    if (EWS_INPUT_CONTROL_FOR_CUSTOMER == type)
    {
        EWS_InputExec_ForCustomer((EWS_INPUT)key_input, rank);
    }
  #endif
    //key convert
    switch (rank)
    {
    case E_EWS_RANK_AWAS:
    case E_EWS_RANK_SIAGA:
    {
        result = (EWS_KEY)INPUT_NULL;
    }
        break;
    case E_EWS_RANK_WASPADA:
    {
        result = EWS_InputExec_ForSystem((EWS_INPUT)key_input);
    }
        break;
    default:
        break;
    }

    return result;
}

static EWS_USER_INPUT_TYPE EWS_Sys_InputParse(EWS_INPUT key)
{
    EWS_USER_INPUT_TYPE input_type = EWS_INPUT_CONTROL_INVALID;
    switch(key)
    {
    case POWER_KEY:
    case EXIT_KEY:
    {
        input_type = EWS_INPUT_CONTROL_STATUS;
        goto EWS_PARSE_END;
    }
        break;
    default:
        break;
    }
  #if EN_EWS_INPUT_FOR_CUSTOMER
    if (EWS_InputChecker_ForCustomer((EWS_INPUT)key))
    {
        input_type = EWS_INPUT_CONTROL_FOR_CUSTOMER;
    }
  #endif
EWS_PARSE_END:    
    return input_type;
}


static EWS_INPUT  EWS_InputExec_ForSystem(EWS_INPUT key)
{
    EWS_INPUT result = INPUT_NULL;

  #if EN_EWS_EXIT_WASPADA_BY_ANYKEY
    if(E_EWS_RANK_WASPADA == EWS_Sys_Get_CurRank())
    {
        result = key;
        if (MUTE_KEY == key)
        {
            return result;
        }

        if(p_EWSSysContext->bEwsInfoPageHold == TRUE)
        {
            return INPUT_NULL;
        }

        EWSStatusInfoSet(E_EWS_CANCEL_BY_USER);
        if (_EWS_Sys_Has_UIShown(E_EWS_RANK_WASPADA))
        {
            EWS_User_Clear_HwndData();
            _EWS_Sys_DisasterPageManager(FALSE);
        }
    }
  #else
    switch(key)
    {
    case POWER_KEY:
    {
        result = POWER_KEY;
    }
        break;
    case EXIT_KEY:
    {
        if(E_EWS_RANK_WASPADA == EWS_Sys_Get_CurRank())
        {
            EWSStatusInfoSet(E_EWS_CANCEL_BY_USER); 
            if (_EWS_Sys_Has_UIShown(E_EWS_RANK_WASPADA))
            {
              #if EN_EWS_INPUT_FOR_CUSTOMER
                _EWS_User_ScrollText(EN_EWS_SCROLL_RESET);
              #endif
                _EWS_Sys_DisasterPageManager(FALSE);
            }         
        } 
        result = INPUT_NULL;//abandon
    }
        break;
    default:
        break;
    }
  #endif
    return result;
}

static void EWS_Sys_EventHandle(U8 conflict_event)
{
    if(EWS_Sys_Get_CurRank() == E_EWS_RANK_AWAS
    || EWS_Sys_Get_CurRank() == E_EWS_RANK_SIAGA)
    {
        EWS_Sys_EventForceStop(conflict_event);
    }
}

#if EN_EWS_INPUT_FOR_CUSTOMER
BOOLEAN EWS_InputChecker_ForCustomer(EWS_INPUT key_input)
{
    BOOLEAN result = FALSE;
    switch(key_input)
    {
    case UP_KEY:
    case DOWN_KEY:
    {
        result = TRUE;
    }
        break;
    default:
        break;
    }
    return result;
}


void EWS_InputExec_ForCustomer(EWS_INPUT key, EWS_RANK rank)
{
    switch(key)
    {
    case UP_KEY:
    {
      #if (EN_EWS_CONTENT_FOCUS)
        EWS_Customer_UpKeyFocusAction(rank);
      #endif

        EWS_Customer_UpKeyDefaultAction(rank);
    }
        break;
    case DOWN_KEY:
    {
      #if (EN_EWS_CONTENT_FOCUS)
        EWS_Customer_DownKeyFocusAction(rank);
      #endif

        EWS_Customer_DownKeyDefaultAction(rank);
    }
        break;
    default:
        break;
    }
}
#endif

/*
*********************************************************************************************************
*                                             USER ACTION
*********************************************************************************************************
*/
#if EN_EWS_INPUT_FOR_CUSTOMER
static void EWS_Customer_UpKeyDefaultAction(EWS_RANK rank)
{
    if (E_EWS_RANK_WASPADA != rank)
    {
        return;
    }

    _EWS_User_ScrollText(EN_EWS_SCROLL_UP);
}

static void EWS_Customer_DownKeyDefaultAction(EWS_RANK rank)
{
    if (E_EWS_RANK_WASPADA != rank)
    {
        return;
    }

    _EWS_User_ScrollText(EN_EWS_SCROLL_DOWN);
}
#endif



// /*
// *********************************************************************************************************
// *                                             UI method
// *********************************************************************************************************
// */

BOOLEAN EWS_Sys_Get_DisasterText(U16* pStrDest, U16 u16DestBufSize, EWS_TEXT_TYPE  text_type)
{
    BOOLEAN result = FALSE;
    U16     len = 0;
    switch(text_type)
    {
        case EWS_LOCATION_OF_DISASTER_TXT:
        {
            len = strlen((char *)EWS_Sys_Get_EWSSettingData()->u8_EwsLocationDisaster);
            if (len <= EWS_DESC_BUF_SIZE)
            {
                EWS_STRCPY_8to16(pStrDest,
                                 EWS_Sys_Get_EWSSettingData()->u8_EwsLocationDisaster,
                                 len);
                pStrDest[len] = '\0';
                result = TRUE;
            }
        }
            break;
        case EWS_TYPE_CONTENT_TXT:
        {
            len = strlen((char *)EWS_Sys_Get_EWSSettingData()->u8_EwsDisasterType);
            if (len <= EWS_DESC_BUF_SIZE)
            {
                EWS_STRCPY_8to16(pStrDest,
                                 EWS_Sys_Get_EWSSettingData()->u8_EwsDisasterType,
                                 len);
                pStrDest[len] = '\0';
                result = TRUE;
            }
        }
            break;
        case EWS_DATE_CONTENT_TXT:
        {
            len = strlen((char *)EWS_Sys_Get_EWSSettingData()->u8_EwsDisasterDate);
            if (len <= EWS_DESC_BUF_SIZE)
            {
                EWS_STRCPY_8to16(pStrDest,
                                 EWS_Sys_Get_EWSSettingData()->u8_EwsDisasterDate,
                                 len);
                pStrDest[len] = '\0';
                result = TRUE;
            }
        }
            break;
        case EWS_LONGITUDE_CONTENT_TXT:
        {
            len = strlen((char *)EWS_Sys_Get_EWSSettingData()->u8_EwsDisasterLongitude);
            if (len <= EWS_DESC_BUF_SIZE)
            {
                EWS_STRCPY_8to16(pStrDest,
                                 EWS_Sys_Get_EWSSettingData()->u8_EwsDisasterLongitude,
                                 len);
                pStrDest[len] = '\0';
                result = TRUE;
            }
        }
            break;
        case EWS_CHARACTER_CONTENT_TXT:
        {
            len = strlen((char *)EWS_Sys_Get_EWSSettingData()->u8_EwsDisasterCharacter);
            if (len <= EWS_DESC_BUF_SIZE)
            {
                EWS_STRCPY_8to16(pStrDest,
                                 EWS_Sys_Get_EWSSettingData()->u8_EwsDisasterCharacter,
                                 len);
                pStrDest[len] = '\0';
                result = TRUE;
            }
        }
            break;
        case EWS_REMARK_DESCRIPTION:
        {
            EWS_Sys_Get_Description(pStrDest, u16DestBufSize);
            result = TRUE;
        }
        default:
            break;
    }
    return result;
}

U16 EWS_Sys_Get_DisasterPicture(EWS_PICTURE_TYPE  picture_type)
{
    switch(picture_type)
    {
        case EWS_AUTHOR_SYMBOL:
            return EWS_Sys_Get_AuthorBitmap();

        case EWS_DISASTER_SYMBOL:
            return EWS_Sys_Get_DisasterBitmap();
        default:
            return 0xFFFF;
    }
}

static BOOLEAN EWS_Sys_Get_Description(U16* pStrDest, U16 u16DestBufSize )
{
    U16 strcount = 0;

    U16 u16CopySize = EWS_DESC_BUF_SIZE;

  #if EN_EWS_INPUT_FOR_CUSTOMER
    U16 u16BufOffset = _EWS_User_GetScrollTextOffset();
  #endif

    SI_EWSSETTING* p_EWSDisasterData = &p_EWSSysContext->stEwsSettingData;

    if (pStrDest == NULL)
    {
        return FALSE;
    }

    pStrDest[0] = 0;

    U8* p_u8EwsDescriptionBuf = EWS_MALLOC( sizeof(U8)*EWS_DESC_BUF_SIZE, BUF_ID_ZUI_STR_BUF );

    if (p_u8EwsDescriptionBuf == NULL)
    {
        return FALSE;
    }
                
    memset(p_u8EwsDescriptionBuf, 0, sizeof(U8)*EWS_DESC_BUF_SIZE );

    EWS_Sys_StringAppend(p_u8EwsDescriptionBuf, p_EWSDisasterData->u8_EwsDisasterType, strcount);
    if(p_EWSDisasterData->u8_EwsDisasterType[0]!=0)
    {
        EWS_Sys_TipAppend(p_u8EwsDescriptionBuf, ", ", strcount);
    }

    if (E_EWS_RANK_WASPADA == EWS_Sys_Get_CurRank())
    {

        EWS_Sys_TipAppend(p_u8EwsDescriptionBuf, "Tanggal : ", strcount);

        EWS_Sys_StringAppend(p_u8EwsDescriptionBuf, p_EWSDisasterData->u8_EwsDisasterDate, strcount);
        if(p_EWSDisasterData->u8_EwsDisasterDate[0]!=0)
        {
            EWS_Sys_TipAppend(p_u8EwsDescriptionBuf, ", ", strcount);
        }

        EWS_Sys_TipAppend(p_u8EwsDescriptionBuf, "Posisi : ", strcount);

        EWS_Sys_StringAppend(p_u8EwsDescriptionBuf, p_EWSDisasterData->u8_EwsDisasterLongitude, strcount);
        if(p_EWSDisasterData->u8_EwsDisasterLongitude[0]!=0)
        {
            EWS_Sys_TipAppend(p_u8EwsDescriptionBuf, ", ", strcount);
        }

        EWS_Sys_StringAppend(p_u8EwsDescriptionBuf, p_EWSDisasterData->u8_EwsDisasterCharacter, strcount);
    }

    EWS_Sys_StringAppend(p_u8EwsDescriptionBuf, p_EWSDisasterData->u8_EwsDisasterRemark, strcount);

    //Daerah Anda :
    if( strcount > (EWS_DESC_BUF_SIZE-20) )
    {
        p_u8EwsDescriptionBuf[strcount]=0;
        goto EWS_REMARK_COPY_STEP;
    }

    EWS_Sys_TipAppend(p_u8EwsDescriptionBuf, ", Daerah Anda : ", strcount);

    if( strcount > (EWS_DESC_BUF_SIZE-20) )
    {
        p_u8EwsDescriptionBuf[strcount]=0;
        goto EWS_REMARK_COPY_STEP;
    }

    switch(EWS_Sys_Get_CurRank())
    {
        case E_EWS_RANK_AWAS:
        {
            EWS_STRCPY((char*)(p_u8EwsDescriptionBuf + strcount), "STATUS AWAS", EWS_DESC_BUF_SIZE-strcount);
        }
            break;

        case E_EWS_RANK_SIAGA:
        {
            EWS_STRCPY((char*)(p_u8EwsDescriptionBuf + strcount), "STATUS SIAGA", EWS_DESC_BUF_SIZE-strcount);
        }
             break;

        case E_EWS_RANK_WASPADA:
        {
            EWS_STRCPY((char*)(p_u8EwsDescriptionBuf + strcount), "STATUS WASPADA", EWS_DESC_BUF_SIZE-strcount);
        }
            break;

        default:
        {
            p_u8EwsDescriptionBuf[strcount++]=0;
        } 
            break;
    }

EWS_REMARK_COPY_STEP:

  #if EN_EWS_INPUT_FOR_CUSTOMER
    u16CopySize = EWS_DESC_BUF_SIZE - u16BufOffset;
  #endif
    if( u16CopySize > u16DestBufSize )
    {
        u16CopySize = u16DestBufSize;
    }
  #if EN_EWS_INPUT_FOR_CUSTOMER
    EWS_DEBUG(printf("[%s][%d] addr:%d\n", __FUNCTION__, __LINE__, u16BufOffset));
    EWS_STRCPY_8to16(pStrDest, (p_u8EwsDescriptionBuf + u16BufOffset), u16CopySize);
  #else
    EWS_STRCPY_8to16(pStrDest, p_u8EwsDescriptionBuf, u16CopySize);
  #endif
    EWS_FREE(p_u8EwsDescriptionBuf, 0);   
    return TRUE;
}

static U16 EWS_Sys_Get_AuthorBitmap(void)
{
    switch(EWS_Sys_Get_EWSSettingData()->u8_EwsAuthor)//Author code
    {
        default:
        case 0x01:
            return EWS_BMP_Logo_BMKG;

        case 0x02:
            return EWS_BMP_Logo_BNPB;
    }
}

static U16 EWS_Sys_Get_DisasterBitmap(void)
{
    switch(EWS_Sys_Get_EWSSettingData()->u8_EwsDisasterSymbol)//Symbol code
    {
        case 0x01:
            return EWS_BMP_gempa;

        case 0x02:
            return EWS_BMP_tsunami;

        case 0x03:
            return EWS_BMP_gunung_meletus;

        case 0x04:
            return EWS_BMP_gerakan_tanah;

        case 0x05:
            return EWS_BMP_banjir;

        case 0x06:
            return EWS_BMP_kekeringan;

        case 0x07:
            return EWS_BMP_kebakaran_hutan;

        case 0x08:
            return EWS_BMP_erosi;

        case 0x09:
            return EWS_BMP_kebakaran_Gedung;

        case 0x0A:
            return EWS_BMP_gelombang_ekstrin_dan_abrasi;

        case 0x0B:
            return EWS_BMP_cuaca_eksrim;

        case 0x0C:
            return EWS_BMP_kegagalan_teknologi;

        case 0x0D:
            return EWS_BMP_Epidemi;

        case 0x0E:
            return EWS_BMP_konflik_sosial;

        case 0xFF:
        default:
            return EWS_BMP_cadangan;
    }
}
#ifndef   EWS_H
#define   EWS_H
/*
*********************************************************************************************************
*                                                 INFO ENUM
*********************************************************************************************************
*/

typedef enum _EWS_SYS_INFO_TAG
{
    E_EWS_IDLE                              = 0, /*meas EWS sys is in running or not*/
    // E_EWS_IR_FORBIDDEN                      = 1, /*EWS sys would abandon IR key from USER or not */
    // E_EWS_NOTIFY_DISPLAY                    = 2, /*EWS information was shown or not*/
    // E_EWS_MUTE_BY_USER                      = 3, /*the system was mute through user input or not*/
    E_EWS_CANCEL_BY_USER                    = 4, /*EWS information was canceled or not */
    E_EWS_ALARM_START                       = 5,
    E_EWS_PANNEL_MUTE                       = 6, /*EWS alarm was need or not */
    E_EWS_SYSTEM_RUN                        = 7, /*EWS alarm was need or not */
    E_EWS_INVALID_TAG                       = 15,
}EWS_SYS_INFO_TAG;

typedef enum 
{
	EWS_LOCATION_OF_DISASTER_TXT,
	EWS_TYPE_CONTENT_TXT,
	EWS_DATE_CONTENT_TXT,
	EWS_LONGITUDE_CONTENT_TXT,
	EWS_CHARACTER_CONTENT_TXT,
	EWS_REMARK_DESCRIPTION
}EWS_TEXT_TYPE;

typedef enum 
{
	EWS_DISASTER_SYMBOL = 0,
	EWS_AUTHOR_SYMBOL
}EWS_PICTURE_TYPE;

typedef enum _EWS_RANK
{
    E_EWS_RANK_INVALID                             = 0, /*meas EWS sys is in running or not*/
    E_EWS_RANK_AWAS,
    E_EWS_RANK_SIAGA,  
    E_EWS_RANK_WASPADA,
    E_EWS_RANK_NONE                                = 0xFF,                 
}EWS_RANK;

typedef  unsigned char                          BOOLEAN;                  /*  8-bit boolean or logical                            */
typedef  unsigned char                          U8;                       /*  8-bit unsigned integer                              */
typedef  unsigned short                         U16;                      /* 16-bit unsigned integer                              */
typedef  unsigned long                          U32;                      /* 32-bit unsigned integer                              */
typedef     U8                                  EWS_KEY;
typedef     U32                                 HWND;

/*
*********************************************************************************************************
*                                               BASE  FUNCTIONG  MACROS
*********************************************************************************************************
*/
#define  EWS_EXIT_DEBUG										   0

#define  SI_SetLocationCode(ptr_array)                         _EWS_Sys_Void_Func()  //

/*
*********************************************************************************************************
*                                             FUNCTIONG  DECLARE
*********************************************************************************************************
*/

#if EWS_EXIT_DEBUG
  #define EWS_Sys_Exit() do {\
    printf(">> WARNING!! EWS_Sys_Exit at"); printf("%lu:%s\n", __LINE__, __FILE__);\
    _EWS_Sys_Exit();\
  } while(0)
#else
  #define EWS_Sys_Exit() do {\
    _EWS_Sys_Exit();\
  } while(0)
#endif

//**************************************************************************
//  [Function Name]:
//                   EWS_TASK()
//  [Description]:
//                 the main handle flow to replace old method in project 
//  [Arguments]:
//                 NONE
//  [Return]:
//                 NONE
//**************************************************************************
void EWS_TASK(void);

//**************************************************************************
//  [Function Name]:
//                   EWS_Sys_Get_CurRank()
//  [Description]:
//                 get cur rank of EWS info, to deal with conflict 
//  [Arguments]:
//                 NONE
//  [Return]:
//                 EWS_RANK: WASPADA / SIAGA / AWAS
//**************************************************************************
EWS_RANK  EWS_Sys_Get_CurRank (void);

//**************************************************************************
//  [Function Name]:
//                   EWSStatusInfoGet()
//  [Description]:
//                 to get the flag defined in this model
//  [Arguments]:
//                 EWS_SYS_INFO_TAG: tag for whitch bit
//  [Return]:
//                 BOOLEAN: on/off
//**************************************************************************
BOOLEAN  EWSStatusInfoGet (EWS_SYS_INFO_TAG sysinfotag);

//**************************************************************************
//  [Function Name]:
//                   EWS_UserInput_Handler()
//  [Description]:
//                 option add in project,to deal with the key
//  [Arguments]:
//                 the key value input
//  [Return]:
//                 key value out_put
//**************************************************************************
U8 EWS_UserInput_Handler(U8 key_input);

//**************************************************************************
//  [Function Name]:
//                   EWS_Sys_Get_DisasterPicture()
//  [Description]:
//                 to get the parsed Disaster Picture
//  [Arguments]:
//                 DISASTER SYMBOL or AUTHOR SYMBOL
//  [Return]:
//                 the picture
//**************************************************************************
U16 EWS_Sys_Get_DisasterPicture(EWS_PICTURE_TYPE  picture_type);

//**************************************************************************
//  [Function Name]:
//                   EWS_Sys_Get_DisasterText()
//  [Description]:
//                 to get for parsed Disaster Text
//  [Arguments]:
//                 Destination, DestinationSize, EWS_TEXT_TYPE
//  [Return]:
//                 get success or not
//**************************************************************************
BOOLEAN EWS_Sys_Get_DisasterText(U16* pStrDest, U16 u16DestBufSize, EWS_TEXT_TYPE  text_type);    

#endif
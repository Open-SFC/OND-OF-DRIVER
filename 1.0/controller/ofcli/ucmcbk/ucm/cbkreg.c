#ifdef OF_CM_DP_CHANNEL_SUPPORT

#include"cbcmninc.h"
#include "dpcbk.h"


void UMDPInit(void);

struct cm_dm_channel_callbacks UCMDPCallbakFunctions = {
	 UCMDP_BeginConfigTransaction,
	 UCMDP_AddRecord,
	 UCMDP_SetRecord,
	 UCMDP_DelRecord,
	 UCMDP_EndConfigTransaction,
	 UCMDP_GetFirstNRecs,
	 UCMDP_GetNextNRecs,
	 UCMDP_GetExactRec,
	 UCMDP_VerifyConfig
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void UCMDPInit(void)
{
	 int32_t return_value;

	 CM_CBK_DEBUG_PRINT ("Entering");

	 return_value=cm_register_channel(CM_DP_CHANNEL_ID,&UCMDPCallbakFunctions);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT (" RegisterDataElements : ofpsdnDataElements, Error : %d", return_value);
			return;
	 }

	 CM_CBK_DEBUG_PRINT ("Registered DP Channel successfully");
}
#endif

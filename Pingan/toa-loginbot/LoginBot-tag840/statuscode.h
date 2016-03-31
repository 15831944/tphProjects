#ifndef _STATUS_CODE_H__
#define _STATUS_CODE_H__

enum StatusCode {
	OPEN_SUCCESS              = 101,
	OPEN_FAILE                = 102,

	LOG_SUCCESS               = 200,
	TASK_WORKING              = 201,
	TASK_SUCCESS              = 202,

	LOG_FAILE                 = 401,	
	TASK_FAILE                = 402,	
	WAIT_VCODE                = 4010,
	LOG_FAILE_VCODEERR        = 40101,

	WAIT_PHONENUM             = 4011,
	LOG_FAILE_PHONENUMERR     = 40111,

	LOG_FAILE_USERNAMEERR     = 4012,
	LOG_FAILE_USERPASSWORDERR = 4013, 
	LOG_FAILE_MAXLOGINFAIL    = 4014,
	LOG_FAILE_WAIT_ACTIVE     = 4015,
	LOG_FAILE_OCR_FAIL        = 4016,
	LOG_FAILE_NOTFINDTYPE     = 4997,
	LOG_FAILE_UNDEF           = 4999,

	WAIT_OUTTIME              = 504,

    PROCESS_EXIT              = 601,
    PROCESS_KEEP_ALIVE        = 602,    

    LOG_FAILE_CATCHFAIL       = 800,
    LOG_FAILE_TIMEOUT         = 801,
    LOG_FAILE_PAGEERR         = 802,
    LOG_FAILE_VERIFINOERR     = 803,  
    LOG_FAILE_BRANCHERR       = 804,  
    LOG_FAILE_NEEDPHONEVERIFY = 805,
    LOG_FAILE_RELOGIN         = 806,
};

#endif
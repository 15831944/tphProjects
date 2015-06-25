#include <string.h>
#include "exception.h"


void Exception::getMessage (char *string) const
{
    strcpy (string, errorType);
    strcat (string, errorName);

    if (message)
    {
        strcat (string, " ");
        strcat (string, message);
    }
}

std::ostream& operator << (std::ostream& s, const Exception& p_error)
{
    char string[256];
    p_error.getMessage (string);
    return s << '\n' << string;
}

TwoStringError::TwoStringError (const char *p_msg1, const char *p_msg2) :
    ARCError (NULL)
{
    errorName = ":";

    tempString = new char [strlen (p_msg1) + strlen (p_msg2) + 1];
    strcpy (tempString, p_msg1);
    strcpy (tempString + strlen (p_msg1), p_msg2);
    message = tempString;
}

// #if 0
// // By default matherr ignores underflow and tloss math errors
// int _matherr (struct exception *e)
// {
//     UnderflowError ufErr (e->name);
//     TotalLossError tlErr (e->name);
//     switch (e->type)
//     {
//         case DOMAIN:
// //          cerr << (DomainError (e->name));
// //          break;
//               throw new DomainError (e->name);
//         case SING:
// //          cerr << (SingularityError (e->name));
// //          break;
//               throw new SingularityError (e->name);
//         case ARC_OVERFLOW:
// //          cerr << (OverflowError (e->name));
// //          break;
//             throw new OverflowError (e->name);
//         case UNDERFLOW:
//             cerr << ufErr;
//             break;
//         case TLOSS:
//             cerr << tlErr;
//             break;
//     }
//     return 1;
// }
// 
// int _matherrl (struct _exceptionl *e)
// {
//     UnderflowError ufErr (e->name);
//     TotalLossError tlErr (e->name);
//     switch (e->type)
//     {
//         case DOMAIN:
// //            cerr << (DomainError (e->name));
// //            break;
//             throw new DomainError (e->name);
//         case SING:
// //            cerr << (SingularityError (e->name));
// //            break;
//             throw new SingularityError (e->name);
//         case OVERFLOW:
// //            cerr << (OverflowError (e->name));
// //            break;
//             throw new OverflowError (e->name);
//         case UNDERFLOW:
//             cerr << ufErr;
//             break;
//         case TLOSS:
//             cerr << tlErr;
//             break;
//     }
//     return 1;
// }
// #endif

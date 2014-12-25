#ifndef PAM300SHARE_H
#define PAM300SHARE_H

#include "./PAM300Share_global.h"

class PAM300SHARESHARED_EXPORT PAM300Share {
public:
    PAM300Share();
    StruOutPara CalcVIValue(StruInputPara para);
};


#endif // PAM300SHARE_H

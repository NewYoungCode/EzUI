#pragma once
#include "global.h"
#include "Application.h"
class MainFrm :
    public Form
{
protected:

    Label b1;
    Label b2;

    HLayout title;
    VLayout layout;
    VList list;
    std::map<EString, EString> maps;
    bool frist = true;
public:
    MainFrm(int width, int height) ;
    void OnLoad() override;
    bool test1(const EString& timeconst, const EString& userName, const EString& text);
    void OnClose(bool& close);
};

extern void NS_BaseInit();
extern void NS_BaseFini();
extern void NS_ListInit();
extern void NS_ListFini();
extern void NS_CfgInit();
extern void NS_CfgFini();
extern void nsnullfun();
extern void nsnullfun();
extern void NS_TimeInit();
extern void NS_TimeFini();
struct tagstartlist {
    void * pinit;
    void * pfini;
    char * name;
};
struct tagstartlist g_aStartList[] = {
   {NS_BaseInit,NS_BaseFini,"d_def"},
   {NS_ListInit,NS_ListFini,"d_list"},
   {NS_CfgInit,NS_CfgFini,"d_cfg"},
   {nsnullfun,nsnullfun,"d_root"},
   {NS_TimeInit,NS_TimeFini,"d_time"},
   {0,0,0}
};

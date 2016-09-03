extern void NS_BaseInit();
extern void NS_BaseFini();
extern void NS_ListInit();
extern void NS_ListFini();
extern void NS_CfgInit();
extern void NS_CfgFini();
extern void NS_LockInit();
extern void NS_LockFini();
extern void NS_TimeInit();
extern void NS_TimeFini();
extern void nsnullfun();
extern void nsnullfun();
extern void NS_TaskInit();
extern void NS_TaskFini();
extern void NS_NameServerInit();
extern void NS_NameServerFini();
struct tagstartlist {
    void * pinit;
    void * pfini;
    char * name;
};
struct tagstartlist g_aStartList[] = {
   {NS_BaseInit,NS_BaseFini,"d_def"},
   {NS_ListInit,NS_ListFini,"d_list"},
   {NS_CfgInit,NS_CfgFini,"d_cfg"},
   {NS_LockInit,NS_LockFini,"d_lock"},
   {NS_TimeInit,NS_TimeFini,"d_time"},
   {nsnullfun,nsnullfun,"d_root"},
   {NS_TaskInit,NS_TaskFini,"d_task"},
   {NS_NameServerInit,NS_NameServerFini,"d_ns"},
   {0,0,0}
};

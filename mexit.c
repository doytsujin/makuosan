/*
 * mexit.c
 * Copyright (C) 2008 KLab Inc.
 */
#include "makuosan.h"

static void chexit()
{
  struct utsname uts;
  char cwd[PATH_MAX];
  if(moption.chroot){
    if(uname(&uts) == -1){
      return;
    }
    if(strcmp("Linux", uts.sysname)){
      return;
    }
    /*----- chroot exit(linux only) -----*/
    mtempname("",".MAKUOWORK",cwd);
    mkdir(cwd,0700);
    chroot(cwd);
    rmdir(cwd);
    chdir("..");
    getcwd(cwd,PATH_MAX);
    while(strcmp("/", cwd)){
      chdir("..");
      getcwd(cwd,PATH_MAX);
    }
    chroot(".");
  }
  return;
}

static void restoreguid()
{
  if(getuid() != geteuid())
    seteuid(getuid());
  if(getgid() != getegid())
    setegid(getgid());
}

static void sock_clean()
{
  socklen_t namelen;
  struct sockaddr_un addr;
  /*----- unlink unix domain socket -----*/
  namelen = sizeof(addr);
  if(!getsockname(moption.lisocket, (struct sockaddr *)&addr, &namelen)){
    if(addr.sun_family == AF_UNIX){
      unlink(addr.sun_path);
    }
  }
  /*----- close -----*/
  close(moption.mcsocket);
  close(moption.lisocket);
}

void mexit()
{
  lprintf(0, "%s: shutdown start\n", __func__);
  msend_clean(); /* recv object free       */
  mrecv_clean(); /* recv object free       */
  restoreguid(); /* restore euid,egid      */
  chexit();      /* exit chroot(LinuxOnly) */
  sock_clean();  /* atoshimatsu            */
  lprintf(0, "%s: shutdown complete\n", __func__);
}

#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/version.h>
#include <linux/fs.h>
#include "kpath.h"

/*
 *get current work-directory
 */
static char* get_pwd_pathname(unsigned* plen)
{
    struct path pwd;
    char* kpathname = ERR_PTR(-EINVAL);

    if(!plen) { goto out; }

    kget_fs_pwd(current->fs,&pwd);
    kpathname = kget_pathname(&pwd,plen);
    kpath_put(&pwd);

out:
    return kpathname;
}

/*
 *get real-path of the path
 *Note: the real-path is may be not an existing path
 */
static char* get_real_pathname(const char* path,unsigned* reslen)
{
    int rc = 0;
    char *npath = NULL;
    unsigned pwdpathlen = 0;
    char* res = ERR_PTR(-EINVAL);
    unsigned maxreslen = PATH_MAX;
    char* pwdpath = ERR_PTR(-ENOENT);

    if(!path) { goto out; }

    rc = -ENOMEM;
    res = __getname();
    if(!res) { goto out; }

    npath = res;
    if (*path != '/') {
        pwdpath = get_pwd_pathname(&pwdpathlen);
        if(IS_ERR(pwdpath)) {
            rc = PTR_ERR(pwdpath);
            goto out;
        }

        if(pwdpathlen > maxreslen - 2) {
            rc = -ENAMETOOLONG;
            goto out;
        }
        memcpy(npath,pwdpath,pwdpathlen);

        npath += pwdpathlen;
        if (npath[-1] != '/') {
          *npath++ = '/';
        }
    } else {
        *npath++ = '/';
        path++;
    }

    while (*path != '\0') {
        if (*path == '/') {
          path++;
          continue;
        }

        if (*path == '.' && (path[1] == '\0' || path[1] == '/')) {
          path++;
          continue;
        }

        if (*path == '.' && path[1] == '.' && (path[2] == '\0' || path[2] == '/')) {
          path += 2;
          while (npath > res + 1 && (--npath)[-1] != '/')
            ;
          continue;
        }

        while (*path != '\0' && *path != '/') {
          if (npath - res > maxreslen - 2) {
            rc = -ENAMETOOLONG;
            goto out;
          }
          *npath++ = *path++;
        }
        *npath++ = '/';
    }

    if (npath != res + 1 && npath[-1] == '/') {
        npath--;
    }

    rc = 0;
    *npath = '\0';
    *reslen = (npath - res);

out:
    if(rc) {
        if(res) { kput_pathname(res); }
        res = ERR_PTR(rc);
    }
    if(!IS_ERR(pwdpath)) { kput_pathname(pwdpath); }

    return res;
}

static char spath[1024] = {0};
#define DEVICE_NAME     "krealpath"

static int __init kreal_path_init(void)
{
    unsigned len = 0;
    char* rpath = NULL;

    printk("-----Start krealpath: %s,"
        "kernel-version: %s\n",spath,UTS_RELEASE);

    rpath = get_real_pathname(spath,&len);
    if(!IS_ERR(rpath)) {
        printk("real path for path [%s] is %s\n",
            spath,rpath);
        kput_pathname(rpath);
    }

    return 0;
}

static void __exit kreal_path_exit(void)
{
    printk("-----Exit krealpath-----\n");
}

module_param_string(spath,spath,sizeof(spath),0);
MODULE_PARM_DESC(spath, "original path");

module_init(kreal_path_init);
module_exit(kreal_path_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("qudreams");
MODULE_DESCRIPTION(DEVICE_NAME);
MODULE_VERSION(DEVICE_VERSION);

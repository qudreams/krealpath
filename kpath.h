/*
 *ktask_cmdline.h: 2019-02-21 created by qudreams
 *
 *process file-path in kernel
 */


#ifndef KPATH_H
#define KPATH_H

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/namei.h>
#include <linux/dcache.h>
#include <linux/mount.h>

/*
 *struct path在2.6.20之前的内核中是没有定义的.
 *2.6.25之前的内核struct path定义在linux/namei.h头文件中
 *从2.6.25开始struct path定义在linux/path.h头文件中
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
#include <linux/path.h> //for struct path
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
struct path {
	struct vfsmount *mnt;
	struct dentry *dentry;
};
#endif

void kput_pathname(const char* pathname);
/*
 *get struct path by struct filp
 *Caller must call kpath_put to free the return-value "path"
 */
int kfilp_path(struct file* filp,struct path* path);
void kpath_put(struct path* path);

/*
 *get pathname by struct path
 *Caller must ensure that "path" is pinned before calling kget_pathname(),
 *and Caller must call kput_pathname to free the return-value
 */
char* kget_pathname(struct path* path,unsigned int* len);

/*
 *get pathname by struct file
 *Caller must call kput_pathname to free the return-value
 */
char* kfilp_pathname(struct file* filp,unsigned int* pathlen);

/*
 *get file system pwd by fs_struct
 * Caller must call kbase_path_put to free the return-value "pwd"
 */
void kget_fs_pwd(struct fs_struct* fs,struct path* pwd);
#endif

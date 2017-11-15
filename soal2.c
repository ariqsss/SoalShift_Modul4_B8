#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

static const char *dirpath = "/home/ariq/Documents";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}
static int xmp_chmod();
static int xmp_rename();
static int xmp_mkdir();
static int xmp_link();
static int xmp_unlink();

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[1000];
  char alamat[1000];
  char namafile[20];
	if(strcmp(path,"/") == 0){
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else {
	sprintf(namafile,"%s",path);
	sprintf(fpath, "%s%s",dirpath,path);
	}
	int res = 0;
	int fd = 0 ;
	int len=strlen(fpath);
	char *last_four = &fpath[len-4];
	(void) fi;
	fd = open(fpath, O_RDONLY);
                if (fd == -1 ) return -errno;
        if ( strcmp(last_four,".txt") != 0 && strcmp(last_four,".doc")!=0 && strcmp(last_four,".pdf")!=0){
 		//kalau bukan ya open seperti biasa
 		/*fd = open(fpath, O_RDONLY);
 		if (fd == -1 ) return -errno;*/
 		res = pread (fd,buf,size,offset);
 		if (res == -1) res=-errno;
 		close(fd);
 	}
 	else { //kalau iya maka lakukan perintahnya
 	char newname[100];//, *errorku=NULL , iferror[100]="Terjadi kesalahan! File berisi konten berbahaya.\n";
// 	errorku = iferror;
 //	memcpy(buf, errorku+offset , size);
 	memcpy(newname,fpath,strlen(fpath));
 	sprintf(newname,"%s.ditandai",newname);
	xmp_rename(fpath,newname);
	xmp_chmod(newname,0333);
	sprintf(alamat,"%s/rahasia",dirpath);
	xmp_mkdir(alamat,0755);
	sprintf(alamat,"%s/rahasia/%s.ditandai",dirpath,namafile);
	xmp_link(newname,alamat);
	xmp_unlink(newname);
	xmp_chmod(alamat,0000);
	system("zenity --error --text='Terjadi kesalahan! File berisi konten berbahaya.\n'");
	close(fd);
	return 0; 
	}
return res;

}
static int xmp_rename(const char *from, const char *to)
{
	int res;

	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}
static int xmp_chmod(const char *path, mode_t mode)
{
	int res;

	res = chmod(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}
static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;

	res = mkdir(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}
static int xmp_unlink(const char *path)
{
	int res;

	res = unlink(path);
	if (res == -1)
		return -errno;

	return 0;
}
static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	
	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}



static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.rename		= xmp_rename,
	.chmod		= xmp_chmod,
	.mkdir		= xmp_mkdir,
	.link		= xmp_link,
	.unlink		= xmp_unlink,
	.mknod		= xmp_mknod,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}

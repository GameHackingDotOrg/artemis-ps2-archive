
#include <stdio.h>
#include <ioman.h>
#include <errno.h>
#include <io_common.h>

const char dev_name[] = "MDISK\0";

// img driver ops functions prototypes
int mdisk_dummy(void);
int mdisk_read(iop_file_t *f, void *buf, u32 size);
int mdisk_lseek(iop_file_t *f, u32 pos, int where);

// driver ops func tab
void *mdiskdev_ops[10] = {
	(void*)mdisk_dummy,
	(void*)mdisk_dummy,
	NULL,
	(void*)mdisk_dummy,
	(void*)mdisk_dummy,
	(void*)mdisk_read,
	NULL,
	(void*)mdisk_lseek,
	NULL,
	NULL
};

// driver descriptor
static iop_device_t mdiskdev = {
	dev_name, 
	IOP_DT_FS,
	1,
	dev_name,
	(struct _iop_device_ops *)&mdiskdev_ops
};

typedef struct {
	void *buf;
	u32 pad1;
	u32 pad2;	
	int size; 
} ioprp_img_t;

static ioprp_img_t ioprpimg = {
	(void *)NULL,
	0,
	0,
	0
};

IRX_ID(dev_name, 1, 1);

//-------------------------------------------------------------------------
int _start(int argc, char** argv)
{	
	register int r;
	
	r = AddDrv(&mdiskdev);	
								
	return r >> 31;
}

//-------------------------------------------------------------------------
int mdisk_dummy(void)
{
	return 0;
}

//-------------------------------------------------------------------------
int mdisk_read(iop_file_t *f, void *buf, u32 size)
{
	register int i;
	void *ioprp_img;
	
	ioprp_img = ioprpimg.buf;
	
	for (i = size; i > 0; i -= 4) {
		*((u32 *)buf) = *((u32 *)ioprp_img);
		buf += 4;
		ioprp_img += 4;
	}
	
	return size;
}

//-------------------------------------------------------------------------
int mdisk_lseek(iop_file_t *f, u32 pos, int where)
{
	if (where == SEEK_SET)
		return 0;
		
	return ioprpimg.size;	
}

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <lv2_syscall.h>

struct lv2_atapi_cmnd_block {
    uint8_t pkt[0x20]; /* packet command block           */ 
    uint32_t pktlen;  
    uint32_t blocks;					
    uint32_t block_size;				
    uint32_t proto; /* transfer mode                  */ 
    uint32_t in_out; /* transfer direction             */ 
    uint32_t unknown;
} __attribute__((packed));

int ps3rom_lv2_get_inquiry(int fd, uint8_t *buffer) {
    int res;
    struct lv2_atapi_cmnd_block atapi_cmnd;

    init_atapi_cmnd_block(&atapi_cmnd, 0x3C, 1, 1);
    atapi_cmnd.pkt[0] = 0x12;
    atapi_cmnd.pkt[1] = 0;
    atapi_cmnd.pkt[2] = 0;
    atapi_cmnd.pkt[3] = 0;
    atapi_cmnd.pkt[4] = 0x3C;

    res = sys_storage_send_atapi_command(fd, &atapi_cmnd, buffer);
    return res;
}



int sys_storage_send_atapi_command(uint32_t fd, struct lv2_atapi_cmnd_block *atapi_cmnd, uint8_t *buffer) 
{
    uint64_t tag;
    system_call_7(0x25C, fd, 1, (uint32_t) atapi_cmnd , sizeof (struct lv2_atapi_cmnd_block), (uint32_t) buffer, atapi_cmnd->block_size, (uint32_t) & tag);
    return_to_user_prog(int);
}



void main ()
{
	// this poke allows us to use storage open on 4.21
	lv2_poke(0x8000000000017B2CULL,0x386000014e800020ULL );
	printf("lv2 poked...\n");

	int fd;
	int ret;
	uint8_t buf[0x38];
	memset(buf,0,0x38);


	// open Blu Ray Drive
	ret = sys_storage_open(0x101000000000006ULL,&fd);
	if(ret != 0)
	{
		printf("sys_storage_open failed (0x%x)...\n", ret);
		return;
	}

	// inquiry command
	ret = ps3rom_lv2_get_inquiry(fd,buf);
	if(ret != 0)
	{
		printf("sys_storage_send_device_command failed (0x%x)...\n", ret);
		return;
	}

	// close device
	sys_storage_close(fd);


	// dump result to file
	FILE * pFile;
	pFile = fopen ( "/dev_hdd0/game/myfile.bin" , "wb" );
	fwrite (buf , 1 , sizeof(buf) , pFile );
	fclose (pFile);
	print("file written...\n");
}
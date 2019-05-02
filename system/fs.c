#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#ifdef FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;

unsigned short int  fd_bitmap = 0xFFFF;

#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */


int get_free_fd() {
 
 int counter = 0;
 unsigned short int bitmap = fd_bitmap;

 while(bitmap > 1) {
  counter++;
  bitmap = bitmap >> 1;
 }

 // toggle the bit at that position
 fd_bitmap ^= 1 << counter;

 return counter;

}


void free_fd(int fd) {

  fd_bitmap ^= 1 << fd;
}

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);

  
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}


int fs_open(char *filename, int flags) {
	//first check if the filename is correct
	int i;
	int inode_num = -1;
	for (i = 0; i < fsd.root_dir.numentries; i++) {
		if (!strcmp(filename,fsd.root_dir.entry[i].name)) {
			inode_num = fsd.root_dir.entry[i].inode_num;
		}
	}
	int new_fd = get_free_fd();
	oft[new_fd].state = FSTATE_OPEN;
	oft[new_fd].fileptr = 0;
	oft[new_fd].de = &(fsd.root_dir.entry[inode_num]);
	struct inode in;
	//get the inode value;
	fs_get_inode_by_num(0, inode_num, &in);
	oft[new_fd].in.id = in.id;
	oft[new_fd].in.type = in.type;
	oft[new_fd].in.nlink = in.nlink;
	oft[new_fd].in.device = in.device;
	oft[new_fd].in.size= in.size;
	for (i = 0; i < INODEBLOCKS; i++){
		oft[new_fd].in.blocks[i] = in.blocks[i];
	}
	next_open_fd++; 
	return new_fd;
}

int fs_close(int fd) {
	//check if the file is open and if the given file descriptor is valid
	if (oft[fd].state != FSTATE_OPEN || fd < 0 || fd >= NUM_FD) {
		return SYSERR;
	}
	//reset to base values
	oft[fd].in.id = 0;
	oft[fd].in.type = FSTATE_CLOSED;
	oft[fd].in.nlink = 0;
	oft[fd].in.device = 0;
	oft[fd].in.size = 0;
	int i;
	
	for (i = 0;i < INODEBLOCKS; i++) {
		oft[fd].in.blocks[i] = 0;
	}
	oft[fd].state = FSTATE_CLOSED;
	oft[fd].fileptr = 0;
	oft[fd].de = NULL;
	free_fd(fd);
	next_open_fd--;
	return OK;
}

int fs_create(char *filename, int mode) {
	//first check if valid
	if (next_open_fd > NUM_FD || fsd.inodes_used > fsd.ninodes) {
		return SYSERR;
	}
	//check if the file already exists
	int i;
	for (i = 0; i < fsd.root_dir.numentries; i++) {
		if (!strcmp(filename,fsd.root_dir.entry[i].name)) {
			printf("File already exists.\n");
			return SYSERR;
		}
	}
	
	//add the file to the directory
	fsd.inodes_used ++;
	fsd.root_dir.entry[fsd.root_dir.numentries].inode_num = fsd.inodes_used;
	strcpy(fsd.root_dir.entry[fsd.root_dir.numentries].name, filename);
	
	int new_fd = get_free_fd();
	oft[new_fd].state = FSTATE_OPEN;
	oft[new_fd].fileptr = 0;
	oft[new_fd].de = &(fsd.root_dir.entry[fsd.root_dir.numentries++]);
	struct inode in;
	in.id = fsd.inodes_used;
	in.type = INODE_TYPE_FILE;
	in.nlink = 0;
	in.device = 0;
	in.size = 0;
	oft[new_fd].in = in;
	
	int block_number = FIRST_INODE_BLOCK + fsd.inodes_used / INODES_PER_BLOCK;
	fs_setmaskbit(block_number);
	next_open_fd++;
	return new_fd;
}

int fs_seek(int fd, int offset) {
	//check if file descriptor is valid
	if (fd < 0 || fd > NUM_FD) {
		return SYSERR;
	}
	int new_begin = oft[fd].fileptr + offset;
	if (new_begin > oft[fd].in.size || new_begin < 0) {
		return SYSERR;
	}
	oft[fd].fileptr = new_begin;
	return OK;
}

int fs_read(int fd, void *buf, int nbytes) {
	//check if file descriptor is valid
	if (fd < 0 || fd > NUM_FD) {
		return SYSERR;
	}
	//use the file pointer to determine which block to read from
	int num_block = oft[fd].fileptr / MDEV_BLOCK_SIZE;
	int offset = oft[fd].fileptr % MDEV_BLOCK_SIZE;
	int block = oft[fd].in.blocks[num_block];
	int read_bytes = 0;
	int total_bytes = nbytes < MDEV_BLOCK_SIZE ? nbytes : MDEV_BLOCK_SIZE - offset;
	
	while (read_bytes < nbytes && block != 0) {
		total_bytes = total_bytes > MDEV_BLOCK_SIZE ? MDEV_BLOCK_SIZE : total_bytes;
		//let's get this bread
		bs_bread(0, block, offset, buf, total_bytes);
		read_bytes += total_bytes;
		//update block number and offset
		num_block++;
		if (num_block > INODEBLOCKS) {
			break;
		}
		block = oft[fd].in.blocks[num_block];
		offset = 0;
		buf += total_bytes;
		total_bytes = nbytes - read_bytes;
	}
	
	oft[fd].fileptr += read_bytes;
	return read_bytes;
}

int fs_write(int fd, void *buf, int nbytes) {
	//check if the fd is valid
	if (fd < 0 || fd > NUM_FD) {
		return SYSERR;
	}
	//clear the file
	if (oft[fd].in.size > 0) {
		int i= 0;
		for (i = 0; i < INODEBLOCKS; i++) {
			fs_clearmaskbit(oft[fd].in.blocks[i]);
			oft[fd].in.blocks[i] = 0;
		} 
		oft[fd].in.size = 0;
	}
	//get some basic info
	int blocks = nbytes % MDEV_BLOCK_SIZE == 0? nbytes / MDEV_BLOCK_SIZE : (nbytes / MDEV_BLOCK_SIZE) + 1;
	int start = FIRST_INODE_BLOCK + NUM_INODE_BLOCKS;
	int bytes_written = 0;
	int blocks_written = 0;
	int total_bytes = nbytes;
	int i = 0;
	
	for (; start < MDEV_NUM_BLOCKS && blocks_written < blocks; start++) {
		if (fs_getmaskbit(start) == 0) {
			total_bytes = total_bytes > MDEV_BLOCK_SIZE ? MDEV_BLOCK_SIZE : total_bytes; 
			//clear the block
			memset(block_cache, 0, MDEV_BLOCK_SIZE);
			bs_bwrite(0, start, 0, &block_cache, MDEV_BLOCK_SIZE);
			bs_bwrite(0, start, 0, buf, MDEV_BLOCK_SIZE);
			fs_setmaskbit(start);
			oft[fd].in.blocks[i++] = start;
			buf += total_bytes;
			
			//determine how many blocks were written
			bytes_written += total_bytes;
			total_bytes = nbytes - bytes_written;
			blocks_written += 1;     
		}
	}
	
	//calculate new size of file
	oft[fd].in.size = bytes_written;
	oft[fd].fileptr = bytes_written;
	return bytes_written;
}

#endif /* FS */

// SDpatch.cpp : Defines the entry point for the console application.
// by Costis. Copyright 2005.
// Ported to GNU/Linux by Daniel Thompson (Merlin).
// 
// This is the source code for the SDpatch.exe program described in
// the install.txt documentation. Feel free to port it to any OS of
// your choice and send it to me for release!
//


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static const char *OS_version = "GNU/Linux";

// Usage.
void usage (void)
{
	printf ("Usage: SDpatch <device>\n\n");
	printf ("\tdevice\tSD card device node mapped by the kernel (see dmesg).\n\n");
	printf ("WARNING: If you choose the wrong device, such as\n");
	printf ("\t one mapped to a hard drive, there is a large\n");
	printf ("\t possibility that the drive contents will become\n");
	printf ("\t corrupted!!! CHOOSE THE DEVICE CAREFULLY!\n");
	exit (0);
}

// This is the level 2 boot-code for file systems without MBR's.
const int sd_lev2_vbr_size = 352;
const unsigned char sd_lev2_vbr[352] = {0x90, 0x63, 0x68, 0x00, 0x7D, 0x3B, 0x4B, 0x78, 0x3A, 0x20, 0x02, 0x02, 0x3D, 0x20, 0x80, 0x10, 
									    0x3B, 0x40, 0xFF, 0xD6, 0x7F, 0x5B, 0xD6, 0x2C, 0x3B, 0x20, 0xFF, 0xCE, 0x7F, 0x3B, 0xCE, 0x2C, 
									    0x8A, 0xFB, 0xFF, 0xD0, 0x7F, 0x5A, 0xB9, 0xD6, 0x57, 0x56, 0x48, 0x2C, 0x7E, 0xE9, 0xB2, 0x14, 
									    0x7D, 0x36, 0x4B, 0x78, 0x7F, 0x19, 0xD2, 0x14, 0x8B, 0x9B, 0xFF, 0xD1, 0x8B, 0x5B, 0xFF, 0xD2, 
									    0x53, 0x5C, 0x44, 0x2E, 0x3B, 0x9C, 0x00, 0x0F, 0x57, 0x9C, 0xE1, 0x3E, 0x7F, 0x18, 0xE2, 0x14, 
									    0x57, 0x27, 0x48, 0x1E, 0x53, 0x27, 0x4C, 0x2C, 0x48, 0x00, 0x00, 0x95, 0x39, 0x29, 0x02, 0x00, 
									    0x3B, 0x39, 0x00, 0x01, 0x7C, 0x19, 0xC0, 0x00, 0x40, 0x82, 0xFF, 0xE8, 0x83, 0x17, 0x00, 0x00, 
									    0x3F, 0x18, 0xAC, 0xBC, 0x37, 0x18, 0xB3, 0xB1, 0x40, 0x82, 0x00, 0x14, 0x83, 0x17, 0x00, 0x04, 
									    0x3F, 0x18, 0xBE, 0xBC, 0x37, 0x18, 0xBA, 0xAE, 0x41, 0x82, 0x00, 0x0C, 0x3A, 0xF7, 0x00, 0x20, 
									    0x4B, 0xFF, 0xFF, 0xDC, 0x3B, 0x00, 0x00, 0x1A, 0x7F, 0x17, 0xC6, 0x2C, 0x8B, 0x5B, 0xFF, 0xCD, 
									    0x3D, 0x20, 0x81, 0x70, 0x3A, 0xF8, 0xFF, 0xFE, 0x7E, 0xF7, 0xD1, 0xD6, 0x7E, 0xF7, 0xCA, 0x14, 
									    0x7E, 0xB7, 0xD2, 0x14, 0x56, 0xE7, 0x48, 0x1E, 0x52, 0xE7, 0x4C, 0x2C, 0x48, 0x00, 0x00, 0x31, 
									    0x39, 0x29, 0x02, 0x00, 0x3A, 0xF7, 0x00, 0x01, 0x7C, 0x17, 0xA8, 0x00, 0x40, 0x82, 0xFF, 0xE8, 
									    0x57, 0x18, 0x08, 0x3C, 0x7F, 0x16, 0xC6, 0x2C, 0x28, 0x18, 0xFF, 0xF7, 0x41, 0x80, 0xFF, 0xC8, 
									    0x3D, 0x20, 0x81, 0x70, 0x7D, 0x28, 0x03, 0xA6, 0x4E, 0x80, 0x00, 0x21, 0x7D, 0x48, 0x02, 0xA6, 
									    0x7E, 0x29, 0x03, 0xA6, 0x90, 0xA3, 0x68, 0x00, 0x4B, 0x00, 0x18, 0x7D, 0x3F, 0xA0, 0x51, 0x00, 
									    0x4B, 0x00, 0x18, 0x75, 0x3B, 0x80, 0x00, 0x00, 0x7C, 0xFD, 0xE0, 0x30, 0x4B, 0x00, 0x18, 0x69, 
									    0x3B, 0x9C, 0x00, 0x08, 0x2C, 0x1C, 0x00, 0x20, 0x40, 0x82, 0xFF, 0xF0, 0x3F, 0xA0, 0x95, 0x00, 
									    0x4B, 0x00, 0x18, 0x55, 0x3F, 0xA0, 0xFF, 0x00, 0x4B, 0x00, 0x18, 0x4D, 0x4B, 0x00, 0x18, 0x49, 
									    0x3C, 0x9F, 0x02, 0x00, 0x74, 0x84, 0xFF, 0x00, 0x40, 0x82, 0xFF, 0xF4, 0x4B, 0x00, 0x18, 0x39, 
									    0x57, 0xFF, 0x46, 0x3E, 0x7F, 0xE9, 0x21, 0xAE, 0x7C, 0x09, 0x20, 0xAC, 0x38, 0x84, 0x00, 0x01, 
									    0x42, 0x00, 0xFF, 0xEC, 0x90, 0x63, 0x68, 0x00, 0x7D, 0x48, 0x03, 0xA6, 0x4E, 0x80, 0x00, 0x20};

// This is the level 2 boot-code for MBR based file systems.
const int sd_lev2_mbr_size = 380;
const unsigned char sd_lev2_mbr[380] = {0x90, 0x63, 0x68, 0x00, 0x3A, 0x20, 0x02, 0x02, 0x3B, 0x00, 0x01, 0x86, 0x7F, 0x09, 0xC4, 0x2C, 
									    0x57, 0x07, 0x48, 0x1E, 0x53, 0x07, 0x4C, 0x2C, 0x3D, 0x20, 0x81, 0x01, 0x48, 0x00, 0x00, 0xED, 
									    0x3B, 0x69, 0x00, 0x40, 0x3D, 0x20, 0x80, 0x10, 0x3B, 0x40, 0xFF, 0xD6, 0x7F, 0x5B, 0xD6, 0x2C, 
										0x3B, 0x20, 0xFF, 0xCE, 0x7F, 0x3B, 0xCE, 0x2C, 0x7F, 0x39, 0xC2, 0x14, 0x8A, 0xFB, 0xFF, 0xD0, 
									    0x7F, 0x5A, 0xB9, 0xD6, 0x57, 0x56, 0x48, 0x2C, 0x7E, 0xE9, 0xB2, 0x14, 0x7D, 0x36, 0x4B, 0x78, 
									    0x7F, 0x19, 0xD2, 0x14, 0x8B, 0x9B, 0xFF, 0xD1, 0x8B, 0x5B, 0xFF, 0xD2, 0x53, 0x5C, 0x44, 0x2E, 
									    0x3B, 0x9C, 0x00, 0x0F, 0x57, 0x9C, 0xE1, 0x3E, 0x7F, 0x18, 0xE2, 0x14, 0x57, 0x27, 0x48, 0x1E, 
									    0x53, 0x27, 0x4C, 0x2C, 0x48, 0x00, 0x00, 0x95, 0x39, 0x29, 0x02, 0x00, 0x3B, 0x39, 0x00, 0x01, 
									    0x7C, 0x19, 0xC0, 0x00, 0x40, 0x82, 0xFF, 0xE8, 0x83, 0x17, 0x00, 0x00, 0x3F, 0x18, 0xAC, 0xBC, 
									    0x37, 0x18, 0xB3, 0xB1, 0x40, 0x82, 0x00, 0x14, 0x83, 0x17, 0x00, 0x04, 0x3F, 0x18, 0xBE, 0xBC, 
									    0x37, 0x18, 0xBA, 0xAE, 0x41, 0x82, 0x00, 0x0C, 0x3A, 0xF7, 0x00, 0x20, 0x4B, 0xFF, 0xFF, 0xDC, 
									    0x3B, 0x00, 0x00, 0x1A, 0x7F, 0x17, 0xC6, 0x2C, 0x8B, 0x5B, 0xFF, 0xCD, 0x3D, 0x20, 0x81, 0x70, 
									    0x3A, 0xF8, 0xFF, 0xFE, 0x7E, 0xF7, 0xD1, 0xD6, 0x7E, 0xF7, 0xCA, 0x14, 0x7E, 0xB7, 0xD2, 0x14, 
									    0x56, 0xE7, 0x48, 0x1E, 0x52, 0xE7, 0x4C, 0x2C, 0x48, 0x00, 0x00, 0x31, 0x39, 0x29, 0x02, 0x00, 
									    0x3A, 0xF7, 0x00, 0x01, 0x7C, 0x17, 0xA8, 0x00, 0x40, 0x82, 0xFF, 0xE8, 0x57, 0x18, 0x08, 0x3C, 
									    0x7F, 0x16, 0xC6, 0x2C, 0x28, 0x18, 0xFF, 0xF7, 0x41, 0x80, 0xFF, 0xC8, 0x3D, 0x20, 0x81, 0x70, 
									    0x7D, 0x28, 0x03, 0xA6, 0x4E, 0x80, 0x00, 0x21, 0x7D, 0x48, 0x02, 0xA6, 0x7E, 0x29, 0x03, 0xA6, 
									    0x90, 0xA3, 0x68, 0x00, 0x4B, 0x00, 0x18, 0x61, 0x3F, 0xA0, 0x51, 0x00, 0x4B, 0x00, 0x18, 0x59, 
									    0x3B, 0x80, 0x00, 0x00, 0x7C, 0xFD, 0xE0, 0x30, 0x4B, 0x00, 0x18, 0x4D, 0x3B, 0x9C, 0x00, 0x08, 
									    0x2C, 0x1C, 0x00, 0x20, 0x40, 0x82, 0xFF, 0xF0, 0x3F, 0xA0, 0x95, 0x00, 0x4B, 0x00, 0x18, 0x39, 
									    0x3F, 0xA0, 0xFF, 0x00, 0x4B, 0x00, 0x18, 0x31, 0x4B, 0x00, 0x18, 0x2D, 0x3C, 0x9F, 0x02, 0x00, 
									    0x74, 0x84, 0xFF, 0x00, 0x40, 0x82, 0xFF, 0xF4, 0x4B, 0x00, 0x18, 0x1D, 0x57, 0xFF, 0x46, 0x3E, 
									    0x7F, 0xE9, 0x21, 0xAE, 0x7C, 0x09, 0x20, 0xAC, 0x38, 0x84, 0x00, 0x01, 0x42, 0x00, 0xFF, 0xEC, 
										0x90, 0x63, 0x68, 0x00, 0x7D, 0x48, 0x03, 0xA6, 0x4E, 0x80, 0x00, 0x20};

int main(int argc, char* argv[])
{
	int ptype;
	int mfile;
	int ret;
	unsigned char mbuf[512], vbuf[512];
	const unsigned char *sd_lev2;
	unsigned long sd_lev2_size;
	
	printf ("SDLoad Card Patcher V1.00 by Costis\n");
	printf ("(%s Version)\n", OS_version);

	// Check if the user has passed a valid parameter...
	if (argc != 2)
		usage ();



	printf ("Reading first sector...");
	mfile = open(argv[1], O_RDONLY);
	ret = read(mfile, &mbuf, 512);
	close(mfile);
	if (ret != 512)
	{
		// Error! Maybe the user has forgotten to insert an SD card?!?!
		printf ("Failed! Make sure a working SD card is inserted!\n");
		exit (0);
	} else
		printf ("OK!\n");

	// Success! We have managed to read out the first sector of the drive.
	// Check byte index 0x15 (drive media type descriptor) and byte index
	// 0x26 (extended signature) to find out whether this sector is a
	// master boot record or a volume boot record...
	if ((mbuf[0x26] == 0x29) && (mbuf[0x15] == 0xF8))
	{
		// This is a volume boot record, NOT a master boot record...
		printf ("Volume Boot Record found in Sector 0.\n");
		// We therefore want to patch in the VBR level 2 code:
		ptype = 1;
		sd_lev2 = sd_lev2_vbr;
		sd_lev2_size = sd_lev2_vbr_size;
	} else {
		// This is a master boot record...
		printf ("Master Boot Record found in Sector 0.\n");
		// We therefore want to patch in the MBR level 2 code:
		ptype = 0;
		sd_lev2 = sd_lev2_mbr;
		sd_lev2_size = sd_lev2_mbr_size;
	}

	printf ("Inserting level 2 boot-code in %cBR...", (ptype ? 'V' : 'M'));

	// Patch in the code in the right offset...
	memcpy (mbuf + 0x40, sd_lev2, sd_lev2_size);

	// Attempt to write the new sector 0 back to the card.
	mfile = open(argv[1], O_WRONLY);
	ret = write(mfile, &mbuf, 512);
	close(mfile);
	if (ret != 512)
	{
		// Write failure! Maybe the user has write protected their card?!?!
		printf ("Write Error! Check the write protect switch on the SD card.\n");
		exit (0);
	} else
		printf ("OK!\n");

	// Apparently our write succeeded! Let's check to make sure...
	printf ("Verifying Master Boot Record...");

	// Read sector 0 again.
	mfile = open(argv[1], O_RDONLY);
	ret = read(mfile, &vbuf, 512);
	close(mfile);
	if (ret != 512)
	{
		printf ("Read Error!\n");
		exit (0);
	} 

	// Compare the new read back sector 0, with the one that was suppsoed to
	// have been written.
	if (memcmp (mbuf, vbuf, 512) != 0)
	{
		// Oops! They are not the same...
		printf ("Verification Failed!\n");
		exit (0);
	} else
		printf ("OK!\n");

	// Finally! The SD card patching process has been completed! Tell the
	// user the good news and instruct them to continue the installation
	// process...
	printf ("\nThe SD card has been patched successfully. The file\n");
	printf ("system on it should remain completely functional as\n");
	printf ("before, and you may now proceed to the next step of\n");
	printf ("the Action Replay SD card bootloader installation\n");
	printf ("process.\n\n");

	// End of program.
	return 0;
}

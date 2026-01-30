#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// MBR Partition
typedef struct {
	uint8_t boot_indicator;
	uint8_t starting_chs[3];
	uint8_t os_type;
	uint8_t ending_chs[3];
	uint32_t starting_lba;
	uint32_t size_lba;
} __attribute__ ((packed)) Mbr_Partition;

// Master Boot Record
typedef struct {
	uint8_t boot_code[440];
	uint32_t mbr_signature;
	uint16_t unknown;
	Mbr_Partition partition[4];
	uint16_t boot_signature;
} __attribute__ ((packed)) Mbr; 
	
char *image_name = "test.img";
const uint64_t LBA_SIZE = 512;
const uint64_t ESP_SIZE = 1024*1024*33;  // 33MB
const uint64_t DATA_SIZE = 1024*1024; 
uint64_t image_size = 0;
uint64_t esp_lbas, data_lbas, image_size_lbas;

// Convert bytes to LBAs
uint64_t bytes_to_lbas(const uint64_t bytes) {
	return bytes / (LBA_SIZE) + (bytes % LBA_SIZE > 0 ? 1 : 0); // Add 1 if any left over
}

// Write protective MBR
bool write_mbr(FILE *image) {
	// Size of LBA is disk size - 1; make sure to clamp
	if (image_size_lbas > 0xFFFFFFFF) image_size_lbas = 0x100000000;
	Mbr mbr = {
		.boot_code = { 0 },
		.mbr_signature = 0,
		.unknown = 0,
		.partition[0] = {
			.boot_indicator = 0,
			// Cylinder = 1024, Head = 255, Sector = 63
			.starting_chs = {0x00, 0x02, 0x00},
			.os_type = 0xEE,   // GPT ID
			.ending_chs = {0xFF, 0xFF, 0xFF},
			.starting_lba = 0x00000001,
			// Don't count MBR sector
			.size_lba = image_size_lbas - 1,
		},
		.boot_signature = 0xAA55,
		
	};
	
	//TODO
	if (fwrite(&mbr, 1, sizeof mbr, image) != sizeof mbr) {}
	return true;
}

int main(void) {
	FILE *image = fopen(image_name, "wb+");
	if (!image) {
		fprintf(stderr, "Error: could not open file %s\n", image_name);
		return EXIT_FAILURE;
	}
	
	image_size = ESP_SIZE + DATA_SIZE + (1024*1024); // Extra padding for GPT/MBR
	image_size_lbas = bytes_to_lbas(image_size);
	
	if (!write_mbr(image)) {
		fprintf(stderr, "Error: could not open protective MBR for file %s\n", image_name);
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}


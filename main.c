#include <stdio.h>
#include <stdint.h>

#define PARTITION_TABLE_OFFSET 446
#define PARTITION_ENTRY_SIZE 16

#define GiB 1073741824
#define MiB 1048576
#define KiB 1024

typedef struct {
    uint8_t status;
    uint8_t chs_start[3];
    uint8_t type;
    uint8_t chs_end[3];
    uint32_t lba;
    uint32_t sectors;
}
__attribute__((packed)) // pack struct members without padding
P_ENTRY;

int main (int argc, char **argv) {
    if (argc != 2) {
        printf("Error: 1 param expected. %d given.", argc - 1);
        return 1;
    }

    unsigned char mbr[512];

    FILE *fptr = fopen(argv[1], "rb");
    if (fptr == NULL) return 2;

    fread(mbr, 1, 512, fptr);
    fclose(fptr);

    if (0x55 != mbr[510] || 0xAA != mbr[511]) {
        printf("Error: Invalid signature\n");
        return 3;
    }

    // for (size_t i = 0; i < 512; i++) {
    //     printf("%02X ", mbr[i]);
    // }
    // printf("\n");

    P_ENTRY *partitions = (P_ENTRY*)&mbr[PARTITION_TABLE_OFFSET];

    for (int i = 0; i < 4; i++) {   // 4 is the partition limit of MBR
        if (!partitions[i].sectors) continue;

        printf("Partition %d\n", i + 1);
        printf("Start: %u\n", partitions[i].lba);
        printf("End: %u\n", partitions[i].lba + partitions[i].sectors);
        printf("Sectors: %u\n", partitions[i].sectors);

        // the sector size is expected to be 512 bytes
        double total = partitions[i].sectors * 512;
        if (total >= GiB) {
            printf("Size: %.2lf GiB\n", total / GiB);
        } else if (total >= MiB) {
            printf("Size: %.2lf MiB\n", total / MiB);
        } else if (total >= KiB) {
            printf("Size: %.2lf KiB\n", total / KiB);
        } else {
            printf("Size: %.2lf Bytes\n", total);
        }
        printf("Type: %02x\n", partitions[i].type);
    }
    return 0;
}
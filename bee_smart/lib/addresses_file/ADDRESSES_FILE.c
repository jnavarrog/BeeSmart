#include "cfs/cfs.h"
#include "cfs/cfs-coffee.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "ADDRESSES_FILE.h"

#define FILENAME "addresses_file"
#define MESSAGE_SIZE 16

int ds18b20_amount_int;
static uint8_t sequence;

void start_addresses_file(int ds18b20_amount) {
  sequence = 0;
  ds18b20_amount_int = ds18b20_amount;
  cfs_coffee_format();
  cfs_remove(FILENAME);
}

uint8_t write_address(Ds18b20_Address address) {
  int fd;
  int r;
  struct record {
    char message[MESSAGE_SIZE + 1];
    uint8_t sequence;
  } record;
  char address_string[MESSAGE_SIZE];

  if (sequence >= ds18b20_amount_int) {
    return -1;
  }

  sprintf(address_string, "%llx", address);

  strncpy(record.message, address_string, sizeof(record.message) - 1);
  record.message[sizeof(record.message) - 1] = '\0';
  record.sequence = sequence;
  sequence++;

  fd = cfs_open(FILENAME, CFS_WRITE | CFS_APPEND | CFS_READ);

  if(fd < 0) {
    printf("failed to open %s\n", FILENAME);
    return 0;
  }

  r = cfs_write(fd, &record, sizeof(record));

  if(r != sizeof(record)) {
    printf("failed to write %d bytes to %s\n", (int)sizeof(record), FILENAME);
    cfs_close(fd);
    return 0;
  }

  printf("Wrote address \"%s\", index %u\n", record.message, record.sequence);

  cfs_close(fd);

  return 1;
}

Ds18b20_Address read_address(uint8_t index) {
  int fd;
  int r;
  struct record {
    char message[MESSAGE_SIZE + 1];
    uint8_t sequence;
  } record;
  Ds18b20_Address address;

  if(index >= ds18b20_amount_int) {
    return -1;
  }

  fd = cfs_open(FILENAME, CFS_WRITE | CFS_APPEND | CFS_READ);

  if(fd < 0) {
    printf("failed to open %s\n", FILENAME);
    return 0;
  }

  if(cfs_seek(fd, sizeof(record) * index, CFS_SEEK_SET) != sizeof(record) * index) {
    printf("seek failed\n");
    cfs_close(fd);
    return 0;
  }

  r = cfs_read(fd, &record, sizeof(record));

  if(r == 0) {
    printf("failed to read %d bytes from %s, index %d, got %d\n", (int)sizeof(record), FILENAME, index, r);
    cfs_close(fd);
    return 0;
  } else if(r < sizeof(record)) {
    printf("failed to read %d bytes from %s, got %d\n", (int)sizeof(record), FILENAME, r);
    cfs_close(fd);
    return 0;
  }

  address = strtoll(record.message, NULL, 16);
  printf("Read address %llx, index %u\n", address, record.sequence);

  cfs_close(fd);

  return address;
}

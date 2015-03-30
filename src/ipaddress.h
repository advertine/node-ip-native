#ifndef HAVE_IPADDRESS_H
#define HAVE_IPADDRESS_H

#include <stdlib.h>
#include <stdint.h>
#include "inet_pton.h"
#include "inet_ntop.h"

typedef struct ipv6data {
  union {
    struct {
      uint32_t pad[3];
      union {
        uint32_t ui32;
        uint32_t ui8[4];
      } addr;
    } ipv4;
    uint32_t ui32[4];
    uint8_t ui8[16];
  };
} ipv6data_t;

#define IPv6_DECIMAL_SIZE 40

#ifndef INET6_ADDRSTRLEN
#  define INET6_ADDRSTRLEN \
          sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")
#endif

#ifndef INET_ADDRSTRLEN
#  define INET_ADDRSTRLEN \
          sizeof("255.255.255.255")
#endif

int IpAddressStr2No(const char *ipstr, ipv6data_t *ip);
char *IPAddressToStr(const void* ipdata, const size_t ipsize, char out[INET6_ADDRSTRLEN]);
int IpAddressV6NumStr(const ipv6data_t *ip, char out[IPv6_DECIMAL_SIZE]);

#endif /* HAVE_IPADDRESS_H */

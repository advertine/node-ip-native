#include "ipaddress.h"

#include <stdio.h>

#ifndef WIN32
#  include <sys/socket.h>
#  include <netinet/in.h>
#else
#ifdef WIN32
#  include <Winsock2.h>
#endif
#endif

int IpAddressStr2No(const char *ipstr, ipv6data_t *ip)
{
  if ( Curl_inet_pton(AF_INET, ipstr, (void *)(ip->ipv4.addr.ui8) ) == 1 ) {

    return 4;

  } else if ( Curl_inet_pton(AF_INET6, ipstr, (void *)(ip->ui8) ) == 1) {

    /* IPv4Map */
    if ( ntohl(ip->ui32[2]) == 0x0000FFFFU &&
         ! (ip->ui32[0] | ip->ui32[1]) )
      return 4;

    return 6;

  }

  return 0;
}

char *IPAddressToStr(const void* ipdata, const size_t ipsize, char out[INET6_ADDRSTRLEN])
{
  switch(ipsize) {
    case 4:
      return Curl_inet_ntop(AF_INET, ipdata, out, INET_ADDRSTRLEN);
    case 16:
      return Curl_inet_ntop(AF_INET6, ipdata, out, INET6_ADDRSTRLEN);
    default:
      return NULL;
  }
}

int IpAddressV6NumStr(const ipv6data_t *ip, char out[IPv6_DECIMAL_SIZE])
{
  char buf[IPv6_DECIMAL_SIZE];
  int n = 0, head = sizeof(buf) - 1;
  char *p = out;

  buf[head] = 0;

  do {
    uint32_t segment = ntohl(ip->ui32[n++]), mask = 0x80000000;

    for ( ; mask != 0 ; mask >>= 1 ) {
      int i, carry;

      carry = ( (segment & mask) != 0 );

      for ( i = sizeof(buf) - 1; i >= head; --i ) {
        carry = ( (buf[i] += buf[i] + carry) > 9 );

        if ( carry )
          buf[i] -= 10;
      }

      if ( carry )
        buf[--head] = 1;
    }
  } while ( n < 4 );

  n = sizeof(buf) - head;

  while ( head < (int)sizeof(buf) ) {
    *p++ = buf[head++] + '0';
  }
  *p = 0;

  return n;
}

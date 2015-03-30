#include "nodeip.h"
#include <string.h>

namespace NodeIp {
  using v8::Handle;
  using v8::Local;
  using v8::Object;
  using v8::Int32;
  using v8::Uint32;

  /**
   * Ip address to binary.
   * 
   * toBuffer(ip[, buffer[, offset]])
   * 
   * @param {string} ip - ip address in string form
   * @param {Buffer} buffer - optional output buffer
   * @param {Int32} offset - optional output buffer offset
   * 
   * @return {number|Buffer|null}
   *
   * In case output buffer was provided this function returns ip address type
   * as number (4 or 6).
   *
   * If only ip address is given the new Buffer instance is returned.
   *
   * Returns null if ip address was invalid.
   *
   * Throws error on invalid arguments.
   *
  **/
  NAN_METHOD(ToBuffer)
  {
    NanScope();

    if ( args.Length() < 1 )
      return NanThrowError("ip.toBuffer: missing IP address");

    ssize_t ipsize;
    const void *ipdata;

    ipv6data_t ip;

    int iptype = IpAddressStr2No(*NanAsciiString(args[0]), &ip);

    switch(iptype) {
      case 4:
        ipsize = 4;
        ipdata = (void *) ip.ipv4.addr.ui8;
        break;

      case 6:
        ipsize = 16;
        ipdata = (void *) ip.ui8;
        break;

      default:
        NanReturnNull();
    }

    if ( args.Length() > 1 ) {
      if ( ! node::Buffer::HasInstance(args[1]) ) {
        return NanThrowError("ip.toBuffer: second argument should be a buffer");
      }

      ssize_t offset = 0, ipoffset = 0;

      if ( args.Length() > 2 ) {
        offset = args[2]->Int32Value();
      }

      Local<Object> buffer = args[1].As<Object>();
      ssize_t bufsize = node::Buffer::Length(buffer);

      /* offset negative is from the end of buffer */
      if ( offset < 0 )
        offset += bufsize;

      /* check boundaries */
      if ( offset < 0 ) {
        ipoffset = -offset;
        ipsize -= ipoffset;
        offset = 0;
      }

      if ( ipsize + offset > bufsize ) {
        ipsize = bufsize - offset;
      }

      if ( ipsize > 0 ) {

        memcpy( (void *)(node::Buffer::Data(buffer) + offset),
                (void *)((char *)ipdata + ipoffset),
                ipsize );
      }

      NanReturnValue( NanNew<Int32>(iptype) );

    } else {

      Local<Object> buffer = NanNewBufferHandle((uint32_t) ipsize);

      memcpy( (void *)node::Buffer::Data(buffer), ipdata, ipsize );

      NanReturnValue(buffer);
    }

  }

  /**
   * Ip address to number.
   * 
   * toNumber(ip)
   * 
   * @param {string} ip - ip address in string form
   * 
   * @return {number|String}
   *
   * If ip address is IPv4 a number is returned.
   * If ip address is IPv6 a decimal number as string is returned.
   *
   * Returns null if ip address was invalid.
   *
   * Throws error on invalid argument.
   *
  **/
  NAN_METHOD(ToNumber)
  {
    NanScope();

    if ( args.Length() < 1 )
      return NanThrowError("ip.toNumber: missing IP address");

    ipv6data_t ip;

    int iptype = IpAddressStr2No(*NanAsciiString(args[0]), &ip);

    switch(iptype) {
      case 4:
        {
          uint32_t ipno = ntohl(ip.ipv4.addr.ui32);
          NanReturnValue( NanNew<Uint32>(ipno) );
        }
      case 6:
        {
          char out[IPv6_DECIMAL_SIZE];
          NanReturnValue( NanNew(out, IpAddressV6NumStr(&ip, out)) );
        }

      default:
        NanReturnNull();
    }
  }

  /**
   * Ip address to number.
   * 
   * toNumber(ip)
   * 
   * @param {string} ip - ip address in string form
   * 
   * @return {number|String}
   *
   * If ip address is IPv4 a number is returned.
   * If ip address is IPv6 a decimal number as string is returned.
   *
   * Returns null if ip address was invalid.
   *
   * Throws error on invalid argument.
   *
  **/
  NAN_METHOD(ToString)
  {
    NanScope();

    if ( args.Length() < 1 || ! node::Buffer::HasInstance(args[0]) )
      return NanThrowError("ip.toString: missing buffer argument");

    if ( ! node::Buffer::HasInstance(args[0]) )
      return NanThrowTypeError("ip.toString: first argument needs to be a buffer");

    Local<Object> buffer = args[0].As<Object>();
    ssize_t offset = 0, bufsize = node::Buffer::Length(buffer), ipsize = bufsize;

    if ( args.Length() > 1 ) {
      offset = args[1]->Int32Value();
      if ( args.Length() > 2 ) {
        ipsize = args[2]->Int32Value();
      } else {
        ipsize = bufsize - offset;
      }

      if ( offset < 0 )
        offset += bufsize;

      if ( offset + ipsize > bufsize )
        ipsize = bufsize - offset;
    }

    if ( offset >= 0 && ipsize > 0 ) {
      char out[INET6_ADDRSTRLEN];
      char *str = IPAddressToStr((void *)(node::Buffer::Data(buffer) + offset), ipsize, out);
      if ( str != NULL )
        NanReturnValue( NanNew(str) );
    }

    NanReturnNull();
  }

  void Init(Handle<Object> exports)
  {
    NODE_SET_METHOD(exports, "toBuffer",     ToBuffer);
    NODE_SET_METHOD(exports, "toString",     ToString);
    NODE_SET_METHOD(exports, "toNumber",     ToNumber);
  }

}

NODE_MODULE(nodeip, NodeIp::Init)

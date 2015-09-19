#include "nodeip.h"
#include <string.h>

namespace NodeIp {
  using v8::Handle;
  using v8::Local;
  using v8::Value;
  using v8::Object;
  using v8::String;

  bool IpCharsFromValue(Local<Value> value, char ipchars[INET6_ADDRSTRLEN])
  {
    Local<String> ipstring = value->ToString();

    if (ipstring.IsEmpty())
      return false;

    const int ipstrlen = ipstring->Length();

    if (!ipstrlen)
      return false;

    int nchars;
    const int flags = String::NO_NULL_TERMINATION | Nan::imp::kReplaceInvalidUtf8;
    int blen = ipstring->WriteUtf8(ipchars, static_cast<int>(INET6_ADDRSTRLEN - 1), &nchars, flags);
    /* utf8 characters or string cut */
    if (blen != nchars || nchars != ipstrlen)
      return false;

    ipchars[blen] = '\0';
    return true;
  }

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

    if ( info.Length() < 1 )
      return Nan::ThrowError("ip.toBuffer: missing IP address");

    ssize_t ipsize;
    const void *ipdata;

    char ipchars[INET6_ADDRSTRLEN];

    if (!IpCharsFromValue(info[0], ipchars))
      return info.GetReturnValue().SetNull();

    ipv6data_t ip;

    int iptype = IpAddressStr2No(ipchars, &ip);

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
        return info.GetReturnValue().SetNull();
    }

    if ( info.Length() > 1 ) {
      if ( ! node::Buffer::HasInstance(info[1]) ) {
        return Nan::ThrowError("ip.toBuffer: second argument should be a buffer");
      }

      ssize_t offset = 0, ipoffset = 0;

      if ( info.Length() > 2 ) {
        offset = info[2]->Int32Value();
      }

      Local<Object> buffer = info[1].As<Object>();
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

      info.GetReturnValue().Set( (int32_t) iptype );

    } else {

      Nan::MaybeLocal<Object> buffer = Nan::CopyBuffer(
                                      (const char *)ipdata, (uint32_t) ipsize);
      if (buffer.IsEmpty()) {
        info.GetReturnValue().SetNull();
      } else
        info.GetReturnValue().Set(buffer.ToLocalChecked());
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

    if ( info.Length() < 1 )
      return Nan::ThrowError("ip.toNumber: missing IP address");

    char ipchars[INET6_ADDRSTRLEN];

    if (!IpCharsFromValue(info[0], ipchars))
      return info.GetReturnValue().SetNull();

    ipv6data_t ip;

    int iptype = IpAddressStr2No(ipchars, &ip);

    switch(iptype) {
      case 4:
        {
          uint32_t ipno = ntohl(ip.ipv4.addr.ui32);
          info.GetReturnValue().Set( Nan::New( ipno ) ); // a bug in 0.10 converts to signed int if not wrapped with New
          break;
        }
      case 6:
        {
          char ipchars[IPv6_DECIMAL_SIZE];
          int ipstrlen = IpAddressV6NumStr(&ip, ipchars);
          Nan::MaybeLocal<String> ipstring = Nan::NewOneByteString((uint8_t *)ipchars, ipstrlen);
          if (ipstring.IsEmpty()) {
            info.GetReturnValue().SetNull();
          } else
            info.GetReturnValue().Set( ipstring.ToLocalChecked() );
          break;
        }

      default:
        info.GetReturnValue().SetNull();
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

    if ( info.Length() < 1 || ! node::Buffer::HasInstance(info[0]) )
      return Nan::ThrowError("ip.toString: missing buffer argument");

    if ( ! node::Buffer::HasInstance(info[0]) )
      return Nan::ThrowTypeError("ip.toString: first argument needs to be a buffer");

    Local<Object> buffer = info[0].As<Object>();
    ssize_t offset = 0, bufsize = node::Buffer::Length(buffer), ipsize = bufsize;

    if ( info.Length() > 1 ) {
      offset = info[1]->Int32Value();
      if ( info.Length() > 2 ) {
        ipsize = info[2]->Int32Value();
      } else {
        ipsize = bufsize - offset;
      }

      if ( offset < 0 )
        offset += bufsize;

      if ( offset + ipsize > bufsize )
        ipsize = bufsize - offset;
    }

    if ( offset >= 0 && ipsize > 0 ) {
      char ipchars[INET6_ADDRSTRLEN];
      char *str = IPAddressToStr((void *)(node::Buffer::Data(buffer) + offset), ipsize, ipchars);
      if ( str != NULL ) {
        Nan::MaybeLocal<String> ipstring = Nan::NewOneByteString((uint8_t *)str);
        if (!ipstring.IsEmpty()) {
          return info.GetReturnValue().Set( ipstring.ToLocalChecked() );
        }
      }
    }

    info.GetReturnValue().SetNull();
  }

  NAN_MODULE_INIT(Init)
  {
    Nan::SetMethod(target, "toBuffer", ToBuffer);
    Nan::SetMethod(target, "toString", ToString);
    Nan::SetMethod(target, "toNumber", ToNumber);
  }

}

NODE_MODULE(nodeip, NodeIp::Init)

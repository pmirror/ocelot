#include <sstream>
#include <cctype>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "response.h"
#include "misc_functions.h"

std::string html(const std::string &body) {
  return response(body, false, true);
}

std::string response(const std::string &body, bool gzip, bool html) {
  const std::string head = response_head(gzip, html);
  std::string out;
  bool processed = false;
  if (html) {
    out = "<html><head><meta name=\"robots\" content=\"noindex, nofollow\" /></head><body>" + body + "</body></html>";
    processed = true;
  }
  if (gzip) {
    std::stringstream ss, zss;
    ss << body;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::gzip_compressor());
    in.push(ss);
    boost::iostreams::copy(in, zss);
    out = zss.str();
    processed = true;
  }
  if (processed) {
    return head + out;
  }
  return head + body;
}

std::string response_head(bool gzip, bool html) {
  const std::string content_type = html ? "text/html" : "text/plain";
  std::string head = "HTTP/1.1 200 OK\r\nServer: Ocelot 1.0";
  head += "\r\nContent-Type: " + content_type;
  if (gzip) {
    head += "\r\nContent-Encoding: gzip";
  }
  head += "\r\nConnection: close\r\n\r\n";
  return head;
}

std::string error(const std::string &err) {
  return response("d14:failure reason" + std::to_string( err.length() ) + ':' + err + "12:min intervali5400e8:intervali5400ee\r\n\r\n", false, false);
}

std::string warning(const std::string &msg) {
  return "15:warning message" + std::to_string( msg.length() ) + ':' + msg + "\r\n\r\n";
}

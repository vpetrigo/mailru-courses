#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#ifdef _WIN32
  #include <winsock2.h>
#else
  #include <sys/socket.h>
#endif
#include "uv.h"
#include "http_parser.h"

#define CHECK(x, msg) \
  if (x != 0) { \
    std::cerr << "error: " << msg << " status: " << uv_strerror(x) << std::endl; \
    exit(1); \
  }

#define LOG(msg) \
  std::cout << msg << std::endl;

class HTTP_request_parser {
public:
  HTTP_request_parser(uv_tcp_t *stream) {
    http_parser_init(&parser, HTTP_REQUEST);
    parser_settings.on_url = on_url;
    parser_settings.on_body = on_body;
    parser_settings.on_message_begin = on_message_begin;
    parser_settings.on_header_field = on_header_field;
    parser_settings.on_header_value = on_header_value;
    parser_settings.on_headers_complete = on_headers_complete;
    parser.data = stream;
  }

  http_parser *get_parser() noexcept {
    return &parser;
  }

  http_parser_settings *get_parser_settings() noexcept {
    return &parser_settings;
  }

private:
  http_parser parser;
  http_parser_settings parser_settings;

  static int on_message_begin(http_parser *hp) {
    std::cout << "msg begin" << std::endl;
    return 0;
  }

  static int on_url(http_parser *hp, const char *at, size_t length) {
    std::cout << "url: " << std::string{at, length} << std::endl;

    return 0;
  }

  static int on_header_field(http_parser *hp, const char *at, size_t length) {
    std::cout << "Header field: " << std::string{at, length} << std::endl;

    return 0;
  }

  static int on_header_value(http_parser *hp, const char *at, size_t length) {
    std::cout << "Header value: " << std::string{at, length} << std::endl;

    return 0;
  }

  static int on_headers_complete(http_parser *hp) {
    std::cout << "headers complete" << std::endl;
    return 0;
  }

  static int on_body(http_parser*, const char *at, size_t length) {
    std::cout << "body" << std::endl;
    return 0;
  }

  static int on_message_complete(http_parser *hp) {
    std::cout << "msg parsed" << std::endl;
    return 0;
  }
};

class HTTP_client {
public:
  HTTP_client() : rp{&handle} {}

  HTTP_client(const HTTP_client& c) = delete;
  HTTP_client& operator=(const HTTP_client& c) = delete;

  uv_tcp_t *get_handle() noexcept {
    return &handle;
  }

  void parse(const char *buf, size_t len) {
      http_parser_execute(rp.get_parser(), rp.get_parser_settings(), buf, len);
  }

private:
  uv_tcp_t handle;
  uv_write_t write_req;
  HTTP_request_parser rp;
};

class HTTP_server {
public:
  HTTP_server(uv_loop_t *l, const std::string& ip, int port) : loop{l} {
    // keepalive interval
    constexpr unsigned int delay = 60;
    struct sockaddr_in sa = {0};
    int status = uv_ip4_addr(ip.c_str(), port, &sa);

    CHECK(status, "cannot convert IP4 address");
    status = uv_tcp_init(loop, &server);
    CHECK(status, "cannot init TCP session");
    status = uv_tcp_keepalive(&server, true, delay);
    CHECK(status, "cannot setup keepalive");
    status = uv_tcp_bind(&server, reinterpret_cast<struct sockaddr *> (&sa), 0);
    CHECK(status, "cannot bind server on " + ip + ":" + std::to_string(port));
    status = uv_listen(reinterpret_cast<uv_stream_t *> (&server), SOMAXCONN, on_connect);
    CHECK(status, "listen fail");
    LOG("Listen on " + ip + ":" + std::to_string(port));
  }

  HTTP_server(const HTTP_server& s) = delete;
  HTTP_server& operator=(const HTTP_server& s) = delete;
private:
  uv_tcp_t server;
  uv_loop_t *loop;

  // callbacks
  static void on_connect(uv_stream_t *server, int status) {
    CHECK(status, "bad connection");
    HTTP_client *cptr = new HTTP_client;
    std::cout << "new connection" << std::endl;
    uv_tcp_t *client_handle = cptr->get_handle();

    // store pointer to the whole HTTP_client class for further usage
    client_handle->data = cptr;
    status = uv_tcp_init(server->loop, client_handle);
    CHECK(status, "cannot init client connection");
    status = uv_accept(server, reinterpret_cast<uv_stream_t *> (client_handle));
    CHECK(status, "cannot accept incoming connection");
    uv_read_start(reinterpret_cast<uv_stream_t *> (client_handle), alloc_memory, on_read);
  }

  // handle reading from stream
  static void on_read(uv_stream_t *peer, ssize_t nread, const uv_buf_t *buf) {
    HTTP_client *cptr = static_cast<HTTP_client *> (peer->data);

    if (nread >= 0) {
      // get HTTP request and parse it
      std::cout << buf->base << std::endl;
      cptr->parse(buf->base, nread);
    }
    else {
      uv_shutdown_t *req = new uv_shutdown_t;

      if (nread != UV_EOF) {
        // error occured
      }

      uv_shutdown(req, peer, on_shutdown);
    }
    // freeing buffer
    delete buf->base;
  }

  // handle shutdown request
  static void on_shutdown(uv_shutdown_t *req, int status) {
    CHECK(status, "cannot shutdown connection");
    LOG("shutdowning client connection");
    uv_close(reinterpret_cast<uv_handle_t *> (req->handle), on_close);
    delete req;
  }

  // handle close request
  static void on_close(uv_handle_t *peer) {
    HTTP_client *cptr = static_cast<HTTP_client *> (peer->data);
    LOG("close client connection");
    delete cptr;
  }

  // alloc memory for client's messages buffer
  static void alloc_memory(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    *buf = uv_buf_init(new char[suggested_size], suggested_size);
  }
};

int main() {
  uv_loop_t *loop = uv_default_loop();
  HTTP_server hs{loop, "0.0.0.0", 9999};

  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);

  return 0;
}

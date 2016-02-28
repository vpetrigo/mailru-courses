#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <utility>
#include <unistd.h>
#include <sys/types.h>
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
  std::cout << msg << std::endl

const std::string http_404 {
  R"(HTTP/1.0 404 Not Found
Content-Type: text/html
Contetn-Length: 19

<b>Bad request</b>
)"};

class HTTP_request_parser {
public:
  HTTP_request_parser(uv_tcp_t *stream) {
    http_parser_settings_init(&parser_settings);
    http_parser_init(&parser, HTTP_REQUEST);
    parser_settings.on_url = on_url;
    parser_settings.on_body = on_body;
    parser_settings.on_message_begin = on_message_begin;
    parser_settings.on_message_complete = on_message_complete;
    parser_settings.on_header_field = on_header_field;
    parser_settings.on_header_value = on_header_value;
    parser_settings.on_headers_complete = on_headers_complete;
    parser.data = stream;
  }

  http_parser& get_parser() noexcept {
    return parser;
  }

  http_parser_settings& get_parser_settings() noexcept {
    return parser_settings;
  }

private:
  http_parser parser;
  http_parser_settings parser_settings;

  static int on_message_begin(http_parser *hp) {
    std::cout << "msg begin" << std::endl;
    return 0;
  }

  static int on_url(http_parser *hp, const char *at, size_t length);

  static int on_header_field(http_parser *hp, const char *at, size_t length) {
    //std::cout << "Header field: " << std::string{at, length} << std::endl;

    return 0;
  }

  static int on_header_value(http_parser *hp, const char *at, size_t length) {
    //std::cout << "Header value: " << std::string{at, length} << std::endl;

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

  static int on_message_complete(http_parser *hp);
};

class HTTP_client {
public:
  HTTP_client() : rp{&handle}, path{} {
    handle.data = this;
    write_req.data = &handle;
  }

  HTTP_client(const HTTP_client& c) = delete;
  HTTP_client& operator=(const HTTP_client& c) = delete;

  uv_tcp_t *get_handle() noexcept {
    return &handle;
  }

  void parse(char *buf, ssize_t len) {
    auto parser_bytes = http_parser_execute(&rp.get_parser(), &rp.get_parser_settings(), buf, len);

    if (len != parser_bytes) {
      throw std::runtime_error{"Wrong number of bytes parsed"};
    }
  }

  template <typename Path>
  void set_path(Path&& p) noexcept {
    path = std::forward<Path>(p);
  }

  template <typename Path>
  void set_server_dir(Path&& sd) noexcept {
    server_dir = std::forward<Path>(sd);
  }

  const std::string& get_path() const noexcept {
    return path;
  }

  const std::string& get_server_dir() const noexcept {
    return server_dir;
  }

  uv_write_t& get_write_req() noexcept {
    return write_req;
  }

private:
  uv_tcp_t handle;
  uv_write_t write_req;
  HTTP_request_parser rp;
  std::string path;
  std::string server_dir;
};

int HTTP_request_parser::on_url(http_parser *hp, const char *at, size_t length) {
  struct http_parser_url url_p;
  // clean up new structure
  http_parser_url_init(&url_p);
  int status = http_parser_parse_url(at, length, false, &url_p);
  CHECK(status, "cannot parse url");

  HTTP_client *cptr = reinterpret_cast<HTTP_client *> (reinterpret_cast<uv_tcp_t *> (hp->data)->data);

  LOG("URL: " + std::string (at, length));
  cptr->set_path(std::string{at + url_p.field_data[UF_PATH].off, url_p.field_data[UF_PATH].len});
  std::cout << cptr->get_path() << std::endl;

  return 0;
}

void close_after_wr(uv_handle_t *handle) {
  HTTP_client *cptr = reinterpret_cast<HTTP_client *> (handle->data);

  delete cptr;
}

void shutdown_after_wr(uv_shutdown_t *req, int status) {
  uv_close(reinterpret_cast<uv_handle_t *> (req->handle), close_after_wr);
  delete req;
}

void on_write(uv_write_t* req, int status) {
  std::cout << "Write finished" << std::endl;
  uv_shutdown_t *shutdown_req = new uv_shutdown_t;
  uv_stream_t *peer = reinterpret_cast <uv_stream_t *> (req->data);

  if (!uv_is_closing(reinterpret_cast<uv_handle_t *> (peer))) {
    uv_shutdown(shutdown_req, peer, shutdown_after_wr);
  }
}

void render_resp(uv_work_t *req) {
  HTTP_client *cptr = reinterpret_cast<HTTP_client *> (req->data);
  uv_stream_t *client_stream = reinterpret_cast<uv_stream_t *> (cptr->get_handle());
  uv_buf_t buf;
  std::string *response = nullptr;
  std::string filepath = cptr->get_server_dir() + cptr->get_path();

  std::cout << filepath << std::endl;
  if (cptr->get_path() == "/" || access(filepath.c_str(), R_OK)) {
    buf = uv_buf_init(const_cast<char *> (http_404.c_str()), http_404.size());
  }
  else {
    std::cout << "Exists" << std::endl;
    std::ifstream ifs{filepath, std::ios::ate | std::ios::binary};
    std::string filedata(ifs.tellg(), '\0');
    // rewind at the begining
    ifs.seekg(0);
    filedata.assign(std::istreambuf_iterator<char> {ifs}, std::istreambuf_iterator<char> {});
    std::string http_200 {"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n"};
    http_200.append("Content-Length: " + std::to_string(filedata.size()) + "\r\n");
    http_200.append("Access-Control-Allow-Origin: *\r\n\r\n");
    response = new std::string{http_200 + filedata};
    buf = uv_buf_init(const_cast<char *> (response->c_str()), response->size());
  }
  int status = uv_write(&cptr->get_write_req(), client_stream, &buf, 1, on_write);
  CHECK(status, "write cannot be done");

  if (response) {
    delete response;
  }
}

void after_render(uv_work_t *req, int status) {
  delete req;
}

int HTTP_request_parser::on_message_complete(http_parser *hp) {
  HTTP_client *cptr = reinterpret_cast<HTTP_client *> (reinterpret_cast<uv_tcp_t *> (hp->data)->data);
  uv_work_t *render_req = new uv_work_t;

  render_req->data = cptr;
  int status = uv_queue_work(cptr->get_handle()->loop, render_req, render_resp, after_render);
  CHECK(status, "queue cannot be started");

  return 0;
}

class HTTP_server {
public:
  HTTP_server(uv_loop_t *l, const std::string& ip, int port, const std::string s_d) : loop{l}, server_dir{s_d} {
    // keepalive interval
    constexpr unsigned int delay = 60;
    struct sockaddr_in sa = {0};
    int status = uv_ip4_addr(ip.c_str(), port, &sa);

    server.data = &server_dir;
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
  std::string server_dir;

  // callbacks
  static void on_connect(uv_stream_t *server, int status) {
    CHECK(status, "bad connection");
    HTTP_client *cptr = new HTTP_client;
    std::cout << "new connection" << std::endl;
    uv_tcp_t *client_handle = cptr->get_handle();

    cptr->set_server_dir(*reinterpret_cast<std::string *> (server->data));
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

struct Server_options {
  Server_options() : ip{"0.0.0.0"}, port{8000}, home_dir{"."} {}

  void set_ip(std::string _ip) noexcept {
    ip = _ip;
  }

  void set_port(const int _port) noexcept {
    port = _port;
  }

  void set_home_dir(std::string _home_dir) {
    home_dir = _home_dir;
  }

  std::string ip;
  int port;
  std::string home_dir;
};

// we must handle the following arguments:
// "-h" : host IP address
// "-p" : host port
// "-d" : host home directory
Server_options parse_args(int argc, char *argv[]) {
  constexpr char serv_set[] = "h:p:d:";
  char option = '\0';
  Server_options so;

  while ((option = getopt(argc, argv, serv_set)) != -1) {
    switch (option) {
      case 'h':
        so.set_ip(optarg);
        break;
      case 'p':
        so.set_port(std::stoi(optarg));
        break;
      case 'd':
        so.set_home_dir(optarg);
        break;
      default:
        std::cerr << "Cannot parse args" << std::endl;
        exit(1);
    }
  }

  LOG("Server parameters: " + so.ip + " " + std::to_string(so.port) + " " + so.home_dir);

  return so;
}

int main(int argc, char *argv[]) {
  Server_options params = parse_args(argc, argv);

  pid_t pid = fork();

  if (pid < 0) {
    std::cerr << "Cannot create child process" << std::endl;
    exit(2);
  }
  else if (pid > 0) {
    std::cout << "Succesfully created new child process. Exiting." << std::endl;
    exit(0);
  }
  // setting all files permissions created by process for all
  umask(0);
    
  std::ofstream ofs{"server.log"};

  std::cout.rdbuf(ofs.rdbuf());
  std::cerr.rdbuf(ofs.rdbuf());

  pid_t sid = setsid();
  
  if (sid < 0) {
    std::cerr << "Cannot obtain SID" << std::endl;
    exit (3);
  }
  close(STDIN_FILENO);
  uv_loop_t *loop = uv_default_loop();
  HTTP_server hs{loop, params.ip, params.port, params.home_dir};

  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);

  return 0;
}

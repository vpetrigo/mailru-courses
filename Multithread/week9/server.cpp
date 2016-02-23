#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <csignal>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <uv.h>

constexpr char any_addr[] = "0.0.0.0";
uv_tcp_t *tcp_server = nullptr;

// free memory allocated for peer connection
void on_close(uv_handle_t *peer) {
    delete peer;
}

void after_shutdown(uv_shutdown_t *req, int status) {
    uv_close(reinterpret_cast<uv_handle_t *> (req->handle), on_close);
    delete req;
}

void after_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    if (nread > 0) {
        std::cout << "received " << nread << " bytes" << std::endl;
        std::cout << "Payload: " << buf->base << std::endl;
    }
    else if (nread < 0) {
        std::cout << "Shutdown connection..." << std::endl;
        uv_shutdown_t *sreq = new uv_shutdown_t;
        
        delete buf->base;
        uv_shutdown(sreq, stream, after_shutdown);
        std::cout << "Done!" << std::endl;
    }
    else {
        std::cout << "Nothing to read" << std::endl;
        delete buf->base;
    }
}

void alloc_fun(uv_handle_t *handle, size_t sug_size, uv_buf_t *buf) {
    buf->base = new char[sug_size];
    buf->len = sug_size;
}

void print_incoming_info(const uv_tcp_t *handle) {
    struct sockaddr_storage ss;
    constexpr int nameline = sizeof ss;
    char buf[INET_ADDRSTRLEN];
    
    uv_tcp_getpeername(handle, reinterpret_cast<struct sockaddr *> (&ss), const_cast<int *> (&nameline));

    struct sockaddr_in *sin_ptr = reinterpret_cast<struct sockaddr_in *> (&ss);
    
    std::cout << inet_ntop(AF_INET, &sin_ptr->sin_addr, buf, INET_ADDRSTRLEN) << ":" << sin_ptr->sin_port << std::endl;
}

void accept_conn(uv_work_t *req) {
    // do stuff with incoming peers
}

void after_accept(uv_work_t *req, int status) {
    // clean worker
}

void new_connection_handler(uv_stream_t *server, int status) {
    std::cout << "Incoming connection" << std::endl;
    uv_work_t *req = new uv_work_t;

    uv_queue_work(server->loop, req, accept_conn, after_accept);
    uv_stream_t *inc_stream = reinterpret_cast<uv_stream_t *> (new uv_tcp_t);

    uv_tcp_init(server->loop, reinterpret_cast<uv_tcp_t *> (inc_stream));
    inc_stream->data = server;
    uv_accept(server, inc_stream);
    print_incoming_info(reinterpret_cast<uv_tcp_t *> (inc_stream));
    uv_read_start(inc_stream, alloc_fun, after_read);
}

uv_loop_t *create_loop() {
    uv_loop_t *loop = uv_default_loop();

    return loop; 
}

uv_stream_t *init_tcp(uv_loop_t *loop) {
    uv_stream_t *tcp_handle = reinterpret_cast<uv_stream_t *> (new uv_tcp_t);
    struct sockaddr_in sin;

    std::cout << "TCP init..." << std::endl;
    uv_tcp_init(loop, reinterpret_cast<uv_tcp_t *> (tcp_handle));
    uv_ip4_addr(any_addr, 8888, &sin);
    uv_tcp_bind(reinterpret_cast<uv_tcp_t *> (tcp_handle), reinterpret_cast<const struct sockaddr *> (&sin), 0);
    uv_listen(tcp_handle, SOMAXCONN, new_connection_handler);
    std::cout << "End TCP init" << std::endl;

    return tcp_handle;
}

void on_server_close(uv_handle_t *handle) {
    std::cout << "Server closed" << std::endl;
    uv_signal_stop(static_cast<uv_signal_t *> (handle->data));
    uv_stop(handle->loop);
    delete handle;
}

void termination_handler(uv_signal_t *handler, int sig) {
    std::cout << "Signal " << sig << " received" << std::endl;
    std::cout << "Close server..." << std::endl;

    if (tcp_server) {
        auto tcp_handle = reinterpret_cast<uv_handle_t *> (tcp_server);
        // for closing in the callback
        tcp_handle->data = handler;
        uv_close(tcp_handle, on_server_close);
    }
}

// register SIGINT and SIGTERM handlers for correct server shutdown
void register_signal_handler(uv_loop_t *loop) {
    constexpr std::size_t q_handler = 2;
    uv_signal_t *sig = new uv_signal_t[q_handler];
    
    for (std::size_t i = 0; i < q_handler; ++i) {
        uv_signal_init(loop, &sig[i]);
    }

    uv_signal_start(&sig[0], termination_handler, SIGINT);
    uv_signal_start(&sig[1], termination_handler, SIGTERM);
}

int main() {
    uv_loop_t *loop = create_loop();
 
    register_signal_handler(loop);  
    tcp_server = reinterpret_cast<uv_tcp_t *> (init_tcp(loop));
    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop); 
    
    return 0;
}

#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LIBSFTCP_SERVER_LIB)
#  define LIBSFTCP_SERVER_EXPORT Q_DECL_EXPORT
# else
#  define LIBSFTCP_SERVER_EXPORT Q_DECL_IMPORT
# endif
#else
# define LIBSFTCP_SERVER_EXPORT
#endif

#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <list>
#include <deque>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

class sf_tcp_error_trans;

constexpr int default_thread_count__ = 4;

class LIBSFTCP_SERVER_EXPORT sftcp_server : public QObject
{
	Q_OBJECT

public:
	using QObject::QObject;
	explicit sftcp_server(QObject *parent = nullptr);
	~sftcp_server();
	void init(int thread_count = default_thread_count__);
	void cleanup();
	bool listen(const QHostAddress &host, quint16 port);
	void disconnect_client(QTcpSocket * const p_sock);
	void send_data(QTcpSocket * const p_sock, const QByteArray& data);
	std::list<QTcpSocket*> get_clients() const;
	int get_send_thread_count() const;
	QTcpServer* get_server() const;

signals:
	void new_connection(QTcpSocket * const p_sock);
	void data_comming(QTcpSocket * const p_sock, const QByteArray &data);
	void client_disconnect(QTcpSocket * const p_sock);
	void sock_error(QTcpSocket * const p_sock, QAbstractSocket::SocketError err);
	void server_error(QAbstractSocket::SocketError err);

private:
	QTcpServer *p_server__{ nullptr };
	std::list<QTcpSocket*> sock_list__;
	std::map<QTcpSocket*, sf_tcp_error_trans*> sock_err_list__;
	std::deque <std::pair<QTcpSocket*, QByteArray>> send_task_deque__;
	std::list<std::shared_ptr<std::thread>> send_thread_list__;
	std::atomic<bool> is_finished__;
	std::mutex mu_send__;
	std::condition_variable cv_send__;
	bool is_listenning__{ false };
	bool is_init__{ false };
	std::mutex mu_deque__;
	int send_thread_count__{ 4 };

	void create_thread__();
	void terminate_thread__();
	void close__();
};

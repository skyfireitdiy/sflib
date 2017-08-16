/// @file	sftcp_server.h
///
/// @brief	Declares the sftcp server class.

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

	/// @class	sf_tcp_error_trans
	///
	/// @brief	A sf TCP error transaction.
	///
	/// @author	SkyFire
	/// @date	2017/8/16

	class sf_tcp_error_trans;

	constexpr int default_thread_count__ = 4;

	/// @class	sftcp_server
	///
	/// @brief	A sftcp server.
	///
	/// @author	SkyFire
	/// @date	2017/8/16

	class LIBSFTCP_SERVER_EXPORT sftcp_server : public QObject
	{
		Q_OBJECT

	public:
		using QObject::QObject;

		/// @fn	explicit sftcp_server::sftcp_server(QObject *parent = nullptr);
		///
		/// @brief	Constructor.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param [in,out]	parent	(Optional) If non-null, the parent.

		explicit sftcp_server(QObject *parent = nullptr);
		~sftcp_server();

		/// @fn	void sftcp_server::init(int thread_count = default_thread_count__);
		///
		/// @brief	Inits the given thread count.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param	thread_count	(Optional) Number of threads.

		void init(int thread_count = default_thread_count__);

		/// @fn	void sftcp_server::cleanup();
		///
		/// @brief	Cleanups this object.
		///
		/// @author	SkyFire
		/// @date	2017/8/16

		void cleanup();

		/// @fn	bool sftcp_server::listen(const QHostAddress &host, quint16 port);
		///
		/// @brief	Listens.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param	host	The host.
		/// @param	port	The port.
		///
		/// @return	True if it succeeds, false if it fails.

		bool listen(const QHostAddress &host, quint16 port);

		/// @fn	void sftcp_server::disconnect_client(QTcpSocket * const p_sock);
		///
		/// @brief	Disconnects the client described by p_sock.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param [in,out]	p_sock	If non-null, the sock.

		void disconnect_client(QTcpSocket * const p_sock);

		/// @fn	void sftcp_server::send_data(QTcpSocket * const p_sock, const QByteArray& data);
		///
		/// @brief	Sends a data.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param [in,out]	p_sock	If non-null, the sock.
		/// @param 		   	data  	The data.

		void send_data(QTcpSocket * const p_sock, const QByteArray& data);

		/// @fn	std::list<QTcpSocket*> sftcp_server::get_clients() const;
		///
		/// @brief	Gets the clients.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @return	Null if it fails, else the clients.

		std::list<QTcpSocket*> get_clients() const;

		/// @fn	int sftcp_server::get_send_thread_count() const;
		///
		/// @brief	Gets send thread count.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @return	The send thread count.

		int get_send_thread_count() const;

		/// @fn	QTcpServer* sftcp_server::get_server() const;
		///
		/// @brief	Gets the server.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @return	Null if it fails, else the server.

		QTcpServer* get_server() const;

	signals:

		/// @fn	void skyfire::sftcp_server::new_connection(QTcpSocket * const p_sock);
		///
		/// @brief	A new connection comming.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param [in,out]	p_sock	If non-null, the sock.

		void new_connection(QTcpSocket * const p_sock);

		/// @fn	void sftcp_server::data_comming(QTcpSocket * const p_sock, const QByteArray &data);
		///
		/// @brief	Data comming.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param [in,out]	p_sock	If non-null, the sock.
		/// @param 		   	data  	The data.

		void data_comming(QTcpSocket * const p_sock, const QByteArray &data);

		/// @fn	void sftcp_server::client_disconnect(QTcpSocket * const p_sock);
		///
		/// @brief	Client disconnect.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param [in,out]	p_sock	If non-null, the sock.

		void client_disconnect(QTcpSocket * const p_sock);

		/// @fn	void sftcp_server::sock_error(QTcpSocket * const p_sock, QAbstractSocket::SocketError err);
		///
		/// @brief	Sock error.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param [in,out]	p_sock	If non-null, the sock.
		/// @param 		   	err   	The error.

		void sock_error(QTcpSocket * const p_sock, QAbstractSocket::SocketError err);

		/// @fn	void sftcp_server::server_error(QAbstractSocket::SocketError err);
		///
		/// @brief	Server error.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param	err	The error.

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

/// @file	sf_tcp_server.h
///
/// @brief	Declares the sftcp server class.

#pragma once


#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(LIBsf_tcp_server_LIB)
#  define LIBsf_tcp_server_EXPORT Q_DECL_EXPORT
# else
#  define LIBsf_tcp_server_EXPORT Q_DECL_IMPORT
# endif
#else
# define LIBsf_tcp_server_EXPORT
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

	/**
	 * @class	sf_tcp_error_trans
	 *
	 * @brief	A sf TCP error transaction.
	 *
	 * @author	SkyFire
	 * @date	2017/8/16
	 */

	class sf_tcp_error_trans;

	/** @brief	The default thread count. */
	constexpr int default_thread_count__ = 4;

	/**
	 * @class	LIBsf_tcp_server_EXPORT
	 *
	 * @brief	A sftcp server.
	 *
	 * @author	SkyFire
	 * @date	2017/8/16
	 */

	class LIBsf_tcp_server_EXPORT sf_tcp_server : public QObject
	{
		Q_OBJECT

	public:
		/** @brief	The object q object. */
		using QObject::QObject;

		/**
		 * @fn	explicit sf_tcp_server::sf_tcp_server(QObject *parent = nullptr);
		 *
		 * @brief	Constructor.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @param [in,out]	parent	(Optional) If non-null, the parent.
		 */

		explicit sf_tcp_server(QObject *parent = nullptr);

		/**
		 * @fn	sf_tcp_server::~sf_tcp_server();
		 *
		 * @brief	Destructor.
		 *
		 * @author	Sky Fire
		 * @date	2017/8/17
		 */

		~sf_tcp_server();

		/**
		 * @fn	void sf_tcp_server::init(int thread_count = default_thread_count__);
		 *
		 * @brief	Inits the given thread count.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @param	thread_count	(Optional) Number of threads.
		 */

		void init(int thread_count = default_thread_count__);

		/**
		 * @fn	void sf_tcp_server::cleanup();
		 *
		 * @brief	Cleanups this object.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 */

		void cleanup();

		/**
		 * @fn	bool sf_tcp_server::listen(const QHostAddress &host, quint16 port);
		 *
		 * @brief	Listens.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @param	host	The host.
		 * @param	port	The port.
		 *
		 * @return	True if it succeeds, false if it fails.
		 */

		bool listen(const QHostAddress &host, quint16 port);

		/**
		 * @fn	void sf_tcp_server::disconnect_client(QTcpSocket * const p_sock);
		 *
		 * @brief	Disconnects the client described by p_sock.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @param [in,out]	p_sock	If non-null, the sock.
		 */

		void disconnect_client(QTcpSocket * const p_sock);

		/**
		 * @fn	void sf_tcp_server::send_data(QTcpSocket * const p_sock, const QByteArray& data);
		 *
		 * @brief	Sends a data.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @param [in,out]	p_sock	If non-null, the sock.
		 * @param 		  	data  	The data.
		 */

		void send_data(QTcpSocket * const p_sock, const QByteArray& data);

		/**
		 * @fn	std::list<QTcpSocket*> sf_tcp_server::get_clients() const;
		 *
		 * @brief	Gets the clients.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @return	Null if it fails, else the clients.
		 */

		std::list<QTcpSocket*> get_clients() const;

		/**
		 * @fn	int sf_tcp_server::get_send_thread_count() const;
		 *
		 * @brief	Gets send thread count.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @return	The send thread count.
		 */

		int get_send_thread_count() const;

		/**
		 * @fn	QTcpServer* sf_tcp_server::get_server() const;
		 *
		 * @brief	Gets the server.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @return	Null if it fails, else the server.
		 */

		QTcpServer* get_server() const;

	signals:

		/**
		 * @fn	void sf_tcp_server::new_connection(QTcpSocket * const p_sock);
		 *
		 * @brief	A new connection comming.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @param [in,out]	p_sock	If non-null, the sock.
		 */

		void new_connection(QTcpSocket * const p_sock);

		/**
		 * @fn	void sf_tcp_server::data_comming(QTcpSocket * const p_sock, const QByteArray &data);
		 *
		 * @brief	Data comming.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @param [in,out]	p_sock	If non-null, the sock.
		 * @param 		  	data  	The data.
		 */

		void data_comming(QTcpSocket * const p_sock, const QByteArray &data);

		/**
		 * @fn	void sf_tcp_server::client_disconnect(QTcpSocket * const p_sock);
		 *
		 * @brief	Client disconnect.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @param [in,out]	p_sock	If non-null, the sock.
		 */

		void client_disconnect(QTcpSocket * const p_sock);

		/**
		 * @fn	void sf_tcp_server::sock_error(QTcpSocket * const p_sock, QAbstractSocket::SocketError err);
		 *
		 * @brief	Sock error.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @param [in,out]	p_sock	If non-null, the sock.
		 * @param 		  	err   	The error.
		 */

		void sock_error(QTcpSocket * const p_sock, QAbstractSocket::SocketError err);

		/**
		 * @fn	void sf_tcp_server::server_error(QAbstractSocket::SocketError err);
		 *
		 * @brief	Server error.
		 *
		 * @author	SkyFire
		 * @date	2017/8/16
		 *
		 * @param	err	The error.
		 */

		void server_error(QAbstractSocket::SocketError err);

	private:

		/**
		 * @property	QTcpServer *p_server__
		 *
		 * @brief	Gets the server.
		 *
		 * @return	The p server.
		 */

		QTcpServer *p_server__{ nullptr };
		/** @brief	List of socks. */
		std::list<QTcpSocket*> sock_list__;
		/** @brief	List of sock errors. */
		std::map<QTcpSocket*, sf_tcp_error_trans*> sock_err_list__;

		/**
		 * @property	std::deque <std::pair<QTcpSocket*, QByteArray>> send_task_deque__
		 *
		 * @brief	Gets the send task deque.
		 *
		 * @return	The send task deque.
		 */

		std::deque <std::pair<QTcpSocket*, QByteArray>> send_task_deque__;
		/** @brief	List of send threads. */
		std::list<std::shared_ptr<std::thread>> send_thread_list__;
		/** @brief	The is finished. */
		std::atomic<bool> is_finished__;
		/** @brief	The mu send. */
		std::mutex mu_send__;
		/** @brief	The cv send. */
		std::condition_variable cv_send__;

		/**
		 * @property	bool is_listenning__
		 *
		 * @brief	Gets a value indicating whether this object is listenning.
		 *
		 * @return	True if this object is listenning, false if not.
		 */

		bool is_listenning__{ false };

		/**
		 * @property	bool is_init__
		 *
		 * @brief	Gets a value indicating whether this object is init.
		 *
		 * @return	True if this object is init, false if not.
		 */

		bool is_init__{ false };
		/** @brief	The mu deque. */
		std::mutex mu_deque__;

		/**
		 * @property	int send_thread_count__
		 *
		 * @brief	Gets the number of send threads.
		 *
		 * @return	The number of send threads.
		 */

		int send_thread_count__{ 4 };

		/**
		 * @fn	void sf_tcp_server::create_thread__();
		 *
		 * @brief	Creates the thread.
		 *
		 * @author	Sky Fire
		 * @date	2017/8/17
		 */

		void create_thread__();

		/**
		 * @fn	void sf_tcp_server::terminate_thread__();
		 *
		 * @brief	Terminate thread.
		 *
		 * @author	Sky Fire
		 * @date	2017/8/17
		 */

		void terminate_thread__();

		/**
		 * @fn	void sf_tcp_server::close__();
		 *
		 * @brief	Close s this object.
		 *
		 * @author	Sky Fire
		 * @date	2017/8/17
		 */

		void close__();
};

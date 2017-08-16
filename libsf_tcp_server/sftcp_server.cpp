#include "sftcp_server.h"
#include "sf_tcp_error_trans.h"

sftcp_server::sftcp_server(QObject *parent)
	: QObject(parent)
{
}

sftcp_server::~sftcp_server()
{
}

void sftcp_server::init(int thread_count)
{
	if (is_init__)
		return;
	is_init__ = true;
	is_listenning__ = false;
	if (thread_count < 1)
	{
		thread_count = 1;
	}
	else if (thread_count > 32)
	{
		thread_count = 32;
	}
	send_thread_count__ = thread_count;
	p_server__ = new QTcpServer(this);
	connect(p_server__, &QTcpServer::acceptError, this, &sftcp_server::server_error);
	create_thread__();
	connect(p_server__, &QTcpServer::newConnection, [this]()
	{
		while (p_server__->hasPendingConnections())
		{
			auto p_sock = p_server__->nextPendingConnection();
			sock_list__.push_back(p_sock);
			connect(p_sock, &QTcpSocket::readyRead, [this, p_sock]()
			{
				emit data_comming(p_sock, p_sock->readAll());
			});
			connect(p_sock, &QTcpSocket::disconnected, [this, p_sock]()
			{
				disconnect_client(p_sock);
				emit client_disconnect(p_sock);
			});
			auto p_err = new sf_tcp_error_trans(p_sock, &sftcp_server::sock_error, this, p_sock);
			connect(p_sock, SIGNAL(error(QAbstractSocket::SocketError)), p_err, SLOT(on_tcp_error__(QAbstractSocket::SocketError)));
			sock_err_list__[p_sock] = p_err;
			emit new_connection(p_sock);
		}
	});
	is_finished__ = false;
}

void sftcp_server::cleanup()
{
	close__();
	for (auto &p : sock_err_list__)
		p.second->deleteLater();
	sock_err_list__.clear();
	send_task_deque__.clear();
	is_init__ = false;
	send_thread_count__ = default_thread_count__;
}

bool sftcp_server::listen(const QHostAddress& host, quint16 port)
{
	if (is_listenning__)
		return false;
	if (p_server__->listen(host, port))
	{
		is_listenning__ = true;
		return true;
	}
	return false;
}

void sftcp_server::disconnect_client(QTcpSocket* const p_sock)
{
	{
		std::lock_guard<std::mutex> lck(mu_deque__);
		decltype(send_task_deque__.begin()) iter;
		while ((iter = std::find_if(send_task_deque__.begin(), send_task_deque__.end(), [p_sock](decltype(*iter) d)
		{
			return d.first == p_sock;
		})) != send_task_deque__.end())
		{
			send_task_deque__.erase(iter);
		}
	}
	sock_err_list__[p_sock]->deleteLater();
	sock_err_list__.erase(p_sock);
	auto iter = std::find(sock_list__.begin(), sock_list__.end(), p_sock);
	if (iter != sock_list__.end())
	{
		p_sock->close();
		sock_list__.erase(iter);
	}
}

void sftcp_server::send_data(QTcpSocket* const p_sock, const QByteArray& data)
{
	std::lock_guard<std::mutex> lck(mu_deque__);
	send_task_deque__.push_back({ p_sock,data });
	cv_send__.notify_all();
}

std::list<QTcpSocket*> sftcp_server::get_clients() const
{
	return sock_list__;
}

int sftcp_server::get_send_thread_count() const
{
	return send_thread_count__;
}

QTcpServer* sftcp_server::get_server() const
{
	return p_server__;
}

void sftcp_server::close__()
{
	terminate_thread__();
	for (auto &p : sock_list__)
	{
		p->close();
		sock_err_list__[p]->deleteLater();
	}
	sock_list__.clear();
	send_task_deque__.clear();
	sock_err_list__.clear();
	p_server__->close();
	p_server__->deleteLater();
	p_server__ = nullptr;
}

void sftcp_server::create_thread__()
{
	for (auto i = 0; i < 4; ++i)
	{
		auto p_thread = std::make_shared<std::thread>(
			[this]()
		{
			while (true)
			{
				if (send_task_deque__.empty())
				{
					std::unique_lock<std::mutex> lck(mu_send__);
					cv_send__.wait(lck);
				}
				if (!send_task_deque__.empty())
				{
					QTcpSocket *p_sock;
					QByteArray data;
					{
						std::lock_guard<std::mutex> lck(mu_deque__);
						auto iter = send_task_deque__.begin();
						p_sock = iter->first;
						data = iter->second;
						send_task_deque__.pop_front();
					}
					p_sock->write(data);
				}
				if (is_finished__)
					break;
			}
		}
		);
	}
}

void sftcp_server::terminate_thread__()
{
	is_finished__ = true;
	cv_send__.notify_all();
	send_thread_list__.clear();
}
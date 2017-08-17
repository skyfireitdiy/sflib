#pragma once

#include <QObject>
#include <QTcpSocket>

class sf_tcp_server;

class QTcpSocket;

class sf_tcp_error_trans : public QObject
{
	Q_OBJECT

public:
	sf_tcp_error_trans(QTcpSocket * const p_sock, void (sf_tcp_server::*func)(QTcpSocket * const, QAbstractSocket::SocketError), sf_tcp_server *const p_server, QObject *parent = nullptr);
	~sf_tcp_error_trans();

	public slots:
	void on_tcp_error__(QAbstractSocket::SocketError err);

private:
	QTcpSocket *p_sock__{ nullptr };
	void(sf_tcp_server::*call_back__)(QTcpSocket * const, QAbstractSocket::SocketError);
	sf_tcp_server *p_server__;
};

#include "sf_tcp_error_trans.h"

sf_tcp_error_trans::sf_tcp_error_trans(QTcpSocket* const p_sock, void(sf_tcp_server::*func)(QTcpSocket* const, QAbstractSocket::SocketError), sf_tcp_server *p_server, QObject* parent) :
	p_sock__(p_sock), call_back__(func), p_server__(p_server), QObject(parent)
{
}

sf_tcp_error_trans::~sf_tcp_error_trans()
{
}

void sf_tcp_error_trans::on_tcp_error__(QAbstractSocket::SocketError err)
{
	emit(p_server__->*call_back__)(p_sock__, err);
}
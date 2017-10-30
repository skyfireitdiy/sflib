#include "sf_debug.h"

using namespace skyfire;
sf_debug<> sflog;


int main()
{
	//下面两句顺序不能颠倒
	//打开日志开关
	sflog.set_out_to_file(true);
	//设置日志文件
	sflog.set_debug_file("runlog.txt");
	sflog.sf_logout("hello", "这是多条演示", 666, 5.6, 5.7f, 9L);
	sflog.sf_logout(5);
	sflog.sf_track("打印在调试器中", 666);
}